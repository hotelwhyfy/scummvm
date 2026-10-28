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

#ifndef SCREAM_ITEM_H
#define SCREAM_ITEM_H

#include "common/array.h"
#include "common/str.h"

#include "scream/action.h"

namespace Scream {

class Item {
public:
	Item(const char *name);
	const Common::String &getName() const { return _name; }
	uint getReactionCount() const { return _reactions.size(); }
	void scInventorySet(const char *icon, const char *label, const char *failText, const char *failVoice);
	void addReaction(Item *item, const Action &action);

private:
	Common::String _name;
	Common::String _icon;
	Common::String _label;
	Common::String _failText;
	Common::String _failVoice;
	Common::Array<Reaction> _reactions;
};

} // end of namespace Scream

#endif
