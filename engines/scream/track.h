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

#ifndef SCREAM_TRACK_H
#define SCREAM_TRACK_H

#include "common/str.h"

namespace Scream {

class Track {
public:
	Track(const char *name);
	void scTrackSet3D(const char *front, const char *rearLeft, const char *rearRight, int loop);
	void scTrackSetEx(int key, int value);
	void scTrackSet(const char *front, int loop);
	const Common::String &getName() const { return _name; }

private:
	Common::String _name;
	Common::String _front;
	Common::String _rearLeft;
	Common::String _rearRight;
	int _volume = 10;
	int _fadeIn = 0;
	int _fadeOut = 0;
	int _loop = 0;
	bool _3d = false; // stored, but never read by the engine
};

} // end of namespace Scream

#endif
