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
const uint kViewportY = 64;
const uint kInventoryIconSize = 64;
const uint kInventoryLeft = 32;
const uint kInventoryGap = 32;
const uint kInventoryDrawTop = 66;
const uint kInventoryClickTop = 64;
const uint kInventoryMaxSlot = 60;
const uint kInventoryColumns = 10;
const uint kInventoryRows = 7;
const uint kViewportHeight = kScreenHeight - (2 * kViewportY);
const float kViewportAspect = (float)kScreenWidth / kViewportHeight;

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
	kScTextSpeed = 1017,
	kScMovSpeed = 1018,
	kScHints = 1029,
	kScLanguage = 1038,
	kScCamSpeed = 1016,
	kScSkySpeed = 1019,
	kScQuality = 1007,
	kScSpeed = 1009,
	kScLog = 1010,
	kScQuickMode = 1011,
	kScFullScreen = 1012,
	kScResource = 1013,
};

enum QualityKey {
	kScLo = 101,
	kScMed = 102,
	kScHi = 103,
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

enum TransitionKind {
	kScInstant = 1,
	kScSmooth = 2,
};

} // end of namespace Scream

#endif
