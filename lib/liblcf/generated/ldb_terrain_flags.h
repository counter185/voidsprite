/* !!!! GENERATED FILE - DO NOT EDIT !!!!
 * --------------------------------------
 *
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

/*
 * Headers
 */
#include "lcf/ldb/reader.h"
#include "lcf/ldb/chunks.h"
#include "reader_struct.h"

namespace lcf {

// Read Terrain.

template <>
char const* const Flags<rpg::Terrain::Flags>::name = "Terrain_Flags";

template <>
decltype(Flags<rpg::Terrain::Flags>::flag_names) Flags<rpg::Terrain::Flags>::flag_names = {
	"back_party",
	"back_enemies",
	"lateral_party",
	"lateral_enemies"
};

template <>
decltype(Flags<rpg::Terrain::Flags>::flags_is2k3) Flags<rpg::Terrain::Flags>::flags_is2k3 = {
	1,
	1,
	1,
	1
};

} //namespace lcf
