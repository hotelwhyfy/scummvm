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

#ifndef SCREAM_NODE_H
#define SCREAM_NODE_H

#include "common/array.h"
#include "common/str.h"

#include "scream/constants.h"
#include "scream/object.h"
#include "scream/room.h"

namespace Scream {

class Node {
public:
	Node(const char *name) : _name(name) {}
	void setParentRoom(Room *room) { _parentRoom = room; }
	void setFace(int face, const char *file);
	void setFootstepFile(const char *file) { _footstepFile = file; }
	void addObject(Object *object) { _objects.push_back(object); }
	const Common::String &getFace(int face) const;

private:
	Common::String _name;
	Room *_parentRoom = nullptr;
	Common::String _faces[kNumFaces];
	Common::String _footstepFile;
	Common::Array<Object *> _objects;
};

} // end of namespace Scream

#endif
