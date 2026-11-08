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

#ifndef SCREAM_VIEWS_SCENE_VIEW_H
#define SCREAM_VIEWS_SCENE_VIEW_H

#include "common/rect.h"
#include "graphics/opengl/system_headers.h"

#include "scream/camera.h"
#include "scream/world/world.h"

namespace Scream {

class Gfx;
class Node;
class Zoom;

class SceneView {
public:
	void draw(Gfx *gfx, World &world, const Camera &camera);
	void invalidate();
	void freeTextures(Gfx *gfx);
	Object *findObjectAt(const World &world, const Camera &camera, const Common::Point &mouse) const;
	static bool isInBand(const Common::Point &mouse);

private:
	GLuint _faceTextures[kNumFaces] = {};
	GLuint _maskTextures[kNumFaces] = {};
	GLuint _backdropTextures[kNumFaces] = {};
	GLuint _horizonTextures[kNumFaces] = {};
	GLuint _zoomTexture = 0;
	GLuint _whiteTexture = 0;
	GLuint _transparentTexture = 0;
	GLuint _rainTexture = 0;
	GLuint _gradientTexture = 0;
	const Node *_loadedNode = nullptr;
	const Zoom *_loadedZoom = nullptr;
	void loadNode(Gfx *gfx, const Node *node);
	void loadZoom(Gfx *gfx, const Zoom *zoom);
	void loadBackdrop(Gfx *gfx);
	void freeNodeTextures(Gfx *gfx);
};

} // end of namespace Scream

#endif
