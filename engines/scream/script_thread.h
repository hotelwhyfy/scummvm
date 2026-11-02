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

#ifndef SCREAM_SCRIPT_THREAD_H
#define SCREAM_SCRIPT_THREAD_H

#include "common/lua/lauxlib.h"
#include "common/str.h"

namespace Scream {

class ScriptThread {
public:
	void start(lua_State *state, const Common::String &name);
	bool isRunning() const { return _thread != nullptr; }
	void resume();
	void sleepFor(int seconds);
	bool isReadyToResume() const;
	bool isSleeping() const;

private:
	void release();
	lua_State *_thread = nullptr;
	int _threadRef = LUA_NOREF;
	lua_State *_state = nullptr;
	uint32 _wakeTime = 0;
};

} // end of namespace Scream

#endif
