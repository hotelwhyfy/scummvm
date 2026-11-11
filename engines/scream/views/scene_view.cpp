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

#include "scream/views/scene_view.h"
#include "scream/constants.h"
#include "scream/gfx.h"
#include "scream/image.h"
#include "scream/world/node.h"
#include "scream/world/zoom.h"

namespace Scream {

const static float kZoomViewScale = kScreenWidth / 800.0f; // zooms have a width of 800px stretched to 1024px

struct SkyPreset {
	int preset;
	float sky[3];
	float horizon[3];
};

struct ScreenPreset {
	int preset;
	float screen[4];
};

static const SkyPreset kSkyPresets[] = {
	{kScDawn, {1.0f, 0.8f, 0.55f}, {0.8f, 0.05f, 0.05f}},
	{kScMorning, {0.9f, 0.9f, 0.9f}, {0.55f, 0.65f, 0.8f}},
	{kScDusk, {0.45f, 0.5f, 0.6f}, {1.0f, 0.0f, 0.0f}},
	{kScStorm, {0.7f, 0.7f, 0.9f}, {0.15f, 0.15f, 0.8f}},
};

static const ScreenPreset kScreenPresets[] = {
	{kScDawn, {1.0f, 0.55f, 0.55f, 0.025f}},
	{kScMorning, {1.0f, 1.0f, 1.0f, 0.0f}},
	{kScDusk, {1.0f, 0.25f, 0.25f, 0.02f}},
	{kScStorm, {0.0f, 0.0f, 0.15f, 0.15f}},
};

static const SkyPreset &findSkyPreset(int preset) {
	for (const SkyPreset &entry : kSkyPresets)
		if (entry.preset == preset)
			return entry;
	warning("findSkyPreset preset not found %d", preset);
	return kSkyPresets[1]; // morning
}

static const ScreenPreset &findScreenPreset(int preset) {
	for (const ScreenPreset &entry : kScreenPresets)
		if (entry.preset == preset)
			return entry;
	warning("findScreenPreset preset not found %d", preset);
	return kScreenPresets[1]; // morning
}

static void stepChannel(float &value, float target, bool &arrived, float step) {
	if (target > value)
		value += step;
	else if (target < value)
		value -= step;
	if (fabs(target - value) < 0.001)
		arrived = true;
}

void SceneView::updateSkyFade(int target) {
	if (_skySettled) {
		const SkyPreset &current = findSkyPreset(_skyPreset);
		_skyColor = current.sky;
		_horizonColor = current.horizon;
	}

	if (_skyPreset == target)
		return;

	if (_skySettled) {
		for (uint i = 0; i < ARRAYSIZE(_skyArrived); ++i)
			_skyArrived[i] = false;
		_skySettled = false;
	}

	const SkyPreset &goal = findSkyPreset(target);
	float *sky = _skyColor.getData();
	float *horizon = _horizonColor.getData();
	for (uint i = 0; i < ARRAYSIZE(_skyArrived); ++i) {
		stepChannel(sky[i], goal.sky[i], _skyArrived[i], 0.0001f);
		stepChannel(horizon[i], goal.horizon[i], _skyArrived[i + 3], 0.0001f);
	}

	bool allArrived = true;
	for (bool arrived : _skyArrived)
		allArrived = allArrived && arrived;

	if (allArrived) {
		_skySettled = true;
		_skyPreset = target;
	}
}

void SceneView::updateScreenFade(int target) {
	if (_screenSettled) {
		const ScreenPreset &current = findScreenPreset(_screenPreset);
		for (uint i = 0; i < ARRAYSIZE(_screenColor); ++i)
			_screenColor[i] = current.screen[i];
	}

	if (_screenPreset == target)
		return;

	const ScreenPreset &goal = findScreenPreset(target);

	if (_screenSettled) {
		for (uint i = 0; i < ARRAYSIZE(_screenArrived); ++i) {
			_screenStep[i] = fabs(goal.screen[i] - _screenColor[i]) * 10.0 * 0.0001;
			_screenArrived[i] = false;
		}
		_screenSettled = false;
	}

	for (uint i = 0; i < ARRAYSIZE(_screenColor); ++i)
		stepChannel(_screenColor[i], goal.screen[i], _screenArrived[i], _screenStep[i]);

	bool allArrived = true;
	for (bool arrived : _screenArrived)
		allArrived = allArrived && arrived;

	if (allArrived) {
		_screenSettled = true;
		_screenPreset = target;
	}
}

void SceneView::snapAmbience(int target, int preset) {
	switch (target) {
	case kScSkyColor:
		_skyPreset = preset;
		break;
	case kScScreenColor:
		_screenPreset = preset;
		break;
	default:
		warning("SceneView::snapAmbience: target case not implemented %d", target);
	}
}

void SceneView::loadNode(Gfx *gfx, const Node *node) {
	if (!node)
		error("SceneView::loadNode: node not provided");

	freeNodeTextures(gfx);

	for (uint i = 0; i < kNumFaces; ++i) {
		_faceTextures[i] = loadTexture(gfx, node->getFace(i + 1).c_str());
		_maskTextures[i] = loadTexture(gfx, node->getMask(i + 1).c_str());
	}

	_loadedNode = node;
}

void SceneView::loadSky(Gfx *gfx) {
	_whiteTexture = loadSolid(gfx, 255, 255, 255);
	_transparentTexture = loadSolid(gfx, 0, 0, 0, 0);
	_rainTexture = loadTexture(gfx, kRainImage);
	_gradientTexture = loadTexture(gfx, kGradientImage);
	for (uint i = 0; i < kNumFaces; ++i) {
		_skyTextures[i] = _whiteTexture;
		_horizonTextures[i] = _gradientTexture;
	}
	_skyTextures[kScDown - 1] = _rainTexture;
	_horizonTextures[kScUp - 1] = _transparentTexture;
	_horizonTextures[kScDown - 1] = _whiteTexture;
}

void SceneView::loadZoom(Gfx *gfx, const Zoom *zoom) {
	if (!zoom)
		error("SceneView::loadZoom: zoom not provided");
	gfx->deleteTexture(_zoomTexture);
	_zoomTexture = loadTexture(gfx, zoom->getImageFile());
	_loadedZoom = zoom;
}

void SceneView::draw(Gfx *gfx, World &world, const Camera &camera) {
	const Node *currentNode = world.getCurrentNode();
	if (currentNode && _loadedNode != currentNode)
		loadNode(gfx, currentNode);

	const Zoom *currentZoom = world.getCurrentZoom();
	if (currentZoom && _loadedZoom != currentZoom)
		loadZoom(gfx, currentZoom);

	if (_whiteTexture == 0)
		loadSky(gfx);

	gfx->clear();

	if (currentNode && !currentZoom) {
		gfx->setup3D(camera.getHeading(), camera.getPitch(), camera.getFov());
		if (world.getAtmosphere().sky) {
			int skyTarget = world.getAtmosphere().skyPreset;
			updateSkyFade(skyTarget);
			const SkyPreset &skyGoal = findSkyPreset(skyTarget);
			const float *skyColor = (_skyPreset != skyTarget) ? _skyColor.getData() : skyGoal.sky;
			const float *horizonColor = (_skyPreset != skyTarget) ? _horizonColor.getData() : skyGoal.horizon;
			gfx->setBlend(kBlendAdditive);
			gfx->drawCube(_skyTextures, skyColor);
			gfx->setBlend(kBlendNormal);
			gfx->drawCube(_horizonTextures, horizonColor);
			gfx->setBlend(kBlendMask);
			gfx->drawCube(_maskTextures);
			gfx->setBlend(kBlendMultiply);
		}
		gfx->drawCube(_faceTextures);
		gfx->setBlend(kBlendNormal);
		int screenTarget = world.getAtmosphere().screenPreset;
		updateScreenFade(screenTarget);
		const ScreenPreset &screenGoal = findScreenPreset(screenTarget);
		const float *screenColor = (_screenPreset != screenTarget) ? _screenColor : screenGoal.screen;
		gfx->drawCube(_maskTextures, screenColor, screenColor[3]);
	}

	gfx->setup2D();

	if (currentZoom) {
		gfx->drawTexture2D(_zoomTexture, 0, kViewportY, kScreenWidth, kViewportHeight);
	}
}

void SceneView::invalidate() {
	_loadedNode = nullptr;
	_loadedZoom = nullptr;
}

void SceneView::freeNodeTextures(Gfx *gfx) {
	for (GLuint texture : _faceTextures)
		gfx->deleteTexture(texture);
	for (GLuint texture : _maskTextures)
		gfx->deleteTexture(texture);
}

void SceneView::freeTextures(Gfx *gfx) {
	freeNodeTextures(gfx);
	gfx->deleteTexture(_zoomTexture);
	gfx->deleteTexture(_whiteTexture);
	gfx->deleteTexture(_rainTexture);
	gfx->deleteTexture(_gradientTexture);
	gfx->deleteTexture(_transparentTexture);
	_zoomTexture = 0;
	_whiteTexture = 0;
	_rainTexture = 0;
	_gradientTexture = 0;
	_transparentTexture = 0;
}

Object *SceneView::findObjectAt(const World &world, const Camera &camera, const Common::Point &mouse) const {
	if (!isInBand(mouse))
		return nullptr;

	if (world.getCurrentZoom()) {
		float x = mouse.x / kZoomViewScale;
		float y = mouse.y / kZoomViewScale - kViewportY;
		Common::Point p(x, y);
		return world.getCurrentZoom()->findObjectAt(p);
	}

	FaceHit fh = camera.pick(mouse);
	return world.getCurrentNode()->findObjectAt(fh.face, fh.pixel);
}

bool SceneView::isInBand(const Common::Point &mouse) {
	return mouse.y > (int)kViewportY && mouse.y < (int)(kViewportY + kViewportHeight);
}

} // end of namespace Scream
