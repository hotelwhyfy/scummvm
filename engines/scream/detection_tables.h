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

namespace Scream {

const PlainGameDescriptor screamGames[] = {
	{ "scratches", "Scratches" },
	{ "scratchesdc", "Scratches: Director's Cut" },
	{ 0, 0 }
};

const ADGameDescription gameDescriptions[] = {
	{
		"scratches",
		nullptr,
		AD_ENTRY2s("scream.sc", "8872a463a0def9d5b0462c79fe3234d4", 5354,
		           "scratches.sc", "852e1ae640fb91420dbbd7cd3b23dcef", 952519),
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},
	{
		"scratchesdc",
		nullptr,
		AD_ENTRY3s("scream.sc", "668e7790d4a38241608218ed2ae973cb", 5889,
		           "scratches.sc", "82c46e5e3a447d4ecb7a72b5673d8b6a", 1124643,
		           "update.res", "41f051565e4502bf5e36a562e3f16d1c", 564153915),
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},
	{
		"scratchesdc",
		"Unpatched",
		AD_ENTRY2s("scream.sc", "668e7790d4a38241608218ed2ae973cb", 5889,
		           "scratches.sc", "6e82a72515ff30d2671c6f41a15ae40b", 1121152),
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},
	AD_TABLE_END_MARKER
};

} // End of namespace Scream
