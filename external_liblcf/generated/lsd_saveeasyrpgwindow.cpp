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

// Headers
#include "lcf/lsd/reader.h"
#include "lcf/lsd/chunks.h"
#include "reader_struct_impl.h"

namespace lcf {

// Read SaveEasyRpgWindow.

template <>
char const* const Struct<rpg::SaveEasyRpgWindow>::name = "SaveEasyRpgWindow";
static TypedField<rpg::SaveEasyRpgWindow, std::vector<rpg::SaveEasyRpgText>> static_texts(
	&rpg::SaveEasyRpgWindow::texts,
	LSD_Reader::ChunkSaveEasyRpgWindow::texts,
	"texts",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgWindow, int32_t> static_width(
	&rpg::SaveEasyRpgWindow::width,
	LSD_Reader::ChunkSaveEasyRpgWindow::width,
	"width",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgWindow, int32_t> static_height(
	&rpg::SaveEasyRpgWindow::height,
	LSD_Reader::ChunkSaveEasyRpgWindow::height,
	"height",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgWindow, DBString> static_system_name(
	&rpg::SaveEasyRpgWindow::system_name,
	LSD_Reader::ChunkSaveEasyRpgWindow::system_name,
	"system_name",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgWindow, int32_t> static_message_stretch(
	&rpg::SaveEasyRpgWindow::message_stretch,
	LSD_Reader::ChunkSaveEasyRpgWindow::message_stretch,
	"message_stretch",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgWindow, rpg::SaveEasyRpgWindow::Flags> static_flags(
	&rpg::SaveEasyRpgWindow::flags,
	LSD_Reader::ChunkSaveEasyRpgWindow::flags,
	"flags",
	0,
	0
);


template <>
Field<rpg::SaveEasyRpgWindow> const* Struct<rpg::SaveEasyRpgWindow>::fields[] = {
	&static_texts,
	&static_width,
	&static_height,
	&static_system_name,
	&static_message_stretch,
	&static_flags,
	NULL
};

template class Struct<rpg::SaveEasyRpgWindow>;

} //namespace lcf
