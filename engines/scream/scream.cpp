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
#include "image/jpeg.h"
#include "math/utils.h"

#include "scream/console.h"
#include "scream/constants.h"
#include "scream/detection.h"
#include "scream/gfx.h"
#include "scream/resources.h"
#include "scream/script.h"

namespace Scream {

ScreamEngine *g_engine;

static const int kCameraSpeed = 50;
static const float kCamFov = 45.0f;
static const float kLookYLimit = 1.75f;
static const int kTurnZonePercent = 15;
static const int kMouseLeftZoneEdge = kScreenWidth * kTurnZonePercent / 100;
static const int kMouseRightZoneEdge = kScreenWidth - kMouseLeftZoneEdge;
static const int kMouseUpZoneEdge = 64 + (kScreenHeight - 192) * kTurnZonePercent / 100;
static const int kMouseDownZoneEdge = (kScreenHeight - 128) - (kScreenHeight - 192) * kTurnZonePercent / 100;
static const float kCamSpeedScale = 0.001f;
static const float kTurnPixelFactor = 0.005f;
static const float kTurnSpeedRad = (kTurnPixelFactor * kCameraSpeed * kCamSpeedScale); // radians per pixel-past-zone-edge per frame
static const float kTurnSpeedDeg = kTurnSpeedRad * (180.0f / (float)M_PI);             // degrees per pixel-past-zone-edge per frame

static GLuint loadTexture(Gfx *gfx, const char *fileName, const Graphics::PixelFormat &format) {
	Common::File file;
	if (!file.open(fileName))
		error("Could not open file: %s", fileName);

	Image::JPEGDecoder decoder; // pick decoder based on file ext (tga, jpeg)
	decoder.setOutputPixelFormat(format);
	if (!decoder.loadStream(file))
		error("could not load image stream: %s", fileName);

	const Graphics::Surface *surface = decoder.getSurface();
	return gfx->createTexture(surface);
}

void ScreamEngine::loadNode(Node *node) {
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
		_faceTextures[i] = loadTexture(_gfx, faceFiles[i], _pixelFormat);
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
	// load update.res if it exists
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

	// set the cursor
	g_system->warpMouse(kScreenWidth / 2, kScreenHeight / 2);
	_cursor.setCursor(kScNormal);
	Common::Point mouse(kScreenWidth / 2, kScreenHeight / 2);

	// Set the engine's debugger console
	setDebugger(new Console());

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot != -1)
		(void)loadGameState(saveSlot);

	Common::Event e;

	uint nodeIndex = 0;
	Node *node = _world.getNode(nodeIndex);
	loadNode(node);

	debug(1, "openGL error: %d", glGetError());

	float heading = 0.0f;
	float pitch = 0.0f;
	float lookY = 0.0f;

	Graphics::FrameLimiter limiter(g_system, 60);

	while (!shouldQuit()) {
		debug(1, "node=%d", nodeIndex);
		while (g_system->getEventManager()->pollEvent(e)) {
			switch (e.type) {
			case Common::EVENT_MOUSEMOVE:
				mouse = e.mouse;
				break;
			case Common::EVENT_KEYDOWN:
				if (e.kbd.keycode == Common::KEYCODE_SPACE) {
					nodeIndex += 1;
					node = _world.getNode(nodeIndex);
					if (!node) {
						nodeIndex = 0;
						node = _world.getNode(nodeIndex);
					}
					loadNode(node);
				}
				break;
			default:
				break;
			}
		}
		if (mouse.x < kMouseLeftZoneEdge) {
			heading -= (kMouseLeftZoneEdge - mouse.x) * kTurnSpeedDeg;
			if (heading < 0)
				heading += 360;
		} else if (mouse.x > kMouseRightZoneEdge) {
			heading += (mouse.x - kMouseRightZoneEdge) * kTurnSpeedDeg;
			if (heading >= 360)
				heading -= 360;
		}

		if (mouse.y < kMouseUpZoneEdge && lookY <= kLookYLimit) {
			lookY += (kMouseUpZoneEdge - mouse.y) * kTurnSpeedRad;
		} else if (mouse.y > kMouseDownZoneEdge && lookY >= -kLookYLimit) {
			lookY -= (mouse.y - kMouseDownZoneEdge) * kTurnSpeedRad;
		}

		pitch = Math::rad2deg(atan(lookY));

		_gfx->clear();
		_gfx->setupCamera(heading, pitch, kCamFov);
		_gfx->drawCube(_faceTextures);

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		limiter.delayBeforeSwap();
		g_system->updateScreen();
		limiter.startFrame();
	}

	for (GLuint texture : _faceTextures)
		_gfx->deleteTexture(texture);

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
