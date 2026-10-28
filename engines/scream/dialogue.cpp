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

#include "scream/dialogue.h"

namespace Scream {

Dialogue::Dialogue(const char *name) : _name(name) {
}

void Dialogue::scScriptAdd(const char *voiceFile, const char *text, bool hint) {
	if (strlen(text) >= 400) {
		warning("ScriptAdd : message for %s is too long", text);
	}
	if (_lines.size() >= 21) {
		warning("ScriptAdd : too many lines %s", _name.c_str());
	}
	_lines.push_back(DialogueLine(voiceFile, text, hint));
}

} // end of namespace Scream
