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

#ifndef SCREAM_ZOOM_H
#define SCREAM_ZOOM_H

#include "common/array.h"
#include "common/str.h"

#include "scream/object.h"

namespace Scream {

class Zoom {
public:
	Zoom(const char *name) : _name(name) {}
	void setImageFile(const char *file) { _imageFile = file; }
	void addObject(Object *object) { _objects.push_back(object); }
	void setAudioFile(int key, const char *file);

private:
	Common::String _name;
	Common::String _imageFile;
	Common::Array<Object *> _objects;
	Common::String _enterAudioFile;
	Common::String _leaveAudioFile;
};

} // end of namespace Scream

#endif
