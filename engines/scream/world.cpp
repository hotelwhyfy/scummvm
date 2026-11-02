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

#include "common/textconsole.h"

#include "scream/constants.h"
#include "scream/world.h"

namespace Scream {

static int parentLevel(int level) {
	switch (level) {
	case kScRoom:
		return kScWorld;
	case kScNode:
	case kScZoom:
		return kScRoom;
	default:
		return -1;
	}
}

World::~World() {
	for (Track *track : _tracks)
		delete track;

	for (Room *room : _rooms)
		delete room;

	for (Dialogue *dialogue : _dialogues)
		delete dialogue;

	for (Choice *choice : _choices)
		delete choice;

	for (Item *item : _items)
		delete item;

	for (Node *node : _nodes)
		delete node;

	for (Zoom *zoom : _zooms)
		delete zoom;

	for (Object *object : _objects)
		delete object;
}

Track *World::boundTrack() {
	if (!_boundTrack)
		error("GetBind: no bound track");
	return _boundTrack;
}

Dialogue *World::boundDialogue() {
	if (!_boundDialogue)
		error("GetBind: no bound dialogue");
	return _boundDialogue;
}

Choice *World::boundChoice() {
	if (!_boundChoice)
		error("GetBind: no bound choice");
	return _boundChoice;
}

Item *World::boundItem() {
	if (!_boundItem)
		error("GetBind: no bound item"); // engine only warns
	return _boundItem;
}

Item *World::findItem(const Common::String &name) {
	for (Item *item : _items) {
		if (item->getName() == name) {
			return item;
		}
	}
	return nullptr;
}

Room *World::boundRoom() {
	if (!_boundRoom)
		error("GetBind: no bound room");
	return _boundRoom;
}

Node *World::boundNode() {
	if (!_boundNode)
		error("GetBind: no bound node");
	return _boundNode;
}

Zoom *World::boundZoom() {
	if (!_boundZoom)
		error("GetBind: no bound zoom");
	return _boundZoom;
}

Object *World::boundObject() {
	if (!_boundObject)
		error("GetBind: no bound object");
	return _boundObject;
}

Node *World::findNode(const Room *room, const Common::String &name) const {
	for (Node *node : _nodes)
		if (node->getName() == name && node->getParentRoom() == room)
			return node;
	return nullptr;
}

Room *World::findRoom(const Common::String &name) const {
	for (Room *room : _rooms)
		if (room->getName() == name)
			return room;
	return nullptr;
}

void World::setEnabled(int key, bool enabled) {
	switch (key) {
	case kScObject:
		boundObject()->setEnabled(enabled);
		break;
	default:
		warning("unhandled key for scEnable/scDisable %d", key);
		break;
	}
}

void World::scPlace(int x, int y, int w, int h, int face) {
	Object *object = boundObject();
	object->setRect(x, y, w, h, face);
	if (_currentLevel == kScZoom) {
		boundZoom()->addObject(object);
	} else {
		boundNode()->addObject(object);
	}
}

void World::scTexture(int face, const char *file) {
	if (face == kScZoom) {
		boundZoom()->setImageFile(file);
	} else {
		boundNode()->setFace(face, file);
	}
}

void World::scEnd(int level) {
	if (_currentLevel != level) {
		warning("scEnd: invalid call %d when current level is %d", level, _currentLevel);
		return;
	}
	_currentLevel = parentLevel(level);
}

void World::scBegin(int level) {
	if (_currentLevel != parentLevel(level)) {
		warning("scBegin: invalid level %d when current level is %d", level, _currentLevel);
		return;
	}
	_currentLevel = level;
	if (level == kScNode) {
		boundNode()->setParentRoom(boundRoom());
		if (!boundRoom()->getLastVisitedNode())
			boundRoom()->setLastVisitedNode(boundNode());
	}
}

void World::scInventoryReaction(Item *boundItem, Item *otherItem, const Action &action) {
	boundItem->addReaction(otherItem, action);
	if (otherItem->getReactionCount() >= kScMaxReactions)
		error("Too many reactions!");
	otherItem->addReaction(boundItem, action);
}

void World::scCreate(int kind, const char *name) {
	switch (kind) {
	case kScTrack: {
		Track *track = new Track(name);
		_tracks.push_back(track);
		_boundTrack = track;
		break;
	}
	case kScRoom: {
		Room *room = new Room(name);
		_rooms.push_back(room);
		_boundRoom = room;
		break;
	}
	case kScScript: {
		Dialogue *dialogue = new Dialogue(name);
		_dialogues.push_back(dialogue);
		_boundDialogue = dialogue;
		break;
	}
	case kScChoice: {
		Choice *choice = new Choice(name);
		_choices.push_back(choice);
		_boundChoice = choice;
		break;
	}
	case kScItem: {
		Item *item = new Item(name);
		_items.push_back(item);
		_boundItem = item;
		break;
	}
	case kScNode: {
		Node *node = new Node(name);
		_nodes.push_back(node);
		_boundNode = node;
		break;
	}
	case kScZoom: {
		Zoom *zoom = new Zoom(name);
		_zooms.push_back(zoom);
		_boundZoom = zoom;
		break;
	}
	case kScObject: {
		Object *object = new Object(name);
		_objects.push_back(object);
		_boundObject = object;
		break;
	}

	default:
		warning("scCreate kind not implemented: %d", kind);
		break;
	}
}

} // end of namespace Scream
