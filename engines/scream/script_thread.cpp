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

#include "common/system.h"
#include "common/textconsole.h"

#include "scream/script_thread.h"

namespace Scream {

void ScriptThread::start(lua_State *state, const Common::String &name) {
	if (isRunning()) {
		warning("ScriptThread::start: thread already running, cannot call %s", name.c_str());
		return;
	}

	_state = state;
	_thread = lua_newthread(_state);
	_threadRef = luaL_ref(_state, LUA_REGISTRYINDEX);

	lua_getglobal(_thread, name.c_str());
	if (!lua_isfunction(_thread, -1)) {
		warning("ScriptThread::start: %s is not a function", name.c_str());
		release();
		return;
	}

	resume();
}

void ScriptThread::resume() {
	switch (lua_resume(_thread, 0)) {
	case 0:
		release();
		break;
	case LUA_YIELD:
		break;
	default:
		warning("ScriptThread::resume error: %s", lua_tostring(_thread, -1));
		release();
		break;
	}
}

void ScriptThread::release() {
	luaL_unref(_state, LUA_REGISTRYINDEX, _threadRef);
	_thread = nullptr;
	_threadRef = LUA_NOREF;
}

void ScriptThread::sleepFor(int seconds) {
	_wakeTime = g_system->getMillis() + seconds * 1000;
}

bool ScriptThread::isSleeping() const {
	return isRunning() && g_system->getMillis() < _wakeTime;
}

bool ScriptThread::isReadyToResume() const {
	if (isRunning() && !isSleeping())
		return true;
	return false;
}

} // end of namespace Scream
