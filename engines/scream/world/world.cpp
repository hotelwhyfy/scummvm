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
#include "scream/world/world.h"

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

template<typename T>
static void deleteAll(const Common::Array<T *> &list) {
	for (T *element : list)
		delete element;
}

template<typename T>
static T *findByName(const Common::Array<T *> &list, const Common::String &name) {
	for (T *element : list)
		if (element->getName() == name)
			return element;
	return nullptr;
}

template<typename T>
static T *ensure(T *element, const char *type) {
	if (!element)
		error("World: no %s", type);
	return element;
}

template<typename T>
static T *create(Common::Array<T *> &list, T *&bound, const char *name) {
	T *element = new T(name);
	list.push_back(element);
	bound = element;
	return element;
}

World::~World() {
	deleteAll(_tracks);
	deleteAll(_rooms);
	deleteAll(_dialogues);
	deleteAll(_choices);
	deleteAll(_items);
	deleteAll(_nodes);
	deleteAll(_zooms);
	deleteAll(_objects);
}

Track *World::boundTrack() const { return ensure(_boundTrack, "bound track"); }
Dialogue *World::boundDialogue() const { return ensure(_boundDialogue, "bound dialogue"); }
Choice *World::boundChoice() const { return ensure(_boundChoice, "bound choice"); }
Item *World::boundItem() const { return ensure(_boundItem, "bound item"); } // engine only warns
Room *World::boundRoom() const { return ensure(_boundRoom, "bound room"); }
Node *World::boundNode() const { return ensure(_boundNode, "bound node"); }
Zoom *World::boundZoom() const { return ensure(_boundZoom, "bound zoom"); }
Object *World::boundObject() const { return ensure(_boundObject, "bound object"); }
Room *World::getCurrentRoom() const { return ensure(_currentRoom, "current room"); }

Item *World::findItem(const Common::String &name) const { return findByName(_items, name); }
Room *World::findRoom(const Common::String &name) const { return findByName(_rooms, name); }
Zoom *World::findZoom(const Common::String &name) const { return findByName(_zooms, name); }

Node *World::findNode(const Room *room, const Common::String &name) const {
	for (Node *node : _nodes)
		if (node->getName() == name && node->getParentRoom() == room)
			return node;
	return nullptr;
}

void World::setAmbience(int target, int preset) {
	switch (target) {
	case kScSkyColor:
		_atmosphere.skyPreset = preset;
		break;
	case kScCloudColor:
		_atmosphere.cloudPreset = preset;
		break;
	case kScScreenColor:
		_atmosphere.screenPreset = preset;
		break;
	default:
		warning("World::setAmbience: unknown target type %d", target);
		break;
	}
}

void World::switchTo(int kind, const Common::String &name) {
	switch (kind) {
	case kScRoom:
		switchRoom(name);
		break;
	case kScNode:
		switchNode(name);
		break;
	case kScZoom:
		switchZoom(name);
		break;
	case kScZoomOut:
		zoomOut();
		break;
	case kScZoomBack:
		getCurrentRoom()->clearZooms();
		break;
	case kScOverlay:
	default:
		warning("World::switchTo: not implemented for kind %d", kind);
		break;
	}
}

void World::scCreate(int kind, const char *name) {
	switch (kind) {
	case kScTrack:
		create(_tracks, _boundTrack, name);
		break;
	case kScRoom:
		_currentRoom = create(_rooms, _boundRoom, name);
		break;
	case kScScript:
		create(_dialogues, _boundDialogue, name);
		break;
	case kScChoice:
		create(_choices, _boundChoice, name);
		break;
	case kScItem:
		create(_items, _boundItem, name);
		break;
	case kScNode:
		create(_nodes, _boundNode, name);
		break;
	case kScZoom:
		create(_zooms, _boundZoom, name);
		break;
	case kScObject:
		create(_objects, _boundObject, name);
		break;
	default:
		warning("World::scCreate: kind not implemented: %d", kind);
		break;
	}
}

void World::scInventoryReaction(Item *boundItem, Item *otherItem, const Action &action) {
	boundItem->addReaction(otherItem, action);
	if (otherItem->getReactionCount() >= kScMaxReactions)
		error("World::scInventoryReaction: Too many reactions!");
	otherItem->addReaction(boundItem, action);
}

void World::scBegin(int level) {
	if (_currentLevel != parentLevel(level)) {
		warning("World::scBegin: invalid level %d when current level is %d", level, _currentLevel);
		return;
	}
	_currentLevel = level;
	if (level == kScNode) {
		boundNode()->setParentRoom(boundRoom()); // add node to currently bound room
		if (!boundRoom()->getLastVisitedNode())
			boundRoom()->setLastVisitedNode(boundNode()); // first visit to a room starts at the first node
	}
}

void World::scEnd(int level) {
	if (_currentLevel != level) {
		warning("World::scEnd: invalid call %d when current level is %d", level, _currentLevel);
		return;
	}
	_currentLevel = parentLevel(level);
}

void World::scTexture(int face, const char *file) {
	if (face == kScZoom) {
		boundZoom()->setImageFile(file);
	} else {
		boundNode()->setFace(face, file);
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

void World::switchNode(const Common::String &name) {
	Room *room = getCurrentRoom();
	Node *node = findNode(room, name);
	if (!node)
		error("World::switchNode: node %s not found", name.c_str());
	room->setLastVisitedNode(node);
	_currentNode = node;
	_boundNode = node;
}

void World::switchRoom(const Common::String &name) {
	Room *room = findRoom(name);
	if (!room)
		error("World::switchRoom: room %s not found", name.c_str());
	Node *node = room->getLastVisitedNode();
	if (!node)
		error("World::switchRoom: room %s has no last visited node", name.c_str());
	_currentRoom = room;
	_currentNode = node;
	_boundRoom = room;
}

void World::switchZoom(const Common::String &name) {
	Zoom *zoom = findZoom(name);
	if (!zoom)
		error("World::switchZoom: zoom %s not found", name.c_str());
	Room *room = getCurrentRoom();
	room->pushZoom(zoom);
	_boundZoom = zoom;
}

void World::zoomOut() {
	getCurrentRoom()->popZoom();
	Zoom *topZoom = getCurrentRoom()->getTopZoom();
	if (topZoom)
		_boundZoom = topZoom;
}

} // end of namespace Scream
