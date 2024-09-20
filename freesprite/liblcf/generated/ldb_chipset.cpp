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
#include "lcf/ldb/reader.h"
#include "lcf/ldb/chunks.h"
#include "reader_struct_impl.h"

namespace lcf {

// Read Chipset.

template <>
char const* const Struct<rpg::Chipset>::name = "Chipset";
static TypedField<rpg::Chipset, DBString> static_name(
	&rpg::Chipset::name,
	LDB_Reader::ChunkChipset::name,
	"name",
	0,
	0
);
static TypedField<rpg::Chipset, DBString> static_chipset_name(
	&rpg::Chipset::chipset_name,
	LDB_Reader::ChunkChipset::chipset_name,
	"chipset_name",
	0,
	0
);
static TypedField<rpg::Chipset, std::vector<int16_t>> static_terrain_data(
	&rpg::Chipset::terrain_data,
	LDB_Reader::ChunkChipset::terrain_data,
	"terrain_data",
	0,
	0
);
static TypedField<rpg::Chipset, std::vector<uint8_t>> static_passable_data_lower(
	&rpg::Chipset::passable_data_lower,
	LDB_Reader::ChunkChipset::passable_data_lower,
	"passable_data_lower",
	0,
	0
);
static TypedField<rpg::Chipset, std::vector<uint8_t>> static_passable_data_upper(
	&rpg::Chipset::passable_data_upper,
	LDB_Reader::ChunkChipset::passable_data_upper,
	"passable_data_upper",
	0,
	0
);
static TypedField<rpg::Chipset, int32_t> static_animation_type(
	&rpg::Chipset::animation_type,
	LDB_Reader::ChunkChipset::animation_type,
	"animation_type",
	0,
	0
);
static TypedField<rpg::Chipset, int32_t> static_animation_speed(
	&rpg::Chipset::animation_speed,
	LDB_Reader::ChunkChipset::animation_speed,
	"animation_speed",
	0,
	0
);


template <>
Field<rpg::Chipset> const* Struct<rpg::Chipset>::fields[] = {
	&static_name,
	&static_chipset_name,
	&static_terrain_data,
	&static_passable_data_lower,
	&static_passable_data_upper,
	&static_animation_type,
	&static_animation_speed,
	NULL
};

template class Struct<rpg::Chipset>;

} //namespace lcf
