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

#include "scream/object.h"
#include "scream/constants.h"

namespace Scream {

void Object::addReaction(Item *item, const Action &action) {
	if (_reactions.size() >= kScMaxReactions)
		error("Too may reactions on %s", _name.c_str());

	Reaction reaction(item, action);
	_reactions.push_back(reaction);
}

void Object::setRect(int x, int y, int w, int h, int face) {
	_x = x;
	_y = y;
	_w = w;
	_h = h;
	_face = face;
	_enabled = true;
}

} // end of namespace Scream
