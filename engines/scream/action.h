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

#ifndef SCREAM_ACTION_H
#define SCREAM_ACTION_H

#include "common/str.h"

namespace Scream {

class Item;

struct Action {
	Action(int kind, const char *arg1, const char *arg2) : _kind(kind), _arg1(arg1), _arg2(arg2) {}
	int _kind;
	Common::String _arg1;
	Common::String _arg2;
};

struct Reaction {
	Reaction(Item *otherItem, const Action &action) : _otherItem(otherItem), _action(action) {}
	Item *_otherItem;
	Action _action;
};

} // end of namespace Scream

#endif
