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

#include "common/textconsole.h"

#include "scream/node.h"

namespace Scream {

static int faceIndex(int face) {
	switch (face) {
	case kScFront:
		return 0;
	case kScBack:
		return 1;
	case kScLeft:
		return 2;
	case kScRight:
		return 3;
	case kScUp:
		return 4;
	case kScDown:
		return 5;
	default:
		return -1;
	}
}

void Node::setFace(int face, const char *file) {
	int index = faceIndex(face);
	if (index >= 0)
		_faces[index] = file;
}

const Common::String &Node::getFace(int face) const {
	int index = faceIndex(face);
	if (index >= 0)
		return _faces[index];

	warning("Node::getFace: unknown face provided %d", face);
	static const Common::String emptyString;
	return emptyString;
}

} // end of namespace Scream
