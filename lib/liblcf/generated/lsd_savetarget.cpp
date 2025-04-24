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

// Read SaveTarget.

template <>
char const* const Struct<rpg::SaveTarget>::name = "SaveTarget";
static TypedField<rpg::SaveTarget, int32_t> static_map_id(
	&rpg::SaveTarget::map_id,
	LSD_Reader::ChunkSaveTarget::map_id,
	"map_id",
	0,
	0
);
static TypedField<rpg::SaveTarget, int32_t> static_map_x(
	&rpg::SaveTarget::map_x,
	LSD_Reader::ChunkSaveTarget::map_x,
	"map_x",
	0,
	0
);
static TypedField<rpg::SaveTarget, int32_t> static_map_y(
	&rpg::SaveTarget::map_y,
	LSD_Reader::ChunkSaveTarget::map_y,
	"map_y",
	0,
	0
);
static TypedField<rpg::SaveTarget, bool> static_switch_on(
	&rpg::SaveTarget::switch_on,
	LSD_Reader::ChunkSaveTarget::switch_on,
	"switch_on",
	0,
	0
);
static TypedField<rpg::SaveTarget, int32_t> static_switch_id(
	&rpg::SaveTarget::switch_id,
	LSD_Reader::ChunkSaveTarget::switch_id,
	"switch_id",
	0,
	0
);


template <>
Field<rpg::SaveTarget> const* Struct<rpg::SaveTarget>::fields[] = {
	&static_map_id,
	&static_map_x,
	&static_map_y,
	&static_switch_on,
	&static_switch_id,
	NULL
};

template class Struct<rpg::SaveTarget>;

} //namespace lcf
