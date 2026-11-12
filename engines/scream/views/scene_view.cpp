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

typedef const float *(*FindColors)(int preset);

struct AmbienceColors {
	int preset;
	float values[ColorFade::kMaxChannels];
};

static const AmbienceColors kSkyPresets[] = { // sky + horizon
	{kScDawn, {1.0f, 0.8f, 0.55f, 0.8f, 0.05f, 0.05f}},
	{kScMorning, {0.9f, 0.9f, 0.9f, 0.55f, 0.65f, 0.8f}},
	{kScDusk, {0.45f, 0.5f, 0.6f, 1.0f, 0.0f, 0.0f}},
	{kScStorm, {0.7f, 0.7f, 0.9f, 0.15f, 0.15f, 0.8f}},
};

static const AmbienceColors kScreenPresets[] = { // screen rgba
	{kScDawn, {1.0f, 0.55f, 0.55f, 0.025f}},
	{kScMorning, {1.0f, 1.0f, 1.0f, 0.0f}},
	{kScDusk, {1.0f, 0.25f, 0.25f, 0.02f}},
	{kScStorm, {0.0f, 0.0f, 0.15f, 0.15f}},
};

static const float *findSkyColors(int preset) {
	for (const AmbienceColors &entry : kSkyPresets)
		if (entry.preset == preset)
			return entry.values;
	warning("findSkyColors preset not found %d", preset);
	return kSkyPresets[1].values; // morning
}

static const float *findScreenColors(int preset) {
	for (const AmbienceColors &entry : kScreenPresets)
		if (entry.preset == preset)
			return entry.values;
	warning("findScreenColors preset not found %d", preset);
	return kScreenPresets[1].values; // morning
}

static const float *updateAmbience(ColorFade &fade, int &preset, int target, FindColors find) {
	if (preset != target && fade.update(find(preset), find(target)))
		preset = target;
	return (preset != target) ? fade.getValues() : find(target);
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
			const float *sky = updateAmbience(_skyFade, _skyPreset, world.getAtmosphere().skyPreset, findSkyColors);
			gfx->setBlend(kBlendAdditive);
			gfx->drawCube(_skyTextures, sky);
			gfx->setBlend(kBlendNormal);
			gfx->drawCube(_horizonTextures, sky + 3);
			gfx->setBlend(kBlendMask);
			gfx->drawCube(_maskTextures);
			gfx->setBlend(kBlendMultiply);
		}
		gfx->drawCube(_faceTextures);
		gfx->setBlend(kBlendNormal);

		const float *screen = updateAmbience(_screenFade, _screenPreset, world.getAtmosphere().screenPreset, findScreenColors);
		gfx->drawCube(_maskTextures, screen, screen[3]);
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
