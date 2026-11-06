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

#ifndef SCREAM_VIEWS_INVENTORY_VIEW_H
#define SCREAM_VIEWS_INVENTORY_VIEW_H

#include "common/hashmap.h"
#include "common/rect.h"
#include "common/str.h"
#include "common/hash-str.h"
#include "graphics/opengl/system_headers.h"

#include "scream/inventory.h"

namespace Scream {

class Gfx;

class InventoryView {
public:
	Item *findItemAt(const Inventory &inventory, const Common::Point &mouse) const;
	void draw(Gfx *gfx, const Inventory &inventory);
	void drawHeld(Gfx *gfx, const Inventory &inventory, const Common::Point &mouse);
	void freeTextures(Gfx *gfx);

private:
	GLuint getIconTexture(Gfx *gfx, const Common::String &icon);
	Common::HashMap<Common::String, GLuint> _iconTextures;
};

} // end of namespace Scream

#endif
