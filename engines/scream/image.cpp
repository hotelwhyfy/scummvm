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

#include "common/file.h"
#include "common/path.h"
#include "common/str.h"
#include "common/textconsole.h"

#include "image/image_decoder.h"
#include "image/jpeg.h"
#include "image/tga.h"

#include "graphics/surface.h"

#include "scream/gfx.h"
#include "scream/image.h"

namespace Scream {

GLuint loadTexture(Gfx *gfx, const Common::String &fileName) {
	Common::File file;
	if (!file.open(Common::Path(fileName)))
		error("Could not open file: %s", fileName.c_str());

	const Graphics::PixelFormat &pixelFormat = gfx->getPixelFormat();

	Image::ImageDecoder *decoder;
	if (fileName.hasSuffixIgnoreCase(".tga")) {
		decoder = new Image::TGADecoder();
	} else {
		Image::JPEGDecoder *jpegDecoder = new Image::JPEGDecoder();
		jpegDecoder->setOutputPixelFormat(pixelFormat);
		decoder = jpegDecoder;
	}

	if (!decoder->loadStream(file))
		error("could not load image stream: %s", fileName.c_str());

	const Graphics::Surface *surface = decoder->getSurface();

	Graphics::Surface *converted = nullptr;
	if (surface->format != pixelFormat) {
		converted = surface->convertTo(pixelFormat);
		surface = converted;
	}

	GLuint texture = gfx->createTexture(surface);

	if (converted) {
		converted->free();
		delete converted;
	}

	delete decoder;

	return texture;
}

} // end of namespace Scream
