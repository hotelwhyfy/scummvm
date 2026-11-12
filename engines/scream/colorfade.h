
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

#ifndef SCREAM_COLORFADE_H
#define SCREAM_COLORFADE_H

#include "common/scummsys.h"

namespace Scream {

class ColorFade {
public:
	static const uint kMaxChannels = 6;
	ColorFade(uint channels, float fixedStep);       // fixedStep 0 = each channel's step is |to - from| / 1000
	bool update(const float *from, const float *to); // one frame; returns true on arrival
	const float *getValues() const { return _values; }

private:
	uint _channels;
	float _fixedStep;
	bool _active = false;
	float _values[kMaxChannels] = {};
	float _steps[kMaxChannels] = {};
	bool _arrived[kMaxChannels] = {};
};

} // End of namespace Scream

#endif
