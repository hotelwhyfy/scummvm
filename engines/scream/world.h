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

#ifndef SCREAM_WORLD_H
#define SCREAM_WORLD_H

#include "common/array.h"

#include "scream/action.h"
#include "scream/choice.h"
#include "scream/constants.h"
#include "scream/dialogue.h"
#include "scream/item.h"
#include "scream/node.h"
#include "scream/object.h"
#include "scream/room.h"
#include "scream/track.h"
#include "scream/zoom.h"

namespace Scream {

class World {
public:
	~World();
	Track *boundTrack();
	Dialogue *boundDialogue();
	Choice *boundChoice();
	Item *boundItem();
	Node *boundNode();
	Room *boundRoom();
	Zoom *boundZoom();
	Item *findItem(const char *name);
	Object *boundObject();
	void scCreate(int kind, const char *name);
	void scInventoryReaction(Item *boundItem, Item *otherItem, const Action &action);
	void scBegin(int level);
	void scEnd(int level);
	void scTexture(int face, const char *file);
	void scPlace(int x, int y, int w, int h, int face);
	void setEnabled(int key, bool enabled);

	// TEMP:
	Node *getNode(int index);

private:
	Room *_currentRoom = nullptr;
	Track *_currentTrack = nullptr;
	Dialogue *_currentDialogue = nullptr;
	Choice *_currentChoice = nullptr;
	Item *_currentItem = nullptr;
	Node *_currentNode = nullptr;
	Zoom *_currentZoom = nullptr;
	Object *_currentObject = nullptr;

	int _currentLevel = kScWorld;

	Common::Array<Room *> _rooms;
	Common::Array<Track *> _tracks;
	Common::Array<Dialogue *> _dialogues;
	Common::Array<Choice *> _choices;
	Common::Array<Item *> _items;
	Common::Array<Node *> _nodes;
	Common::Array<Zoom *> _zooms;
	Common::Array<Object *> _objects;
};

} // end of namespace Scream

#endif
