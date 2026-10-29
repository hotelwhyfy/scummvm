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
#include "engines/util.h"
#include "graphics/framelimiter.h"
#include "graphics/opengl/system_headers.h"
#include "graphics/paletteman.h"
#include "image/jpeg.h"
#include "math/utils.h"
#include "scream/console.h"
#include "scream/detection.h"

#include "scream/script.h"

namespace Scream {

ScreamEngine *g_engine;

static const float kCubeVertices[kNumFaces][4][3] = {
	// front
	{
		{-1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
	},
	// back
	{
		{1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
	},
	// left
	{
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, -1.0f},
	},
	// right
	{
		{1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, 1.0f},
	},
	// up
	{
		{-1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
	},
	// down
	{
		{-1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
	},
};
static const float kFaceTexCoords[4][2] = {
	{0.0f, 1.0f},
	{1.0f, 1.0f},
	{0.0f, 0.0f},
	{1.0f, 0.0f},
};

static const int kScreenWidth = 1024;
static const int kScreenHeight = 768;
static const int kCameraSpeed = 50;
static const float kScreenAspect = (float)kScreenWidth / (float)kScreenHeight;

static const float kCamFov = 45.0f;
static const float kScreenNear = 0.1f;
static const float kScreenFar = 100.0f;

static const int kTurnZonePercent = 15;
static const int kMouseLeftZoneEdge = kScreenWidth * kTurnZonePercent / 100;
static const int kMouseRightZoneEdge = kScreenWidth - kMouseLeftZoneEdge;
static const float kCamSpeedScale = 0.001f;
static const float kTurnPixelFactor = 0.005f;
static const float kTurnSpeed = (kTurnPixelFactor * kCameraSpeed * kCamSpeedScale) * (180.0f / (float)M_PI); // degrees per pixel-past-zone-edge per frame

static GLuint loadTexture(const char *fileName, const Graphics::PixelFormat &format) {
	Common::File file;
	if (!file.open(fileName)) {
		error("Could not open file: %s", fileName);
	}
	Image::JPEGDecoder decoder; // pick decoder based on file ext (tga, jpeg)
	decoder.setOutputPixelFormat(format);
	if (!decoder.loadStream(file)) {
		error("could not load image stream: %s", fileName);
	}
	const Graphics::Surface *surface = decoder.getSurface();
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->getPixels());
	return textureId;
}

ScreamEngine::ScreamEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
																			   _gameDescription(gameDesc), _randomSource("Scream") {
	g_engine = this;
}

ScreamEngine::~ScreamEngine() {
	delete _script;
}

uint32 ScreamEngine::getFeatures() const {
	return _gameDescription->flags;
}

Common::String ScreamEngine::getGameId() const {
	return _gameDescription->gameId;
}

Common::Error ScreamEngine::run() {
	_script = new Script();
	_script->runFile("scream.sc");
	_script->runFile("scratches.sc");

	// Initialize graphics mode
	initGraphics3d(1024, 768);

	// Set the engine's debugger console
	setDebugger(new Console());

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot != -1)
		(void)loadGameState(saveSlot);

	Common::Event e;

	const char *faceFiles[kNumFaces] = {
		"hs-lobby_dc_n1f.jpg",
		"hs-lobby_dc_n1b.jpg",
		"hs-lobby_dc_n1l.jpg",
		"hs-lobby_dc_n1r.jpg",
		"hs-lobby_dc_n1u.jpg",
		"hs-lobby_dc_n1d.jpg",
	};
	GLuint faceTextures[kNumFaces];
	for (uint i = 0; i < kNumFaces; ++i) {
		faceTextures[i] = loadTexture(faceFiles[i], _pixelFormat);
	}

	debug(1, "openGL error: %d", glGetError());

	float heading = 0.0f;
	float pitch = 0.0f;
	Common::Point mouse;

	Graphics::FrameLimiter limiter(g_system, 60);

	const float kFrustumTop = kScreenNear * tan(Math::deg2rad(kCamFov) / 2);
	const float kFrustumRight = kFrustumTop * kScreenAspect;
	const float kFrustumBottom = -kFrustumTop;
	const float kFrustumLeft = -kFrustumRight;

	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
			switch (e.type) {
			case Common::EVENT_MOUSEMOVE:
				mouse = e.mouse;
				break;
			default:
				break;
			}
		}
		if (mouse.x < kMouseLeftZoneEdge) {
			heading -= (kMouseLeftZoneEdge - mouse.x) * kTurnSpeed;
		} else if (mouse.x > kMouseRightZoneEdge) {
			heading += (mouse.x - kMouseRightZoneEdge) * kTurnSpeed;
		}
		if (heading < 0)
			heading += 360;
		if (heading >= 360)
			heading -= 360;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 1.0f);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(kFrustumLeft, kFrustumRight, kFrustumBottom, kFrustumTop, kScreenNear, kScreenFar);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(pitch, 1.0f, 0.0f, 0.0f);
		glRotatef(heading, 0.0f, 1.0f, 0.0f);

		for (uint face = 0; face < kNumFaces; ++face) {
			glBindTexture(GL_TEXTURE_2D, faceTextures[face]);
			glBegin(GL_TRIANGLE_STRIP);
			for (uint corner = 0; corner < 4; ++corner) {
				glTexCoord2f(kFaceTexCoords[corner][0], kFaceTexCoords[corner][1]);
				glVertex3f(kCubeVertices[face][corner][0], kCubeVertices[face][corner][1], kCubeVertices[face][corner][2]);
			}
			glEnd();
		}

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		limiter.delayBeforeSwap();
		g_system->updateScreen();
		limiter.startFrame();
	}

	glDeleteTextures(kNumFaces, faceTextures);

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
