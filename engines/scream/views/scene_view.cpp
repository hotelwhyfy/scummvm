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

void SceneView::loadBackdrop(Gfx *gfx) {
	_whiteTexture = loadSolid(gfx, 255, 255, 255);
	_rainTexture = loadTexture(gfx, kRainImage);
	for (uint i = 0; i < kNumFaces; ++i) {
		_backdropTextures[i] = _whiteTexture;
	}
	_backdropTextures[kScDown - 1] = _rainTexture;
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
		loadBackdrop(gfx);

	gfx->clear();

	if (currentNode && !currentZoom) {
		gfx->setup3D(camera.getHeading(), camera.getPitch(), camera.getFov());
		if (world.getAtmosphere().sky) {
			gfx->setBlend(kBlendAdditive);
			gfx->drawCube(_backdropTextures, Color(0.9f, 0.9f, 0.9f));
			gfx->setBlend(kBlendMask);
			gfx->drawCube(_maskTextures);
			gfx->setBlend(kBlendMultiply);
		}
		gfx->drawCube(_faceTextures);
		gfx->setBlend(kBlendNormal);
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
	_zoomTexture = 0;
	_whiteTexture = 0;
	_rainTexture = 0;
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
