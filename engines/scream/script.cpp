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

#include "common/debug.h"
#include "common/file.h"
#include "common/lua/lauxlib.h"
#include "common/lua/lua.h"

#include "scream/font.h"
#include "scream/item.h"
#include "scream/menu.h"
#include "scream/object.h"
#include "scream/resources.h"
#include "scream/scream.h"
#include "scream/script.h"
#include "scream/settings.h"

namespace Scream {

static const char *getString(lua_State *L, int index) {
	const char *s = lua_tostring(L, index);
	if (!s) {
		warning("expected a string at argument %d, got %s", index, lua_typename(L, lua_type(L, index)));
		return "";
	}
	return s;
}

static const char *getOptionalString(lua_State *L, int index) {
	if (lua_isnoneornil(L, index))
		return "";
	return lua_tostring(L, index);
}

static void describeFunction(lua_State *L, const char *name) {
	Common::String out = Common::String::format("%s( ", name);
	int argCount = lua_gettop(L);
	for (int i = 1; i <= argCount; ++i) {
		if (lua_isnumber(L, i)) {
			out += Common::String::format("%g ", lua_tonumber(L, i));
		} else if (lua_isstring(L, i)) {
			out += Common::String::format("%s ", getString(L, i));
		} else if (lua_isboolean(L, i)) {
			out += Common::String::format("%s ", lua_toboolean(L, i) ? "true" : "false");
		} else {
			out += Common::String::format("%s ", lua_typename(L, lua_type(L, i)));
		}
	}
	out += " )";
	debug(1, "%s", out.c_str());
}

int Script::scTerminate(lua_State *L) {
	describeFunction(L, "scTerminate");
	return 0;
}

int Script::scRun(lua_State *L) {
	describeFunction(L, "scRun");
	return 0;
}

int Script::scAudio(lua_State *L) {
	// describeFunction(L, "scAudio");
	int key = lua_tonumber(L, 1);
	const char *file = getString(L, 2);
	g_engine->_world.boundZoom()->setAudioFile(key, file);
	return 0;
}

int Script::scReaction(lua_State *L) {
	// describeFunction(L, "scReaction");
	const char *itemName = getString(L, 1);
	int kind = lua_tonumber(L, 2);
	const char *arg1 = getString(L, 3);
	const char *arg2 = getOptionalString(L, 4);
	Item *item = g_engine->_world.findItem(itemName);
	g_engine->_world.boundObject()->addReaction(item, Action(kind, arg1, arg2));
	return 0;
}

int Script::scEnd(lua_State *L) {
	// describeFunction(L, "scEnd");
	int level = lua_tonumber(L, 1);
	g_engine->_world.scEnd(level);
	return 0;
}

int Script::scDisable(lua_State *L) {
	// describeFunction(L, "scDisable");
	int key = lua_tonumber(L, 1);
	g_engine->_world.setEnabled(key, false);
	return 0;
}

int Script::scEnable(lua_State *L) {
	// describeFunction(L, "scEnable");
	int key = lua_tonumber(L, 1);
	g_engine->_world.setEnabled(key, true);
	return 0;
}

int Script::scOnClick(lua_State *L) {
	// describeFunction(L, "scOnClick");
	int kind = lua_tonumber(L, 1);
	const char *arg1 = getString(L, 2);
	const char *arg2 = getOptionalString(L, 3);
	Action action(kind, arg1, arg2);
	g_engine->_world.boundObject()->setClickAction(action);
	return 0;
}

int Script::scOnHover(lua_State *L) {
	// describeFunction(L, "scOnHover");
	int cursor = lua_tonumber(L, 1);
	g_engine->_world.boundObject()->setHoverCursor(cursor);
	return 0;
}

int Script::scPlace(lua_State *L) {
	// describeFunction(L, "scPlace");
	int x = lua_tonumber(L, 1);
	int y = lua_tonumber(L, 2);
	int w = lua_tonumber(L, 3);
	int h = lua_tonumber(L, 4);
	int face = lua_tonumber(L, 5);
	g_engine->_world.scPlace(x, y, w, h, face);
	return 0;
}

int Script::scFootstep(lua_State *L) {
	// describeFunction(L, "scFootstep");
	const char *file = getString(L, 1);
	g_engine->_world.boundNode()->setFootstepFile(file);
	return 0;
}

int Script::scTexture(lua_State *L) {
	// describeFunction(L, "scTexture");
	int face = lua_tonumber(L, 1);
	const char *file = getString(L, 2);
	g_engine->_world.scTexture(face, file);
	return 0;
}

int Script::scBegin(lua_State *L) {
	// describeFunction(L, "scBegin");
	int level = lua_tonumber(L, 1);
	g_engine->_world.scBegin(level);
	return 0;
}

int Script::scInventoryReaction(lua_State *L) {
	// describeFunction(L, "scInventoryReaction");
	const char *itemName = getString(L, 1);
	int kind = lua_tonumber(L, 2);
	const char *arg1 = getString(L, 3);
	const char *arg2 = getOptionalString(L, 4);
	Item *boundItem = g_engine->_world.boundItem();
	Item *otherItem = g_engine->_world.findItem(itemName);
	Action action(kind, arg1, arg2);
	g_engine->_world.scInventoryReaction(boundItem, otherItem, action);
	return 0;
}

int Script::scJournalSet(lua_State *L) {
	// describeFunction(L, "scJournalSet");
	const char *image = getString(L, 1);
	const char *font = getString(L, 2);
	const char *openSound = getString(L, 3);
	const char *turnSound = getString(L, 4);
	const char *closeSound = getString(L, 5);
	g_engine->_journal.scJournalSet(image, font, openSound, turnSound, closeSound);
	return 0;
}

int Script::scInventorySet(lua_State *L) {
	// describeFunction(L, "scInventorySet");
	const char *icon = getString(L, 1);
	const char *label = getString(L, 2);
	const char *failText = getString(L, 3);
	const char *failVoice = getOptionalString(L, 4);
	g_engine->_world.boundItem()->scInventorySet(icon, label, failText, failVoice);
	return 0;
}

int Script::scChoiceAdd(lua_State *L) {
	// describeFunction(L, "scChoiceAdd");
	const char *text = getString(L, 1);
	const char *handler = getString(L, 2);
	g_engine->_world.boundChoice()->scChoiceAdd(text, handler);
	return 0;
}

int Script::scScriptAdd(lua_State *L) {
	// describeFunction(L, "scScriptAdd");
	const char *voiceFile = getString(L, 1);
	const char *text = getString(L, 2);
	bool hint = lua_toboolean(L, 3);
	g_engine->_world.boundDialogue()->scScriptAdd(voiceFile, text, hint);
	return 0;
}

int Script::scTrackSet(lua_State *L) {
	// describeFunction(L, "scTrackSet");
	const char *front = getString(L, 1);
	int loop = lua_tonumber(L, 2);
	g_engine->_world.boundTrack()->scTrackSet(front, loop);
	return 0;
}

int Script::scTrackSetEx(lua_State *L) {
	// describeFunction(L, "scTrackSetEx");
	int key = lua_tonumber(L, 1);
	int value = lua_tonumber(L, 2);
	g_engine->_world.boundTrack()->scTrackSetEx(key, value);
	return 0;
}

int Script::scTrackSet3D(lua_State *L) {
	// describeFunction(L, "scTrackSet3D");
	const char *front = getString(L, 1);
	const char *rearLeft = getString(L, 2);
	const char *rearRight = getString(L, 3);
	int loop = lua_tonumber(L, 4);
	g_engine->_world.boundTrack()->scTrackSet3D(front, rearLeft, rearRight, loop);
	return 0;
}

int Script::scCreate(lua_State *L) {
	// describeFunction(L, "scCreate");
	int kind = lua_tonumber(L, 1);
	const char *name = getString(L, 2);
	g_engine->_world.scCreate(kind, name);
	return 0;
}

int Script::scHintsSet(lua_State *L) {
	// describeFunction(L, "scHintsSet");
	int key = lua_tonumber(L, 1);
	const char *value = getString(L, 2);
	g_engine->_hints.scHintsSet(key, value);
	return 0;
}

int Script::scMenuSetSaveFunc(lua_State *L) {
	// describeFunction(L, "scMenuSetSaveFunc");
	g_engine->_menu.scMenuSetSaveFunc();
	return 0;
}

int Script::scMenuSetEx(lua_State *L) {
	// describeFunction(L, "scMenuSetEx");
	int key = lua_tonumber(L, 1);
	const char *value = getString(L, 2);
	g_engine->_menu.scMenuSetEx(key, value);
	return 0;
}

int Script::scMenuSet(lua_State *L) {
	// describeFunction(L, "scMenuSet");
	const char *logoFile = getString(L, 1);
	const char *backFile = getString(L, 2);
	const char *newFile = getString(L, 3);
	const char *setupFile = getString(L, 4);
	const char *loadFile = getString(L, 5);
	const char *saveFile = getString(L, 6);
	const char *quitFile = getString(L, 7);
	const char *newGame1 = getString(L, 8);
	const char *newGame2 = getString(L, 9);
	g_engine->_menu.scMenuSet(logoFile, backFile, newFile, setupFile, loadFile, saveFile, quitFile, newGame1, newGame2);
	return 0;
}

int Script::scFont(lua_State *L) {
	// describeFunction(L, "scFont");
	const char *file = getString(L, 1);
	g_engine->_font.scFont(file);
	return 0;
}

int Script::scCursor(lua_State *L) {
	// describeFunction(L, "scCursor");
	int key = lua_tonumber(L, 1);
	const char *value = getString(L, 2);
	g_engine->_cursor.scCursor(key, value);
	return 0;
}

int Script::scSet(lua_State *L) {
	// describeFunction(L, "scSet");
	int key = lua_tonumber(L, 1);
	int type = lua_type(L, 2);
	if (type == LUA_TSTRING) {
		const char *value = getString(L, 2);
		g_engine->_settings.scSet(key, value);
	} else {
		warning("unhandled type in Script::scSet (%d)", key);
	}
	return 0;
}

int Script::scInit(lua_State *L) {
	// describeFunction(L, "scInit");
	Res *res = new Res();
	const Common::String &resFile = g_engine->_settings._resFile;
	if (!res->open((Common::Path)resFile)) {
		error("Could not open resource file \"%s\"", resFile.c_str());
	}
	SearchMan.add(resFile, res);
	return 0;
}

Script::Script() {
	_state = luaL_newstate();
	if (_state == nullptr) {
		error("Could not create Lua state");
	}
	lua_register(_state, "scSet", scSet);
	lua_register(_state, "scInit", scInit);
	lua_register(_state, "scCursor", scCursor);
	lua_register(_state, "scFont", scFont);
	lua_register(_state, "scMenuSet", scMenuSet);
	lua_register(_state, "scMenuSetEx", scMenuSetEx);
	lua_register(_state, "scMenuSetSaveFunc", scMenuSetSaveFunc);
	lua_register(_state, "scHintsSet", scHintsSet);
	lua_register(_state, "scCreate", scCreate);
	lua_register(_state, "scTrackSet3D", scTrackSet3D);
	lua_register(_state, "scTrackSetEx", scTrackSetEx);
	lua_register(_state, "scTrackSet", scTrackSet);
	lua_register(_state, "scScriptAdd", scScriptAdd);
	lua_register(_state, "scChoiceAdd", scChoiceAdd);
	lua_register(_state, "scInventorySet", scInventorySet);
	lua_register(_state, "scJournalSet", scJournalSet);
	lua_register(_state, "scInventoryReaction", scInventoryReaction);
	lua_register(_state, "scBegin", scBegin);
	lua_register(_state, "scTexture", scTexture);
	lua_register(_state, "scFootstep", scFootstep);
	lua_register(_state, "scPlace", scPlace);
	lua_register(_state, "scOnHover", scOnHover);
	lua_register(_state, "scOnClick", scOnClick);
	lua_register(_state, "scEnable", scEnable);
	lua_register(_state, "scDisable", scDisable);
	lua_register(_state, "scEnd", scEnd);
	lua_register(_state, "scReaction", scReaction);
	lua_register(_state, "scAudio", scAudio);
	lua_register(_state, "scRun", scRun);
	lua_register(_state, "scTerminate", scTerminate);
}

Script::~Script() {
	if (_state)
		lua_close(_state);
}

bool Script::runFile(const Common::Path &filename) {
	Common::File file;
	Common::String name = filename.toString();
	if (file.open(filename)) {
		int64 size = file.size();
		if (size > 0) {
			uint32 len = (uint32)size;
			byte *buffer = new byte[len];
			file.read(buffer, len);
			int loadResult = luaL_loadbuffer(_state, (const char *)buffer, len, name.c_str());
			delete[] buffer;
			if (loadResult != 0) {
				error("Couldn't compile \"%s\":\n%s", name.c_str(), getString(_state, -1));
			}
			debug(1, "Script \"%s\" opened, size=%u", name.c_str(), len);
			int runtimeResult = lua_pcall(_state, 0, 0, 0);
			if (runtimeResult != 0) {
				error("Runtime error: \"%s\":\n%s", name.c_str(), getString(_state, -1));
			}
			return true;
		}
	}
	warning("Could not open script file \"%s\"", name.c_str());
	return false;
}

} // end of namespace Scream
