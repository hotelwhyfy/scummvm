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

#ifndef SCREAM_OBJECT_H
#define SCREAM_OBJECT_H

#include "common/array.h"
#include "common/str.h"

#include "scream/action.h"
#include "scream/constants.h"

namespace Scream {

class Object {
public:
	Object(const char *name) : _name(name) {}
	void setRect(int x, int y, int w, int h, int face);
	void setHoverCursor(int cursor) { _hoverCursor = cursor; }
	void setClickAction(const Action &action) { _clickAction = action; }
	void setEnabled(bool enabled) { _enabled = enabled; }
	void addReaction(Item *item, const Action &action);

private:
	Common::String _name;
	int _x = 0;
	int _y = 0;
	int _w = 0;
	int _h = 0;
	int _face = 0;
	bool _enabled = false;
	int _hoverCursor = kScNormal;
	Action _clickAction = Action(0, "", "");
	Common::Array<Reaction> _reactions;
};

} // end of namespace Scream

#endif
