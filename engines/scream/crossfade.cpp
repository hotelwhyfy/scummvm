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

#include "scream/crossfade.h"
#include "scream/constants.h"
#include "scream/gfx.h"

namespace Scream {

static const float kFadeStep = 0.001f;

void Crossfade::capture(Gfx *gfx) {
	gfx->copyScreen(_texture);
	_p = 0.0f;
}

void Crossfade::draw(Gfx *gfx, uint speed) {
	if (_p >= 1.0f)
		return;
	gfx->drawTexture2D(_texture, 0, 0, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f - _p); // capture is stored bottom row first
	_p = MIN(1.0f, _p + speed * kFadeStep);
}

void Crossfade::freeTexture(Gfx *gfx) {
	gfx->deleteTexture(_texture);
	_texture = 0;
	_p = 1.0f;
}

} // end of namespace Scream
