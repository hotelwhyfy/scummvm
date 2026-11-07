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

#include "scream/inventory.h"
#include "scream/world/item.h"

namespace Scream {

static int findItemIndex(const Common::Array<Item *> &inventory, Item *item) {
	for (uint i = 0; i < inventory.size(); ++i) {
		if (inventory[i] == item)
			return i;
	}
	return -1;
}

void Inventory::add(Item *item) {
	_items.push_back(item);
}

void Inventory::remove(Item *item) {
	int index = findItemIndex(_items, item);
	Item *removed = nullptr;
	if (index >= 0)
		removed = _items.remove_at(index);
	else if (_items.size() > 0)
		removed = _items.remove_at(0);
	if (_held == removed)
		_held = nullptr;
}

} // end of namespace Scream
