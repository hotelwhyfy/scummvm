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

#include "scream/font.h"
#include "scream/gfx.h"
#include "scream/image.h"

namespace Scream {

void Font::scFont(const char *file) {
	_font = file;
}

void Font::drawText(Gfx *gfx, int x, int y, const Common::String &text, float alpha) {
	if (_loadedFile != _font) {
		gfx->deleteTexture(_texture);
		_texture = loadTexture(gfx, _font);
		_loadedFile = _font;
	}
	for (uint i = 0; i < text.size(); ++i) {
		byte b = (byte)text[i];
		float column = b % 16;
		float row = b / 16;
		float u0 = column / 16.0f;
		float v0 = row / 16.0f;
		float u1 = (column + 1) / 16.0f;
		float v1 = (row + 1) / 16.0f;
		gfx->drawTexture2D(_texture, x, y, 16, 16, u0, v0, u1, v1, alpha);
		x += 10;
	}
}

void Font::freeTexture(Gfx *gfx) {
	gfx->deleteTexture(_texture);
	_texture = 0;
}

} // end of namespace Scream
