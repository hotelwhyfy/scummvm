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

#ifndef SCREAM_DIALOGUE_H
#define SCREAM_DIALOGUE_H

#include "common/array.h"
#include "common/str.h"

namespace Scream {

struct DialogueLine {
	DialogueLine(const char *voiceFile, const char *text, bool hint) : _voiceFile(voiceFile), _text(text), _hint(hint) {}
	Common::String _voiceFile;
	Common::String _text;
	bool _hint;
};

class Dialogue {
public:
	Dialogue(const char *name);
	void scScriptAdd(const char *voiceFile, const char *text, bool hint);

private:
	Common::String _name;
	Common::Array<DialogueLine> _lines;
};

} // end of namespace Scream

#endif
