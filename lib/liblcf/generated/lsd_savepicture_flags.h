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
#include "lcf/lsd/reader.h"
#include "lcf/lsd/chunks.h"
#include "reader_struct.h"

namespace lcf {

// Read SavePicture.

template <>
char const* const Flags<rpg::SavePicture::Flags>::name = "SavePicture_Flags";

template <>
decltype(Flags<rpg::SavePicture::Flags>::flag_names) Flags<rpg::SavePicture::Flags>::flag_names = {
	"erase_on_map_change",
	"erase_on_battle_end",
	"unused_bit",
	"unused_bit2",
	"affected_by_tint",
	"affected_by_flash",
	"affected_by_shake"
};

template <>
decltype(Flags<rpg::SavePicture::Flags>::flags_is2k3) Flags<rpg::SavePicture::Flags>::flags_is2k3 = {
	1,
	1,
	1,
	1,
	1,
	1,
	1
};

} //namespace lcf
