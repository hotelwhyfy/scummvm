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

#include "common/rect.h"
#include "math/matrix3.h"
#include "math/utils.h"

#include "scream/camera.h"
#include "scream/constants.h"

namespace Scream {

static const float kDefaultFov = 45.0f;
static const int kFaceImageSize = 1024;
static const int kCameraSpeed = 50;
static const float kLookYLimit = 1.75f;
static const float kTurnPixelFactor = 0.005f;
static const float kCamSpeedScale = 0.001f;
static const int kTurnZonePercent = 15;
static const float kTurnSpeedRad = (kTurnPixelFactor * kCameraSpeed * kCamSpeedScale); // radians per pixel-past-zone-edge per frame
static const float kRadInDeg = (180.0f / (float)M_PI);
static const float kTurnSpeedDeg = kTurnSpeedRad * kRadInDeg; // degrees per pixel-past-zone-edge per frame
static const int kMouseLeftZoneEdge = kScreenWidth * kTurnZonePercent / 100;
static const int kMouseRightZoneEdge = kScreenWidth - kMouseLeftZoneEdge;
static const int kMouseUpZoneEdge = kViewportY + (kScreenHeight - 3 * kViewportY) * kTurnZonePercent / 100;
static const int kMouseDownZoneEdge = (kScreenHeight - 2 * kViewportY) - (kScreenHeight - 3 * kViewportY) * kTurnZonePercent / 100;
static const float kLookUpY = 6.28f;
static const float kLookDownY = -1.57f;

Camera::Camera() : _fov(kDefaultFov) {}

static Math::Vector3d mouseToDirection(const Common::Point &mouse, float fov, float heading, float pitch) {
	float fovRatio = tan(Math::deg2rad(fov / 2));
	float nx = 2 * ((float)mouse.x / kScreenWidth) - 1;
	float ny = 1 - 2 * (((float)mouse.y - kViewportY) / kViewportHeight);
	float x = nx * fovRatio * kViewportAspect;
	float y = ny * fovRatio;
	float z = -1;

	Math::Vector3d v(x, y, z);

	Math::Matrix3 rotationX;
	rotationX.buildAroundX(pitch);
	rotationX.transformVector(&v);

	Math::Matrix3 rotationY;
	rotationY.buildAroundY(-heading);
	rotationY.transformVector(&v);

	return v;
}

static Math::Vector3d directionToHitPoint(const Math::Vector3d &direction) {
	float x = direction.x();
	float y = direction.y();
	float z = direction.z();
	float max = MAX(fabs(x), MAX(fabs(y), fabs(z)));
	return direction / max;
}

static int directionToFace(const Math::Vector3d &direction) {
	float x = direction.x();
	float y = direction.y();
	float z = direction.z();
	float xAbs = fabs(x);
	float yAbs = fabs(y);
	float zAbs = fabs(z);
	if (xAbs > yAbs && xAbs > zAbs) {
		return (x > 0) ? kScRight : kScLeft;
	} else if (yAbs > xAbs && yAbs > zAbs) {
		return (y > 0) ? kScUp : kScDown;
	} else {
		return (z > 0) ? kScBack : kScFront;
	}
}

static Common::Point hitPointToFacePixels(const Math::Vector3d &hit, int face) {
	switch (face) {
	case kScFront: {
		float px = kFaceImageSize * (hit.x() + 1) / 2;
		float py = kFaceImageSize * (1 - hit.y()) / 2;
		return Common::Point(px, py);
	}
	case kScBack: {
		float px = kFaceImageSize * (1 - hit.x()) / 2;
		float py = kFaceImageSize * (1 - hit.y()) / 2;
		return Common::Point(px, py);
	}
	case kScLeft: {
		float px = kFaceImageSize * (1 - hit.z()) / 2;
		float py = kFaceImageSize * (1 - hit.y()) / 2;
		return Common::Point(px, py);
	}
	case kScRight: {
		float px = kFaceImageSize * (hit.z() + 1) / 2;
		float py = kFaceImageSize * (1 - hit.y()) / 2;
		return Common::Point(px, py);
	}
	case kScUp: {
		float px = kFaceImageSize * (hit.x() + 1) / 2;
		float py = kFaceImageSize * (1 - hit.z()) / 2;
		return Common::Point(px, py);
	}
	case kScDown: {
		float px = kFaceImageSize * (hit.x() + 1) / 2;
		float py = kFaceImageSize * (hit.z() + 1) / 2;
		return Common::Point(px, py);
	}
	default:
		return Common::Point(-1, -1);
	}
}

static float calculatePitch(float lookY) {
	return Math::rad2deg(atan(lookY));
}

static float wrap(float input, float min, float max) {
	float range = max - min;
	return min + fmod(fmod(input - min, range) + range, range);
}

bool Camera::setTarget(int face) {
	switch (face) {
	case kScFront:
		_targetHeading = 0.0f;
		_targetLookY = 0.0f;
		break;
	case kScBack:
		_targetHeading = 180.0f;
		_targetLookY = 0.0f;
		break;
	case kScLeft:
		_targetHeading = 270.0f;
		_targetLookY = 0.0f;
		break;
	case kScRight:
		_targetHeading = 90.0f;
		_targetLookY = 0.0f;
		break;
	case kScUp:
		_targetHeading = 0.0f;
		_targetLookY = kLookUpY;
		break;
	case kScDown:
		_targetHeading = 0.0f;
		_targetLookY = kLookDownY;
		break;
	default:
		warning("Camera::setTarget: invalid face %d", face);
		return false;
	}
	return true;
}

void Camera::lookAt(int face) {
	if (setTarget(face)) {
		_heading = _targetHeading;
		_lookY = _targetLookY;
		_pitch = calculatePitch(_lookY);
	}
}

void Camera::update(const Common::Point &mouse) {
	if (_isTurning) {
		float dx = wrap(_targetHeading - _heading, -180, 180);
		float dy = _targetLookY - _lookY;
		float stepX = (copysign(kRadInDeg, dx) + dx) * 0.01f; // eased turn from original `step = (1 + d_rad) × 0.01`
		float stepY = (copysign(1.0f, dy) + dy) * 0.01f;
		_heading = fabs(stepX) >= fabs(dx) ? _targetHeading : _heading + stepX;
		_lookY = fabs(stepY) >= fabs(dy) ? _targetLookY : _lookY + stepY;
		_pitch = calculatePitch(_lookY);
		if (_heading == _targetHeading && _lookY == _targetLookY)
			_isTurning = false;
		return;
	}

	if (mouse.x < kMouseLeftZoneEdge) {
		_heading -= (kMouseLeftZoneEdge - mouse.x) * kTurnSpeedDeg;
		if (_heading < 0)
			_heading += 360;
	} else if (mouse.x > kMouseRightZoneEdge) {
		_heading += (mouse.x - kMouseRightZoneEdge) * kTurnSpeedDeg;
		if (_heading >= 360)
			_heading -= 360;
	}

	if (mouse.y < kMouseUpZoneEdge && _lookY <= kLookYLimit) {
		_lookY += (kMouseUpZoneEdge - mouse.y) * kTurnSpeedRad;
	} else if (mouse.y > kMouseDownZoneEdge && _lookY >= -kLookYLimit) {
		_lookY -= (mouse.y - kMouseDownZoneEdge) * kTurnSpeedRad;
	}

	_pitch = calculatePitch(_lookY);
}

FaceHit Camera::pick(const Common::Point &mouse) const {
	bool inBand = mouse.y > (int)kViewportY && mouse.y < (int)(kViewportY + kViewportHeight);

	if (!inBand)
		return FaceHit();

	Math::Vector3d v = mouseToDirection(mouse, _fov, _heading, _pitch);
	int face = directionToFace(v);
	Math::Vector3d hp = directionToHitPoint(v);
	Common::Point fp = hitPointToFacePixels(hp, face);

	return FaceHit(face, fp);
}

void Camera::turnTo(int face) {
	if (face == kScDown)
		face = kScFront;
	if (setTarget(face)) {
		_isTurning = true;
	}
}

} // end of namespace Scream
