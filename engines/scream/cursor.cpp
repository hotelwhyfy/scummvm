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
#include "graphics/cursorman.h"
#include "image/tga.h"

#include "scream/constants.h"
#include "scream/cursor.h"

namespace Scream {

Cursor::Cursor() {
	// populate defaults
	_cursorMap[kScNormal] = "cursor_def.tga";
	_cursorMap[kScForward] = "cursor_def.tga";
	_cursorMap[kScLook] = "cursor_def.tga";
	_cursorMap[kScUse] = "cursor_def.tga";
	_cursorMap[kScZoomOut] = "cursor_def.tga";
	_cursorMap[kScWait] = "wait_def.tga";
}

void Cursor::setCursor(int kind) {
	Common::String fileName = getFileName(kind);
	if (fileName.empty())
		return;

	Common::File file;
	if (!file.open(Common::Path(fileName)))
		error("Could not open file: %s", fileName.c_str());

	Image::TGADecoder decoder;
	if (!decoder.loadStream(file))
		error("could not load image stream: %s", fileName.c_str());

	const Graphics::Surface *surface = decoder.getSurface();
	debug(1, "cursor %d: %s (%d x %d)", kind, fileName.c_str(), surface->w, surface->h);
	const Common::Point hotspot = getHotSpot(kind);
	CursorMan.replaceCursor(*surface, hotspot.x, hotspot.y, 0);
	CursorMan.showMouse(true);
}

void Cursor::scCursor(int kind, const char *file) {
	if (kind == kScZoomBack)
		kind = kScZoomOut;

	_cursorMap[kind] = file;
}

Common::String Cursor::getFileName(int kind) {
	if (kind == kScZoomBack)
		kind = kScZoomOut;

	if (_cursorMap.contains(kind))
		return _cursorMap[kind];

	warning("Cursor type not found: %d", kind);
	return "";
}

Common::Point Cursor::getHotSpot(int kind) {
	switch (kind) {
	case kScMenuCursor:
	case kScForward:
	case kScLook:
		return Common::Point(8, 8);
	case kScNormal:
	case kScUse:
	case kScZoomOut:
	case kScZoomBack:
		return Common::Point(24, 24);
	default:
		return Common::Point(32, 32);
	}
}

} // end of namespace Scream
