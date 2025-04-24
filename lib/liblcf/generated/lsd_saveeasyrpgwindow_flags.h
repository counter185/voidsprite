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

// Read SaveEasyRpgWindow.

template <>
char const* const Flags<rpg::SaveEasyRpgWindow::Flags>::name = "SaveEasyRpgWindow_Flags";

template <>
decltype(Flags<rpg::SaveEasyRpgWindow::Flags>::flag_names) Flags<rpg::SaveEasyRpgWindow::Flags>::flag_names = {
	"draw_frame",
	"border_margin"
};

template <>
decltype(Flags<rpg::SaveEasyRpgWindow::Flags>::flags_is2k3) Flags<rpg::SaveEasyRpgWindow::Flags>::flags_is2k3 = {
	0,
	0
};

} //namespace lcf
