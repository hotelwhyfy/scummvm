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

#ifndef SCREAM_WORLD_ROOM_H
#define SCREAM_WORLD_ROOM_H

#include "common/stack.h"
#include "common/str.h"
#include "common/textconsole.h"

namespace Scream {

class Node;
class Zoom;

class Room {
public:
	Room(const char *name) : _name(name) {}
	const Common::String &getName() const { return _name; }
	Node *getLastVisitedNode() const { return _lastVisitedNode; }
	void setLastVisitedNode(Node *node) { _lastVisitedNode = node; }
	void pushZoom(Zoom *zoom) { _zooms.push(zoom); }
	void popZoom();
	void clearZooms() { _zooms.clear(); }
	Zoom *getTopZoom() const;

private:
	Common::String _name;
	Node *_lastVisitedNode = nullptr;
	Common::Stack<Zoom *> _zooms;
};

} // end of namespace Scream

#endif
