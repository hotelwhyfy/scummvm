
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
#include "common/textconsole.h"
#include "common/util.h"

#include "scream/colorfade.h"

namespace Scream {

static void stepChannel(float &value, float target, bool &arrived, float step) {
	if (target > value)
		value += step;
	else if (target < value)
		value -= step;
	if (fabs(target - value) < 0.001)
		arrived = true;
}

ColorFade::ColorFade(uint channels, float fixedStep) : _channels(channels), _fixedStep(fixedStep) {
	if (channels > kMaxChannels)
		error("ColorFade::ColorFade channels > kMaxChannels (%u, %u)", channels, kMaxChannels);
}

bool ColorFade::update(const float *from, const float *to) {
	if (!_active) {
		for (uint i = 0; i < _channels; ++i) {
			_values[i] = from[i];
			_steps[i] = _fixedStep != 0.0f ? _fixedStep : fabs(to[i] - from[i]) * 10.0 * 0.0001;
			_arrived[i] = false;
		}
		_active = true;
	}

	for (uint i = 0; i < _channels; ++i)
		stepChannel(_values[i], to[i], _arrived[i], _steps[i]);

	bool allArrived = true;
	for (uint i = 0; i < _channels; ++i)
		allArrived = allArrived && _arrived[i];

	if (allArrived) {
		_active = false;
		return true;
	}

	return false;
}

} // End of namespace Scream
