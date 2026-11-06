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
	Object *boundObject();

	Item *findItem(const Common::String &name) const;
	Node *findNode(const Room *room, const Common::String &name) const;
	Room *findRoom(const Common::String &name) const;
	Zoom *findZoom(const Common::String &name) const;

	Node *getCurrentNode() const { return _currentNode; }
	Room *getCurrentRoom() const { return _currentNode->getParentRoom(); }
	Zoom *getCurrentZoom() const { return getCurrentRoom()->getTopZoom(); }

	void setBoundRoom(Room *room) { _boundRoom = room; }
	void setBoundItem(Item *item) { _boundItem = item; }

	void scCreate(int kind, const char *name);
	void scInventoryReaction(Item *boundItem, Item *otherItem, const Action &action);
	void scBegin(int level);
	void scEnd(int level);
	void scTexture(int face, const char *file);
	void scPlace(int x, int y, int w, int h, int face);
	void setCurrentNode(Node *node) { _currentNode = node; }
	void switchTo(int kind, const Common::String &name);

private:
	void switchNode(const Common::String &name);
	void switchRoom(const Common::String &name);
	void switchZoom(const Common::String &name);
	void zoomOut();

	Room *_boundRoom = nullptr;
	Track *_boundTrack = nullptr;
	Dialogue *_boundDialogue = nullptr;
	Choice *_boundChoice = nullptr;
	Item *_boundItem = nullptr;
	Node *_boundNode = nullptr;
	Zoom *_boundZoom = nullptr;
	Object *_boundObject = nullptr;

	Node *_currentNode = nullptr;

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
