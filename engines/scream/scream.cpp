/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "scream/scream.h"

#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/file.h"
#include "common/scummsys.h"
#include "common/system.h"
#include "graphics/framelimiter.h"
#include "graphics/opengl/system_headers.h"

#include "scream/console.h"
#include "scream/constants.h"
#include "scream/detection.h"
#include "scream/gfx.h"
#include "scream/image.h"
#include "scream/resources.h"
#include "scream/script.h"

namespace Scream {

ScreamEngine *g_engine;

void ScreamEngine::performAction(const Action &action) {
	switch (action._kind) {
	case kScNode:
	case kScRoom:
	case kScZoom:
	case kScZoomOut:
	case kScZoomBack:
		_world.switchTo(action._kind, action._arg1);
		break;
	case kScCustom:
		_script->callFunction(action._arg1);
		if (action._arg2.size() > 0)
			warning("performAction: custom action with arg2 not implemented: %s", action._arg2.c_str());
		break;
	case kScFeedback:
		showFeedback(action._arg1);
		break;
	case kScFeedbackEx:
		showFeedback(action._arg1);
		warning("click action kScFeedbackEx does not handle audio");
		break;
	default:
		warning("click action type not implemented: %d", action._kind);
		break;
	}
}

void ScreamEngine::showFeedback(const Common::String &text) {
	_feedback.set(text, _settings._textSpeed);
}

void ScreamEngine::updateCamera(const Common::Point &mouse) {
	if (!_script->isSleeping() && !_world.getCurrentZoom())
		_camera.update(mouse);
}

void ScreamEngine::updateCursor(int hoverCursor) {
	int cursor = hoverCursor;
	if (_script->isWaiting())
		cursor = kScWait;
	_cursor.setCursor(_gfx, cursor);
}

static bool actionHappensInPlace(int actionType) {
	return (actionType == kScFeedback || actionType == kScFeedbackEx || actionType == kScCustom);
}

void ScreamEngine::handleSceneInput() {
	updateCamera(_mouse);

	Object *hoverObject = _sceneView.findObjectAt(_world, _camera, _mouse);

	if (hoverObject && !_script->isWaiting())
		_world.setBoundObject(hoverObject);

	updateCursor(hoverObject ? hoverObject->getHoverCursor() : kScNormal);

	if (!_clicked)
		return;

	const Item *held = _inventory.getHeld();

	if (!_sceneView.isInBand(_mouse))
		return;

	_inventory.drop();

	if (!hoverObject)
		return;

	if (!held) {
		performAction(hoverObject->getClickAction());
		return;
	}

	if (!actionHappensInPlace(hoverObject->getClickAction()._kind))
		return;

	const Action *reaction = hoverObject->findReaction(held);
	if (reaction)
		performAction(*reaction);
	else
		showFeedback(held->getFailText());
}

void ScreamEngine::handleInventoryInput() {
	Item *hoverItem = _inventoryView.findItemAt(_inventory, _mouse);

	if (hoverItem)
		_world.setBoundItem(hoverItem);

	updateCursor(hoverItem ? kScUse : kScNormal);

	if (!_clicked)
		return;

	const Item *held = _inventory.getHeld();

	if (!held && hoverItem) {
		_inventory.grab(hoverItem);
		return;
	}

	if (held && hoverItem && hoverItem != held) {
		const Action *reaction = hoverItem->findReaction(held);
		if (reaction)
			performAction(*reaction);
		else
			showFeedback(held->getFailText());
	}

	_dropWhenIdle = true;
}

void ScreamEngine::handleEvents(Common::Event &e) {
	while (g_system->getEventManager()->pollEvent(e)) {

		if (_script->isBusy())
			continue;

		switch (e.type) {
		case Common::EVENT_MOUSEMOVE:
			_mouse = e.mouse;
			break;
		case Common::EVENT_LBUTTONUP:
			_clicked = true;
			break;
		case Common::EVENT_RBUTTONUP:
			if (_inventory.count() > 0 || _inventoryOpen)
				_inventoryOpen = !_inventoryOpen;
			break;
		case Common::EVENT_KEYDOWN:
			break;
		default:
			break;
		}
	}
}

void ScreamEngine::freeTextures() {
	_cursor.freeTexture(_gfx);
	_font.freeTexture(_gfx);
	_inventoryView.freeTextures(_gfx);
	_sceneView.freeTextures(_gfx);
}

ScreamEngine::ScreamEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
																			   _gameDescription(gameDesc), _randomSource("Scream") {
	g_engine = this;
}

ScreamEngine::~ScreamEngine() {
	delete _script;
	delete _gfx;
}

uint32 ScreamEngine::getFeatures() const {
	return _gameDescription->flags;
}

Common::String ScreamEngine::getGameId() const {
	return _gameDescription->gameId;
}

Common::Error ScreamEngine::run() {
	// the original expects update.res to be merged into resources.res
	if (Common::File::exists("update.res")) {
		Res *updateRes = new Res();
		if (!updateRes->open("update.res"))
			error("Unable to load update.res");
		SearchMan.add("update.res", updateRes, 1);
	}

	_script = new Script();
	_script->runFile("scream.sc");
	_script->runFile("scratches.sc");

	_gfx = new Gfx();

	_cursor.setCursor(_gfx, kScNormal);

	g_system->warpMouse(_mouse.x, _mouse.y);

	// Set the engine's debugger console
	setDebugger(new Console());

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot != -1)
		(void)loadGameState(saveSlot);

	Common::Event e;

	_world.switchTo(kScRoom, "garden");

	debug(1, "openGL error: %d", glGetError());

	Graphics::FrameLimiter limiter(g_system, 60);

	while (!shouldQuit()) {
		handleEvents(e);

		_script->update();

		if (_inventoryOpen)
			handleInventoryInput();
		else
			handleSceneInput();

		if (_dropWhenIdle && !_script->isBusy()) {
			_inventory.drop();
			_dropWhenIdle = false;
		}

		_sceneView.draw(_gfx, _world, _camera);

		if (_inventoryOpen)
			_inventoryView.draw(_gfx, _inventory);

		_feedback.draw(_gfx, _font, _settings._textSpeed);

		if (_inventory.getHeld() && !_script->isWaiting())
			_inventoryView.drawHeld(_gfx, _inventory, _mouse);
		else
			_cursor.draw(_gfx, _mouse);

		_clicked = false;

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		limiter.delayBeforeSwap();
		g_system->updateScreen();
		limiter.startFrame();
	}

	freeTextures();

	return Common::kNoError;
}

Common::Error ScreamEngine::syncGame(Common::Serializer &s) {
	// The Serializer has methods isLoading() and isSaving()
	// if you need to specific steps; for example setting
	// an array size after reading it's length, whereas
	// for saving it would write the existing array's length
	int dummy = 0;
	s.syncAsUint32LE(dummy);

	return Common::kNoError;
}

} // End of namespace Scream
