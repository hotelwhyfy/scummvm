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

#ifndef SCREAM_HINTS_H
#define SCREAM_HINTS_H

#include "common/str.h"

namespace Scream {

class Hints {
public:
	void scHintsSet(int key, const char *value);

private:
	Common::String _inventoryOpenText;
	Common::String _itemGrabbedText;
	Common::String _inventoryClosedText;
	Common::String _itemName;
};

} // end of namespace Scream

#endif
