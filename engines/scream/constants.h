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

#ifndef SCREAM_CONSTANTS_H
#define SCREAM_CONSTANTS_H

#include "common/scummsys.h"

namespace Scream {

const uint kScMaxReactions = 63;
const uint kNumFaces = 6;
const uint kCursorSize = 64;
const uint kScreenWidth = 1024;
const uint kScreenHeight = 768;

enum FaceKey {
	kScFront = 1,
	kScBack = 2,
	kScLeft = 3,
	kScRight = 4,
	kScUp = 5,
	kScDown = 6,
};

enum CursorKey {
	kScNormal = 21,
	kScForward = 22,
	kScLook = 23,
	kScUse = 24,
	kScWait = 25,
	kScMenuCursor = 3005,
};

enum SettingsKey {
	kScResPath = 1014,
	kScResFile = 1015,
	kScSplash = 1028,
};

enum AudioKey {
	kScStereo = 40,
	kScVolume = 41,
	kScFadeIn = 42,
	kScFadeOut = 43,
};

enum MenuKey {
	kScIntro = 3006,
	kScImage = 3008,
	kScOutro = 3012,
};

enum HintKey {
	kScHintsOpen = 2,
	kScHintsGrab = 3,
	kScHintsClose = 4,
	kScHintsItem = 5,
};

enum ObjectKind {
	kScRoom = 2000,
	kScNode = 2001,
	kScZoom = 2002,
	kScObject = 2003,
	kScOverlay = 2004,
	kScItem = 2005,
	kScScript = 2006,
	kScChoice = 2007,
	kScTrack = 2008,
};

enum ActionKind {
	kScFeedback = 30,
	kScCustom = 31,
	kScZoomOut = 32,
	kScZoomBack = 33,
	kScFeedbackEx = 34,
};

enum LevelKey {
	kScWorld = 3000,
};

} // end of namespace Scream

#endif
