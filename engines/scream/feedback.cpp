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
#include "common/util.h"

#include "scream/feedback.h"
#include "scream/gfx.h"

namespace Scream {

static const uint kFeedbackLineLength = 100;

void Feedback::set(const Common::String &text, uint textSpeed) {
	if (text.size() >= 400)
		return;

	_lines.clear();

	if (text.empty())
		return;

	uint start = 0;
	uint pos = 0;
	uint len = text.size();
	uint lines = (len + kFeedbackLineLength - 1) / kFeedbackLineLength;
	uint share = len / lines;

	// split lines on first space char after line length limit
	while (pos <= len) {
		bool endOfText = pos == len;
		bool filledLines = _lines.size() == lines - 1;
		bool filledShare = pos - start >= share;
		if (endOfText || (!filledLines && filledShare && text[pos] == ' ')) {
			Common::String sub = text.substr(start, pos - start);
			sub.replace('|', ' ');
			_lines.push_back(sub);
			start = pos + 1;
		}
		++pos;
	}

	uint effectiveLength = (len <= 16) ? 32 : len; // from spec: "Texts of 16 characters or fewer count as 32"
	_durationMs = (100 / textSpeed + effectiveLength / 16) * 1000;
	_startTime = g_system->getMillis();
	_transparency = 1.0f;
}

void Feedback::draw(Gfx *gfx, Font &font, uint textSpeed) {
	uint32 now = g_system->getMillis();
	uint32 elapsed = now - _startTime;

	if (elapsed > _durationMs && _transparency >= 1.0f)
		return;

	uint firstIndex = (_lines.size() == 4 && elapsed > _durationMs / 2) ? 1 : 0; // display lines 1-3 for half duration, then lines 2-4 for remainder
	uint count = _lines.size() == 4 ? 3 : _lines.size();

	for (uint i = firstIndex; i < firstIndex + count; ++i) {
		if (elapsed <= _durationMs && _transparency > 0.0f)
			_transparency -= textSpeed * 0.001f;
		else if (elapsed > _durationMs && _transparency < 1.0f)
			_transparency += textSpeed * 0.0001f;

		int x = (kScreenWidth / 2) - 10 * (_lines[i].size() / 2) + 1;
		int y = 24 + 8 * (count - 1) - 16 * (i - firstIndex);
		font.drawText(gfx, x, y, _lines[i], 1.0f - _transparency);
	}
}

} // end of namespace Scream
