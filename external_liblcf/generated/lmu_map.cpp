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
#include "lcf/lmu/reader.h"
#include "lcf/lmu/chunks.h"
#include "reader_struct_impl.h"

namespace lcf {

// Read Map.

template <>
char const* const Struct<rpg::Map>::name = "Map";
static TypedField<rpg::Map, int32_t> static_chipset_id(
	&rpg::Map::chipset_id,
	LMU_Reader::ChunkMap::chipset_id,
	"chipset_id",
	0,
	0
);
static TypedField<rpg::Map, int32_t> static_width(
	&rpg::Map::width,
	LMU_Reader::ChunkMap::width,
	"width",
	0,
	0
);
static TypedField<rpg::Map, int32_t> static_height(
	&rpg::Map::height,
	LMU_Reader::ChunkMap::height,
	"height",
	0,
	0
);
static TypedField<rpg::Map, int32_t> static_scroll_type(
	&rpg::Map::scroll_type,
	LMU_Reader::ChunkMap::scroll_type,
	"scroll_type",
	1,
	0
);
static TypedField<rpg::Map, bool> static_parallax_flag(
	&rpg::Map::parallax_flag,
	LMU_Reader::ChunkMap::parallax_flag,
	"parallax_flag",
	0,
	0
);
static TypedField<rpg::Map, DBString> static_parallax_name(
	&rpg::Map::parallax_name,
	LMU_Reader::ChunkMap::parallax_name,
	"parallax_name",
	0,
	0
);
static TypedField<rpg::Map, bool> static_parallax_loop_x(
	&rpg::Map::parallax_loop_x,
	LMU_Reader::ChunkMap::parallax_loop_x,
	"parallax_loop_x",
	0,
	0
);
static TypedField<rpg::Map, bool> static_parallax_loop_y(
	&rpg::Map::parallax_loop_y,
	LMU_Reader::ChunkMap::parallax_loop_y,
	"parallax_loop_y",
	0,
	0
);
static TypedField<rpg::Map, bool> static_parallax_auto_loop_x(
	&rpg::Map::parallax_auto_loop_x,
	LMU_Reader::ChunkMap::parallax_auto_loop_x,
	"parallax_auto_loop_x",
	0,
	0
);
static TypedField<rpg::Map, int32_t> static_parallax_sx(
	&rpg::Map::parallax_sx,
	LMU_Reader::ChunkMap::parallax_sx,
	"parallax_sx",
	0,
	0
);
static TypedField<rpg::Map, bool> static_parallax_auto_loop_y(
	&rpg::Map::parallax_auto_loop_y,
	LMU_Reader::ChunkMap::parallax_auto_loop_y,
	"parallax_auto_loop_y",
	0,
	0
);
static TypedField<rpg::Map, int32_t> static_parallax_sy(
	&rpg::Map::parallax_sy,
	LMU_Reader::ChunkMap::parallax_sy,
	"parallax_sy",
	0,
	0
);
static TypedField<rpg::Map, bool> static_generator_flag(
	&rpg::Map::generator_flag,
	LMU_Reader::ChunkMap::generator_flag,
	"generator_flag",
	0,
	0
);
static TypedField<rpg::Map, int32_t> static_generator_mode(
	&rpg::Map::generator_mode,
	LMU_Reader::ChunkMap::generator_mode,
	"generator_mode",
	0,
	0
);
static TypedField<rpg::Map, bool> static_top_level(
	&rpg::Map::top_level,
	LMU_Reader::ChunkMap::top_level,
	"top_level",
	0,
	0
);
static TypedField<rpg::Map, int32_t> static_generator_tiles(
	&rpg::Map::generator_tiles,
	LMU_Reader::ChunkMap::generator_tiles,
	"generator_tiles",
	0,
	0
);
static TypedField<rpg::Map, int32_t> static_generator_width(
	&rpg::Map::generator_width,
	LMU_Reader::ChunkMap::generator_width,
	"generator_width",
	0,
	0
);
static TypedField<rpg::Map, int32_t> static_generator_height(
	&rpg::Map::generator_height,
	LMU_Reader::ChunkMap::generator_height,
	"generator_height",
	0,
	0
);
static TypedField<rpg::Map, bool> static_generator_surround(
	&rpg::Map::generator_surround,
	LMU_Reader::ChunkMap::generator_surround,
	"generator_surround",
	0,
	0
);
static TypedField<rpg::Map, bool> static_generator_upper_wall(
	&rpg::Map::generator_upper_wall,
	LMU_Reader::ChunkMap::generator_upper_wall,
	"generator_upper_wall",
	0,
	0
);
static TypedField<rpg::Map, bool> static_generator_floor_b(
	&rpg::Map::generator_floor_b,
	LMU_Reader::ChunkMap::generator_floor_b,
	"generator_floor_b",
	0,
	0
);
static TypedField<rpg::Map, bool> static_generator_floor_c(
	&rpg::Map::generator_floor_c,
	LMU_Reader::ChunkMap::generator_floor_c,
	"generator_floor_c",
	0,
	0
);
static TypedField<rpg::Map, bool> static_generator_extra_b(
	&rpg::Map::generator_extra_b,
	LMU_Reader::ChunkMap::generator_extra_b,
	"generator_extra_b",
	0,
	0
);
static TypedField<rpg::Map, bool> static_generator_extra_c(
	&rpg::Map::generator_extra_c,
	LMU_Reader::ChunkMap::generator_extra_c,
	"generator_extra_c",
	0,
	0
);
static TypedField<rpg::Map, std::vector<uint32_t>> static_generator_x(
	&rpg::Map::generator_x,
	LMU_Reader::ChunkMap::generator_x,
	"generator_x",
	0,
	0
);
static TypedField<rpg::Map, std::vector<uint32_t>> static_generator_y(
	&rpg::Map::generator_y,
	LMU_Reader::ChunkMap::generator_y,
	"generator_y",
	0,
	0
);
static TypedField<rpg::Map, std::vector<int16_t>> static_generator_tile_ids(
	&rpg::Map::generator_tile_ids,
	LMU_Reader::ChunkMap::generator_tile_ids,
	"generator_tile_ids",
	0,
	0
);
static TypedField<rpg::Map, std::vector<int16_t>> static_lower_layer(
	&rpg::Map::lower_layer,
	LMU_Reader::ChunkMap::lower_layer,
	"lower_layer",
	1,
	0
);
static TypedField<rpg::Map, std::vector<int16_t>> static_upper_layer(
	&rpg::Map::upper_layer,
	LMU_Reader::ChunkMap::upper_layer,
	"upper_layer",
	1,
	0
);
static TypedField<rpg::Map, std::vector<rpg::Event>> static_events(
	&rpg::Map::events,
	LMU_Reader::ChunkMap::events,
	"events",
	1,
	0
);
static TypedField<rpg::Map, int32_t> static_save_count_2k3e(
	&rpg::Map::save_count_2k3e,
	LMU_Reader::ChunkMap::save_count_2k3e,
	"save_count_2k3e",
	0,
	1
);
static TypedField<rpg::Map, int32_t> static_save_count(
	&rpg::Map::save_count,
	LMU_Reader::ChunkMap::save_count,
	"save_count",
	0,
	0
);


template <>
Field<rpg::Map> const* Struct<rpg::Map>::fields[] = {
	&static_chipset_id,
	&static_width,
	&static_height,
	&static_scroll_type,
	&static_parallax_flag,
	&static_parallax_name,
	&static_parallax_loop_x,
	&static_parallax_loop_y,
	&static_parallax_auto_loop_x,
	&static_parallax_sx,
	&static_parallax_auto_loop_y,
	&static_parallax_sy,
	&static_generator_flag,
	&static_generator_mode,
	&static_top_level,
	&static_generator_tiles,
	&static_generator_width,
	&static_generator_height,
	&static_generator_surround,
	&static_generator_upper_wall,
	&static_generator_floor_b,
	&static_generator_floor_c,
	&static_generator_extra_b,
	&static_generator_extra_c,
	&static_generator_x,
	&static_generator_y,
	&static_generator_tile_ids,
	&static_lower_layer,
	&static_upper_layer,
	&static_events,
	&static_save_count_2k3e,
	&static_save_count,
	NULL
};

template class Struct<rpg::Map>;

} //namespace lcf
