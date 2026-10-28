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

#ifndef SCREAM_MENU_H
#define SCREAM_MENU_H

#include "common/str.h"

namespace Scream {

class Menu {
private:
	bool _showMenu = false;
	bool _useSaveFunc = false;

	Common::String _logoFile;
	Common::String _backFile;
	Common::String _newFile;
	Common::String _setupFile;
	Common::String _loadFile;
	Common::String _saveFile;
	Common::String _quitFile;
	Common::String _newGame1;
	Common::String _newGame2;

	Common::String _musicFile;
	Common::String _introHandler;
	Common::String _outroHandler;
	Common::String _image;

public:
	void scMenuSet(const char *logoFile, const char *backFile, const char *newFile, const char *setupFile, const char *loadFile, const char *saveFile, const char *quitFile, const char *newGame1, const char *newGame2);
	void scMenuSetEx(int key, const char *value);
	void scMenuSetSaveFunc();
};

} // end of namespace Scream

#endif
