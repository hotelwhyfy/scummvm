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

#ifndef SCREAM_CAMERA_H
#define SCREAM_CAMERA_H

#include "common/rect.h"

namespace Scream {

struct FaceHit {
	FaceHit() = default;
	FaceHit(int face, const Common::Point &pixel) : face(face), pixel(pixel) {}
	int face = 0;
	Common::Point pixel;
	bool isValid() const { return face != 0; }
};

class Camera {
public:
	Camera();
	float getHeading() const { return _heading; }
	float getPitch() const { return _pitch; }
	float getFov() const { return _fov; }
	void update(const Common::Point &mouse);
	FaceHit pick(const Common::Point &mouse) const;

private:
	float _heading = 0.0f;
	float _pitch = 0.0f;
	float _lookY = 0.0f;
	float _fov;
};

} // end of namespace Scream

#endif
