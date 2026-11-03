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

#ifndef SCREAM_SCRIPT_H
#define SCREAM_SCRIPT_H

#include "common/lua/lauxlib.h"
#include "common/path.h"

#include "scream/script_thread.h"

struct lua_State;

namespace Scream {

class Script {
public:
	Script();
	~Script();
	bool runFile(const Common::Path &filename);
	void callFunction(const Common::String &name) { _thread.start(_state, name); }
	void update();
	bool isWaiting() const;
	bool isSleeping() const { return _thread.isSleeping(); }

private:
	lua_State *_state = nullptr;
	static int scSet(lua_State *L);
	static int scInit(lua_State *L);
	static int scCursor(lua_State *L);
	static int scFont(lua_State *L);
	static int scMenuSet(lua_State *L);
	static int scMenuSetEx(lua_State *L);
	static int scMenuSetSaveFunc(lua_State *L);
	static int scHintsSet(lua_State *L);
	static int scCreate(lua_State *L);
	static int scTrackSet3D(lua_State *L);
	static int scTrackSetEx(lua_State *L);
	static int scTrackSet(lua_State *L);
	static int scScriptAdd(lua_State *L);
	static int scChoiceAdd(lua_State *L);
	static int scInventorySet(lua_State *L);
	static int scJournalSet(lua_State *L);
	static int scInventoryReaction(lua_State *L);
	static int scBegin(lua_State *L);
	static int scTexture(lua_State *L);
	static int scFootstep(lua_State *L);
	static int scPlace(lua_State *L);
	static int scOnHover(lua_State *L);
	static int scOnClick(lua_State *L);
	static int scEnable(lua_State *L);
	static int scDisable(lua_State *L);
	static int scEnd(lua_State *L);
	static int scReaction(lua_State *L);
	static int scAudio(lua_State *L);
	static int scRun(lua_State *L);
	static int scTerminate(lua_State *L);
	static int scFXPlayEx(lua_State *L);
	static int scSleep(lua_State *L);
	static int scBind(lua_State *L);
	static int scSetLastVisited(lua_State *L);
	static int scSwitch(lua_State *L);
	static int scLookAt(lua_State *L);
	static int scFXSet(lua_State *L);
	static int scFXPlay(lua_State *L);
	static int scTrackFadeOut(lua_State *L);
	static int scCutscene(lua_State *L);
	static int scFXStop(lua_State *L);
	static int scFeedbackEx(lua_State *L);

	void setEnabled(int key, bool enabled);

	ScriptThread _thread;
	bool _waitEnabled = true;
};

} // end of namespace Scream

#endif
