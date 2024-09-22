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

// Read SaveEasyRpgText.

template <>
char const* const Struct<rpg::SaveEasyRpgText>::name = "SaveEasyRpgText";
static TypedField<rpg::SaveEasyRpgText, DBString> static_text(
	&rpg::SaveEasyRpgText::text,
	LSD_Reader::ChunkSaveEasyRpgText::text,
	"text",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgText, int32_t> static_position_x(
	&rpg::SaveEasyRpgText::position_x,
	LSD_Reader::ChunkSaveEasyRpgText::position_x,
	"position_x",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgText, int32_t> static_position_y(
	&rpg::SaveEasyRpgText::position_y,
	LSD_Reader::ChunkSaveEasyRpgText::position_y,
	"position_y",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgText, DBString> static_font_name(
	&rpg::SaveEasyRpgText::font_name,
	LSD_Reader::ChunkSaveEasyRpgText::font_name,
	"font_name",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgText, int32_t> static_font_size(
	&rpg::SaveEasyRpgText::font_size,
	LSD_Reader::ChunkSaveEasyRpgText::font_size,
	"font_size",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgText, int32_t> static_letter_spacing(
	&rpg::SaveEasyRpgText::letter_spacing,
	LSD_Reader::ChunkSaveEasyRpgText::letter_spacing,
	"letter_spacing",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgText, int32_t> static_line_spacing(
	&rpg::SaveEasyRpgText::line_spacing,
	LSD_Reader::ChunkSaveEasyRpgText::line_spacing,
	"line_spacing",
	0,
	0
);
static TypedField<rpg::SaveEasyRpgText, rpg::SaveEasyRpgText::Flags> static_flags(
	&rpg::SaveEasyRpgText::flags,
	LSD_Reader::ChunkSaveEasyRpgText::flags,
	"flags",
	0,
	0
);


template <>
Field<rpg::SaveEasyRpgText> const* Struct<rpg::SaveEasyRpgText>::fields[] = {
	&static_text,
	&static_position_x,
	&static_position_y,
	&static_font_name,
	&static_font_size,
	&static_letter_spacing,
	&static_line_spacing,
	&static_flags,
	NULL
};

template class Struct<rpg::SaveEasyRpgText>;

} //namespace lcf
