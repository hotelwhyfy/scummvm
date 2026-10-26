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

#include "common/file.h"
#include "common/memstream.h"
#include "common/substream.h"

#include "scream/resources.h"

namespace Scream {

ResEntry::ResEntry(const Common::Path &path, uint32 type, uint32 offset, uint32 len, Res *parent) : _path(path), _offset(offset), _len(len), _type(type), _parent(parent) {
}

Common::SeekableReadStream *ResEntry::createReadStream() const {
	return _parent->createReadStreamForMember(_path);
}

bool Res::open(const Common::Path &filename) {
	_resFileName = filename;

	bool result = true;

	Common::File file;
	if (!file.open(filename)) {
		result = false;
	} else {
		file.seek(0x100);                   // skip header
		uint32 count = file.readUint32LE(); // entry count

		for (uint32 i = 0; i < count; ++i) {
			char buf[81];
			file.read(buf, 80); // entry name (80 bytes)
			buf[80] = '\0';     // ensure string is null-terminated
			Common::Path path(buf, Common::Path::kNoSeparator);
			uint32 type = file.readUint32LE(); // type: 1 = `.jpg`, 2 = `.tga`, 3 = `.ogg`
			uint32 offset = file.readUint32LE();
			uint32 len = file.readUint32LE();
			if (!_entries.contains(path)) {
				ResEntry *entry = new ResEntry(path, type, offset, len, this);
				_entries[path] = Common::SharedPtr<ResEntry>(entry);
			}
		}
	}

	return result;
}

bool Res::hasFile(const Common::Path &path) const {
	return _entries.contains(path);
}

int Res::listMembers(Common::ArchiveMemberList &list) const {
	int count = 0;
	for (const auto &entry : _entries) {
		list.push_back(Common::ArchiveMemberPtr(entry._value));
		++count;
	}
	return count;
}

const Common::ArchiveMemberPtr Res::getMember(const Common::Path &path) const {
	if (!_entries.contains(path)) {
		return Common::ArchiveMemberPtr();
	}
	return _entries[path];
}

Common::SeekableReadStream *Res::createReadStreamForMember(const Common::Path &path) const {
	if (!hasFile(path))
		return nullptr;

	ResEntryPtr entry = _entries[path];
	Common::File *file = new Common::File();
	if (!file->open(_resFileName)) {
		delete file;
		return nullptr;
	}
	return new Common::SeekableSubReadStream(file, entry->_offset, entry->_offset + entry->_len, DisposeAfterUse::YES);
}

} // end of namespace Scream
