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

#include "scream/track.h"
#include "scream/constants.h"

namespace Scream {

Track::Track(const char *name) : _name(name) {
}

void Track::scTrackSet(const char *front, int loop) {
	_front = front;
	_loop = loop;
	_3d = false;
}

void Track::scTrackSet3D(const char *front, const char *rearLeft, const char *rearRight, int loop) {
	_front = front;
	_rearLeft = rearLeft;
	_rearRight = rearRight;
	_loop = loop;
	_3d = true;
}

void Track::scTrackSetEx(int key, int value) {
	switch (key) {
	case kScVolume:
		_volume = value;
		break;
	case kScFadeIn:
		_fadeIn = value;
		break;
	case kScFadeOut:
		_fadeOut = value;
		break;
	default:
		break;
	}
}

} // end of namespace Scream
