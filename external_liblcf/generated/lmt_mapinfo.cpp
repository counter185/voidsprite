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
#include "lcf/lmt/reader.h"
#include "lcf/lmt/chunks.h"
#include "reader_struct_impl.h"

namespace lcf {

// Read MapInfo.

template <>
char const* const Struct<rpg::MapInfo>::name = "MapInfo";
static TypedField<rpg::MapInfo, DBString> static_name(
	&rpg::MapInfo::name,
	LMT_Reader::ChunkMapInfo::name,
	"name",
	0,
	0
);
static TypedField<rpg::MapInfo, int32_t> static_parent_map(
	&rpg::MapInfo::parent_map,
	LMT_Reader::ChunkMapInfo::parent_map,
	"parent_map",
	0,
	0
);
static TypedField<rpg::MapInfo, int32_t> static_indentation(
	&rpg::MapInfo::indentation,
	LMT_Reader::ChunkMapInfo::indentation,
	"indentation",
	0,
	0
);
static TypedField<rpg::MapInfo, int32_t> static_type(
	&rpg::MapInfo::type,
	LMT_Reader::ChunkMapInfo::type,
	"type",
	0,
	0
);
static TypedField<rpg::MapInfo, int32_t> static_scrollbar_x(
	&rpg::MapInfo::scrollbar_x,
	LMT_Reader::ChunkMapInfo::scrollbar_x,
	"scrollbar_x",
	0,
	0
);
static TypedField<rpg::MapInfo, int32_t> static_scrollbar_y(
	&rpg::MapInfo::scrollbar_y,
	LMT_Reader::ChunkMapInfo::scrollbar_y,
	"scrollbar_y",
	0,
	0
);
static TypedField<rpg::MapInfo, bool> static_expanded_node(
	&rpg::MapInfo::expanded_node,
	LMT_Reader::ChunkMapInfo::expanded_node,
	"expanded_node",
	0,
	0
);
static TypedField<rpg::MapInfo, int32_t> static_music_type(
	&rpg::MapInfo::music_type,
	LMT_Reader::ChunkMapInfo::music_type,
	"music_type",
	1,
	0
);
static TypedField<rpg::MapInfo, rpg::Music> static_music(
	&rpg::MapInfo::music,
	LMT_Reader::ChunkMapInfo::music,
	"music",
	1,
	0
);
static TypedField<rpg::MapInfo, int32_t> static_background_type(
	&rpg::MapInfo::background_type,
	LMT_Reader::ChunkMapInfo::background_type,
	"background_type",
	1,
	0
);
static TypedField<rpg::MapInfo, DBString> static_background_name(
	&rpg::MapInfo::background_name,
	LMT_Reader::ChunkMapInfo::background_name,
	"background_name",
	0,
	0
);
static TypedField<rpg::MapInfo, int32_t> static_teleport(
	&rpg::MapInfo::teleport,
	LMT_Reader::ChunkMapInfo::teleport,
	"teleport",
	1,
	0
);
static TypedField<rpg::MapInfo, int32_t> static_escape(
	&rpg::MapInfo::escape,
	LMT_Reader::ChunkMapInfo::escape,
	"escape",
	1,
	0
);
static TypedField<rpg::MapInfo, int32_t> static_save(
	&rpg::MapInfo::save,
	LMT_Reader::ChunkMapInfo::save,
	"save",
	1,
	0
);
static TypedField<rpg::MapInfo, std::vector<rpg::Encounter>> static_encounters(
	&rpg::MapInfo::encounters,
	LMT_Reader::ChunkMapInfo::encounters,
	"encounters",
	1,
	0
);
static TypedField<rpg::MapInfo, int32_t> static_encounter_steps(
	&rpg::MapInfo::encounter_steps,
	LMT_Reader::ChunkMapInfo::encounter_steps,
	"encounter_steps",
	0,
	0
);
static TypedField<rpg::MapInfo, rpg::Rect> static_area_rect(
	&rpg::MapInfo::area_rect,
	LMT_Reader::ChunkMapInfo::area_rect,
	"area_rect",
	1,
	0
);


template <>
Field<rpg::MapInfo> const* Struct<rpg::MapInfo>::fields[] = {
	&static_name,
	&static_parent_map,
	&static_indentation,
	&static_type,
	&static_scrollbar_x,
	&static_scrollbar_y,
	&static_expanded_node,
	&static_music_type,
	&static_music,
	&static_background_type,
	&static_background_name,
	&static_teleport,
	&static_escape,
	&static_save,
	&static_encounters,
	&static_encounter_steps,
	&static_area_rect,
	NULL
};

template class Struct<rpg::MapInfo>;

} //namespace lcf
