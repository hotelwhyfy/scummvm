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

#include "engines/util.h"
#include "math/utils.h"

#include "scream/constants.h"
#include "scream/gfx.h"

namespace Scream {

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

static const int kViewportY = 64;
static const int kViewportHeight = kScreenHeight - (2 * kViewportY);
static const float kViewportAspect = (float)kScreenWidth / kViewportHeight;
static const float kNearPlane = 0.1f;
static const float kFarPlane = 1000.0f;

Gfx::Gfx() {
	initGraphics3d(kScreenWidth, kScreenHeight);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLuint Gfx::createTexture(const Graphics::Surface *surface) {
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

void Gfx::drawTexture2D(GLuint texture, int x, int y, int w, int h) {
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(x, y);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(x + w, y);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(x, y + h);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(x + w, y + h);
	glEnd();
}

void Gfx::deleteTexture(GLuint texture) {
	glDeleteTextures(1, &texture);
}

void Gfx::clear() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Gfx::setup2D() {
	glViewport(0, 0, kScreenWidth, kScreenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, kScreenWidth, 0, kScreenHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Gfx::setupCamera(float heading, float pitch, float fov) {
	const float frustumTop = kNearPlane * tan(Math::deg2rad(fov) / 2);
	const float frustumRight = frustumTop * kViewportAspect;
	const float frustumBottom = -frustumTop;
	const float frustumLeft = -frustumRight;

	glViewport(0, kViewportY, kScreenWidth, kViewportHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(frustumLeft, frustumRight, frustumBottom, frustumTop, kNearPlane, kFarPlane);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
	glRotatef(heading, 0.0f, 1.0f, 0.0f);
}

void Gfx::drawCube(const GLuint textures[kNumFaces]) {
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);

	for (uint face = 0; face < kNumFaces; ++face) {
		glBindTexture(GL_TEXTURE_2D, textures[face]);
		glBegin(GL_TRIANGLE_STRIP);
		for (uint corner = 0; corner < 4; ++corner) {
			glTexCoord2f(kFaceTexCoords[corner][0], kFaceTexCoords[corner][1]);
			glVertex3f(kCubeVertices[face][corner][0], kCubeVertices[face][corner][1], kCubeVertices[face][corner][2]);
		}
		glEnd();
	}
}

} // end of namespace Scream
