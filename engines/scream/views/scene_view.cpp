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
#include "scream/node.h"
#include "scream/zoom.h"

namespace Scream {

const static float kZoomViewScale = kScreenWidth / 800.0f; // zooms have a width of 800px stretched to 1024px

void SceneView::loadNode(Gfx *gfx, const Node *node) {
	if (!node)
		error("SceneView::loadNode: node not provided");

	for (GLuint faceTexture : _faceTextures)
		gfx->deleteTexture(faceTexture);

	const char *faceFiles[kNumFaces] = {
		node->getFace(kScFront).c_str(),
		node->getFace(kScBack).c_str(),
		node->getFace(kScLeft).c_str(),
		node->getFace(kScRight).c_str(),
		node->getFace(kScUp).c_str(),
		node->getFace(kScDown).c_str(),
	};

	for (uint i = 0; i < kNumFaces; ++i)
		_faceTextures[i] = loadTexture(gfx, faceFiles[i]);

	_loadedNode = node;
}

void SceneView::loadZoom(Gfx *gfx, const Zoom *zoom) {
	if (!zoom)
		error("SceneView::loadZoom: zoom not provided");
	gfx->deleteTexture(_zoomTexture);
	_zoomTexture = loadTexture(gfx, zoom->getImageFile());
	_loadedZoom = zoom;
}

void SceneView::draw(Gfx *gfx, World &world, const Camera &camera) {
	if (_loadedNode != world.getCurrentNode())
		loadNode(gfx, world.getCurrentNode());

	const Zoom *currentZoom = world.getCurrentZoom();
	if (currentZoom && _loadedZoom != currentZoom)
		loadZoom(gfx, currentZoom);

	gfx->clear();

	if (!world.getCurrentZoom()) {
		gfx->setup3D(camera.getHeading(), camera.getPitch(), camera.getFov());
		gfx->drawCube(_faceTextures);
		gfx->setup2D();
	} else {
		gfx->setup2D();
		gfx->drawTexture2D(_zoomTexture, 0, kViewportY, kScreenWidth, kViewportHeight);
	}
}

void SceneView::invalidate() {
	_loadedNode = nullptr;
	_loadedZoom = nullptr;
	warning("SceneView: reload scene crossfade not implemented");
}

void SceneView::freeTextures(Gfx *gfx) {
	for (GLuint texture : _faceTextures)
		gfx->deleteTexture(texture);

	gfx->deleteTexture(_zoomTexture);
}

const Object *SceneView::findObjectAt(const World &world, const Camera &camera, const Common::Point &mouse) const {
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
