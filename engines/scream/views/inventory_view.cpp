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

#include "common/array.h"

#include "scream/constants.h"
#include "scream/gfx.h"
#include "scream/image.h"
#include "scream/views/inventory_view.h"
#include "scream/world/item.h"

namespace Scream {

static const float kMaxFade = 0.5f;
static const float kFadeStep = 0.001f;
static const float kTintR = 0.9f;
static const float kTintG = 0.75f;
static const float kTintB = 0.5f;

Item *InventoryView::findItemAt(const Inventory &inventory, const Common::Point &mouse) const {
	const Common::Array<Item *> &items = inventory.getItems();
	for (uint i = 0; i < items.size() && i < kInventoryMaxSlot - 1; ++i) {
		uint column = i % kInventoryColumns;
		uint row = i / kInventoryColumns;
		uint x = kInventoryLeft + column * (kInventoryIconSize + kInventoryGap);
		uint y = kInventoryClickTop + row * (kInventoryIconSize + kInventoryGap);
		Common::Rect rect(x, y, x + kInventoryIconSize, y + kInventoryIconSize);
		if (rect.contains(mouse))
			return items[i];
	}
	return nullptr;
}

void InventoryView::draw(Gfx *gfx, const Inventory &inventory, uint speed) {
	if (!isOpen())
		return;
	float k = 1.0f - _fade;
	gfx->tint(kTintR * k, kTintG * k, kTintB * k);
	const Common::Array<Item *> &items = inventory.getItems();
	for (uint i = 0; i < items.size(); ++i) {
		if (items[i] == inventory.getHeld())
			continue;
		Common::String icon = items[i]->getIcon();
		GLuint texture = getIconTexture(gfx, icon);
		uint column = i % kInventoryColumns;
		uint row = i / kInventoryColumns;
		uint x = kInventoryLeft + column * (kInventoryIconSize + kInventoryGap);
		uint y = kScreenHeight - kInventoryDrawTop - kInventoryIconSize - (kInventoryIconSize + kInventoryGap) * row;
		gfx->drawTexture2D(texture, x, y, kInventoryIconSize, kInventoryIconSize, 0.0f, 0.0f, 1.0f, 1.0f, _fade * 2);
	}
	_fade = MIN(kMaxFade, _fade + speed * kFadeStep);
}

GLuint InventoryView::getIconTexture(Gfx *gfx, const Common::String &icon) {
	if (!_iconTextures.contains(icon))
		_iconTextures[icon] = loadTexture(gfx, icon);
	return _iconTextures[icon];
}

void InventoryView::freeTextures(Gfx *gfx) {
	for (auto &entry : _iconTextures)
		gfx->deleteTexture(entry._value);
	_iconTextures.clear();
}

void InventoryView::toggle(const Inventory &inventory) {
	if (_open) {
		close();
	} else if (inventory.count() > 0) {
		if (_fade >= kMaxFade)
			_fade = 0.0f;
		_open = true;
	}
}

void InventoryView::close() {
	_open = false;
}

} // end of namespace Scream
