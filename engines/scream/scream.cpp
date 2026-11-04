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
#include "common/events.h"
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

void ScreamEngine::loadNode(const Node *node) {
	if (!node)
		error("ScreamEngine::loadNode: node not provided");

	for (GLuint faceTexture : _faceTextures)
		_gfx->deleteTexture(faceTexture);

	const char *faceFiles[kNumFaces] = {
		node->getFace(kScFront).c_str(),
		node->getFace(kScBack).c_str(),
		node->getFace(kScLeft).c_str(),
		node->getFace(kScRight).c_str(),
		node->getFace(kScUp).c_str(),
		node->getFace(kScDown).c_str(),
	};

	for (uint i = 0; i < kNumFaces; ++i)
		_faceTextures[i] = loadTexture(_gfx, faceFiles[i]);

	_loadedNode = node;
}

void ScreamEngine::loadZoom(const Zoom *zoom) {
	if (!zoom)
		error("ScreamEngine::loadZoom: zoom not provided");
	_gfx->deleteTexture(_zoomTexture);
	_zoomTexture = loadTexture(_gfx, zoom->getImageFile());
	_loadedZoom = zoom;
}

void ScreamEngine::performAction(const Action &action) {
	switch (action._kind) {
	case kScNode:
		_world.switchNode(action._arg1);
		break;
	case kScRoom:
		_world.switchRoom(action._arg1);
		break;
	case kScZoom:
		_world.switchZoom(action._arg1);
		break;
	case kScCustom:
		_script->callFunction(action._arg1);
		if (action._arg2.size() > 0)
			warning("performAction: custom action with arg2 not implemented: %s", action._arg2.c_str());
		break;
	case kScZoomOut:
		_world.zoomOut();
		break;
	case kScZoomBack:
		_world.zoomBack();
		break;
	case kScFeedbackEx:
	default:
		warning("click action type not implemented: %d", action._kind);
		break;
	}
}

void ScreamEngine::updateCamera(const Common::Point &mouse) {
	if (!_script->isSleeping() && !_world.getCurrentZoom())
		_camera.update(mouse);
}

const Object *ScreamEngine::findHoverObject(const Common::Point &mouse) {
	if (_world.getCurrentZoom()) {
		return _world.getCurrentZoom()->findObjectAt(mouse);
	} else {
		FaceHit fh = _camera.pick(mouse);
		if (fh.isValid())
			return _world.getCurrentNode()->findObjectAt(fh.face, fh.pixel);
	}
	return nullptr;
}

void ScreamEngine::loadScene() {
	if (_loadedNode != _world.getCurrentNode())
		loadNode(_world.getCurrentNode());

	const Zoom *currentZoom = _world.getCurrentZoom();
	if (currentZoom && _loadedZoom != currentZoom)
		loadZoom(currentZoom);
}

void ScreamEngine::drawScene(const Common::Point &mouse) {
	_gfx->clear();

	if (!_world.getCurrentZoom()) {
		_gfx->setup3D(_camera.getHeading(), _camera.getPitch(), _camera.getFov());
		_gfx->drawCube(_faceTextures);
		_gfx->setup2D();
	} else {
		_gfx->setup2D();
		_gfx->drawTexture2D(_zoomTexture, 0, kViewportY, kScreenWidth, kViewportHeight);
	}

	_cursor.draw(_gfx, mouse);
}

int ScreamEngine::chooseCursor(const Object *hoverObject) const {
	if (_script->isWaiting())
		return kScWait;
	if (hoverObject)
		return hoverObject->getHoverCursor();
	return kScNormal;
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

	// set the cursor
	g_system->warpMouse(kScreenWidth / 2, kScreenHeight / 2);
	Common::Point mouse(kScreenWidth / 2, kScreenHeight / 2);

	// Set the engine's debugger console
	setDebugger(new Console());

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot != -1)
		(void)loadGameState(saveSlot);

	Common::Event e;
	bool clicked = false;

	Node *tempNode = _world.findNode(_world.findRoom("house-living"), "n1");
	_world.setCurrentNode(tempNode);

	debug(1, "openGL error: %d", glGetError());

	Graphics::FrameLimiter limiter(g_system, 60);

	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
			switch (e.type) {
			case Common::EVENT_MOUSEMOVE:
				mouse = e.mouse;
				break;
			case Common::EVENT_LBUTTONUP:
				clicked = true;
				break;
			case Common::EVENT_KEYDOWN:
				break;
			default:
				break;
			}
		}

		_script->update();
		updateCamera(mouse);

		const Object *hoverObject = findHoverObject(mouse);
		_cursor.setCursor(_gfx, chooseCursor(hoverObject));

		if (clicked && hoverObject && !_script->isWaiting())
			performAction(hoverObject->getClickAction());

		loadScene();
		drawScene(mouse);

		clicked = false;

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		limiter.delayBeforeSwap();
		g_system->updateScreen();
		limiter.startFrame();
	}

	for (GLuint texture : _faceTextures)
		_gfx->deleteTexture(texture);
	_cursor.freeTexture(_gfx);
	_gfx->deleteTexture(_zoomTexture);

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
