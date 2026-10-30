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

#include "scream/cursor.h"
#include "scream/constants.h"
#include "scream/gfx.h"
#include "scream/image.h"

namespace Scream {

Cursor::Cursor() {
	_cursorMap[kScNormal] = "cursor_def.tga";
	_cursorMap[kScForward] = "cursor_def.tga";
	_cursorMap[kScLook] = "cursor_def.tga";
	_cursorMap[kScUse] = "cursor_def.tga";
	_cursorMap[kScZoomOut] = "cursor_def.tga";
	_cursorMap[kScWait] = "wait_def.tga";
}

GLuint Cursor::getTexture() const {
	return _currentTexture;
}

void Cursor::setCursor(Gfx *gfx, int cursorType) {
	if (cursorType == _currentType)
		return;

	Common::String fileName = getFileName(cursorType);
	if (fileName.empty())
		return;

	gfx->deleteTexture(_currentTexture);
	GLuint cursorTexture = loadTexture(gfx, fileName);
	_currentType = cursorType;
	_currentTexture = cursorTexture;
}

void Cursor::scCursor(int cursorType, const char *file) {
	if (cursorType == kScZoomBack)
		cursorType = kScZoomOut;

	_cursorMap[cursorType] = file;
}

Common::String Cursor::getFileName(int cursorType) const {
	if (cursorType == kScZoomBack)
		cursorType = kScZoomOut;

	if (_cursorMap.contains(cursorType))
		return _cursorMap[cursorType];

	warning("Cursor type not found: %d", cursorType);
	return "";
}

Common::Point Cursor::getHotSpot(int cursorType) const {
	switch (cursorType) {
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
