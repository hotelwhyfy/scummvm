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

void Cursor::draw(Gfx *gfx, const Common::Point &mouse) const {
	int x = mouse.x - _currentHotspot.x;
	int y = kScreenHeight - (mouse.y - _currentHotspot.y) - kCursorSize;
	gfx->drawTexture2D(_currentTexture, x, y, kCursorSize, kCursorSize);
	if (_glow) {
		gfx->setBlend(kBlendAdditiveAlpha);
		gfx->drawTexture2D(_currentTexture, x, y, kCursorSize, kCursorSize);
		gfx->drawTexture2D(_currentTexture, x - 1, y - 1, kCursorSize + 2, kCursorSize + 2);
		gfx->setBlend(kBlendNormal);
	}
}

void Cursor::freeTexture(Gfx *gfx) {
	gfx->deleteTexture(_currentTexture);
	_currentTexture = 0;
	_currentType = 0;
	_currentFile = "";
}

void Cursor::loadFile(Gfx *gfx, const Common::String &fileName) {
	_currentTexture = loadTexture(gfx, fileName);
	_currentFile = fileName;
}

void Cursor::setCursor(Gfx *gfx, int cursorType) {
	_glow = false;

	if (cursorType == _currentType)
		return;

	Common::String fileName = getFileName(cursorType);
	if (fileName.empty())
		return;

	freeTexture(gfx);
	loadFile(gfx, fileName);
	_currentType = cursorType;
	_currentHotspot = getHotSpot(_currentType);
}

void Cursor::setIcon(Gfx *gfx, const Common::String &icon, bool glow) {
	_glow = glow;

	if (icon == _currentFile)
		return;

	freeTexture(gfx);
	loadFile(gfx, icon);
	_currentHotspot = Common::Point(kCursorSize / 2, kCursorSize / 2);
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
		return Common::Point(kCursorSize / 2, kCursorSize / 2);
	}
}

} // end of namespace Scream
