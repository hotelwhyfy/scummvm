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

#ifndef SCREAM_INVENTORY_H
#define SCREAM_INVENTORY_H

#include "common/array.h"

namespace Scream {

class Item;

class Inventory {
public:
	void add(Item *item);
	void remove(Item *item);
	void grab(const Item *item) { _held = item; }
	void drop() { _held = nullptr; }
	const Item *getHeld() const { return _held; }
	uint count() const { return _items.size(); }
	const Common::Array<Item *> &getItems() const { return _items; }

private:
	Common::Array<Item *> _items;
	const Item *_held = nullptr;
};

} // end of namespace Scream

#endif
