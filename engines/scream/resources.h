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

#ifndef SCREAM_RESOURCES_H
#define SCREAM_RESOURCES_H

#include "common/archive.h"

namespace Scream {

class Res;

enum ResourceType {
	kResourceJPEG = 1,
	kResourceTGA = 2,
	kResourceOGG = 3
};

class ResEntry : public Common::ArchiveMember {
	Res *_parent;
	Common::Path _path;
	uint32 _type;
	uint32 _offset;
	uint32 _len;

public:
	ResEntry(const Common::Path &path, uint32 type, uint32 offset, uint32 len, Res *parent);
	Common::SeekableReadStream *createReadStream() const override;
	Common::SeekableReadStream *createReadStreamForAltStream(Common::AltStreamType altStreamType) const override { return nullptr; }
	Common::String getName() const override { return _path.baseName(); }
	Common::Path getPathInArchive() const override { return _path; }
	Common::String getFileName() const override { return _path.baseName(); }
	friend class Res;
};

class Res : public Common::Archive {
public:
	bool open(const Common::Path &filename);
	// Common::Archive implementation
	bool hasFile(const Common::Path &path) const override;
	int listMembers(Common::ArchiveMemberList &list) const override;
	const Common::ArchiveMemberPtr getMember(const Common::Path &path) const override;
	Common::SeekableReadStream *createReadStreamForMember(const Common::Path &path) const override;

private:
	Common::Path _resFileName;
	typedef Common::SharedPtr<ResEntry> ResEntryPtr;
	typedef Common::HashMap<Common::Path, ResEntryPtr, Common::Path::IgnoreCase_Hash, Common::Path::IgnoreCase_EqualTo> ResMap;
	ResMap _entries;
};

} // end of namespace Scream

#endif
