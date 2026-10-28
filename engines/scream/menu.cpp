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

#include "common/str.h"
#include "common/textconsole.h"

#include "scream/constants.h"
#include "scream/menu.h"

namespace Scream {

void Menu::scMenuSet(const char *logoFile, const char *backFile, const char *newFile, const char *setupFile, const char *loadFile, const char *saveFile, const char *quitFile, const char *newGame1, const char *newGame2) {
	_musicFile.clear();
	_introHandler.clear();
	_outroHandler.clear();
	_image.clear();
	_useSaveFunc = false;

	_showMenu = true;
	_logoFile = logoFile;
	_backFile = backFile;
	_newFile = newFile;
	_setupFile = setupFile;
	_loadFile = loadFile;
	_saveFile = saveFile;
	_quitFile = quitFile;
	_newGame1 = newGame1;
	_newGame2 = newGame2;
}

void Menu::scMenuSetEx(int key, const char *value) {
	switch (key) {
	case kScStereo:
		_musicFile = value;
		break;
	case kScIntro:
		_introHandler = value;
		break;
	case kScOutro:
		_outroHandler = value;
		break;
	case kScImage:
		_image = value;
		break;
	default:
		warning("unhandled key for scMenuSetEx (%d)", key);
		break;
	}
}

void Menu::scMenuSetSaveFunc() {
	_useSaveFunc = true;
}

} // end of namespace Scream
