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

// Read Terrain.

template <>
char const* const Struct<rpg::Terrain>::name = "Terrain";
static TypedField<rpg::Terrain, DBString> static_name(
	&rpg::Terrain::name,
	LDB_Reader::ChunkTerrain::name,
	"name",
	0,
	0
);
static TypedField<rpg::Terrain, int32_t> static_damage(
	&rpg::Terrain::damage,
	LDB_Reader::ChunkTerrain::damage,
	"damage",
	0,
	0
);
static TypedField<rpg::Terrain, int32_t> static_encounter_rate(
	&rpg::Terrain::encounter_rate,
	LDB_Reader::ChunkTerrain::encounter_rate,
	"encounter_rate",
	0,
	0
);
static TypedField<rpg::Terrain, DBString> static_background_name(
	&rpg::Terrain::background_name,
	LDB_Reader::ChunkTerrain::background_name,
	"background_name",
	0,
	0
);
static TypedField<rpg::Terrain, bool> static_boat_pass(
	&rpg::Terrain::boat_pass,
	LDB_Reader::ChunkTerrain::boat_pass,
	"boat_pass",
	0,
	0
);
static TypedField<rpg::Terrain, bool> static_ship_pass(
	&rpg::Terrain::ship_pass,
	LDB_Reader::ChunkTerrain::ship_pass,
	"ship_pass",
	0,
	0
);
static TypedField<rpg::Terrain, bool> static_airship_pass(
	&rpg::Terrain::airship_pass,
	LDB_Reader::ChunkTerrain::airship_pass,
	"airship_pass",
	0,
	0
);
static TypedField<rpg::Terrain, bool> static_airship_land(
	&rpg::Terrain::airship_land,
	LDB_Reader::ChunkTerrain::airship_land,
	"airship_land",
	0,
	0
);
static TypedField<rpg::Terrain, int32_t> static_bush_depth(
	&rpg::Terrain::bush_depth,
	LDB_Reader::ChunkTerrain::bush_depth,
	"bush_depth",
	1,
	0
);
static TypedField<rpg::Terrain, rpg::Sound> static_footstep(
	&rpg::Terrain::footstep,
	LDB_Reader::ChunkTerrain::footstep,
	"footstep",
	1,
	1
);
static TypedField<rpg::Terrain, bool> static_on_damage_se(
	&rpg::Terrain::on_damage_se,
	LDB_Reader::ChunkTerrain::on_damage_se,
	"on_damage_se",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_background_type(
	&rpg::Terrain::background_type,
	LDB_Reader::ChunkTerrain::background_type,
	"background_type",
	0,
	1
);
static TypedField<rpg::Terrain, DBString> static_background_a_name(
	&rpg::Terrain::background_a_name,
	LDB_Reader::ChunkTerrain::background_a_name,
	"background_a_name",
	0,
	1
);
static TypedField<rpg::Terrain, bool> static_background_a_scrollh(
	&rpg::Terrain::background_a_scrollh,
	LDB_Reader::ChunkTerrain::background_a_scrollh,
	"background_a_scrollh",
	0,
	1
);
static TypedField<rpg::Terrain, bool> static_background_a_scrollv(
	&rpg::Terrain::background_a_scrollv,
	LDB_Reader::ChunkTerrain::background_a_scrollv,
	"background_a_scrollv",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_background_a_scrollh_speed(
	&rpg::Terrain::background_a_scrollh_speed,
	LDB_Reader::ChunkTerrain::background_a_scrollh_speed,
	"background_a_scrollh_speed",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_background_a_scrollv_speed(
	&rpg::Terrain::background_a_scrollv_speed,
	LDB_Reader::ChunkTerrain::background_a_scrollv_speed,
	"background_a_scrollv_speed",
	0,
	1
);
static TypedField<rpg::Terrain, bool> static_background_b(
	&rpg::Terrain::background_b,
	LDB_Reader::ChunkTerrain::background_b,
	"background_b",
	0,
	1
);
static TypedField<rpg::Terrain, DBString> static_background_b_name(
	&rpg::Terrain::background_b_name,
	LDB_Reader::ChunkTerrain::background_b_name,
	"background_b_name",
	0,
	1
);
static TypedField<rpg::Terrain, bool> static_background_b_scrollh(
	&rpg::Terrain::background_b_scrollh,
	LDB_Reader::ChunkTerrain::background_b_scrollh,
	"background_b_scrollh",
	0,
	1
);
static TypedField<rpg::Terrain, bool> static_background_b_scrollv(
	&rpg::Terrain::background_b_scrollv,
	LDB_Reader::ChunkTerrain::background_b_scrollv,
	"background_b_scrollv",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_background_b_scrollh_speed(
	&rpg::Terrain::background_b_scrollh_speed,
	LDB_Reader::ChunkTerrain::background_b_scrollh_speed,
	"background_b_scrollh_speed",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_background_b_scrollv_speed(
	&rpg::Terrain::background_b_scrollv_speed,
	LDB_Reader::ChunkTerrain::background_b_scrollv_speed,
	"background_b_scrollv_speed",
	0,
	1
);
static TypedField<rpg::Terrain, rpg::Terrain::Flags> static_special_flags(
	&rpg::Terrain::special_flags,
	LDB_Reader::ChunkTerrain::special_flags,
	"special_flags",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_special_back_party(
	&rpg::Terrain::special_back_party,
	LDB_Reader::ChunkTerrain::special_back_party,
	"special_back_party",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_special_back_enemies(
	&rpg::Terrain::special_back_enemies,
	LDB_Reader::ChunkTerrain::special_back_enemies,
	"special_back_enemies",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_special_lateral_party(
	&rpg::Terrain::special_lateral_party,
	LDB_Reader::ChunkTerrain::special_lateral_party,
	"special_lateral_party",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_special_lateral_enemies(
	&rpg::Terrain::special_lateral_enemies,
	LDB_Reader::ChunkTerrain::special_lateral_enemies,
	"special_lateral_enemies",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_grid_location(
	&rpg::Terrain::grid_location,
	LDB_Reader::ChunkTerrain::grid_location,
	"grid_location",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_grid_top_y(
	&rpg::Terrain::grid_top_y,
	LDB_Reader::ChunkTerrain::grid_top_y,
	"grid_top_y",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_grid_elongation(
	&rpg::Terrain::grid_elongation,
	LDB_Reader::ChunkTerrain::grid_elongation,
	"grid_elongation",
	0,
	1
);
static TypedField<rpg::Terrain, int32_t> static_grid_inclination(
	&rpg::Terrain::grid_inclination,
	LDB_Reader::ChunkTerrain::grid_inclination,
	"grid_inclination",
	0,
	1
);
static TypedField<rpg::Terrain, bool> static_easyrpg_damage_in_percent(
	&rpg::Terrain::easyrpg_damage_in_percent,
	LDB_Reader::ChunkTerrain::easyrpg_damage_in_percent,
	"easyrpg_damage_in_percent",
	0,
	0
);
static TypedField<rpg::Terrain, bool> static_easyrpg_damage_can_kill(
	&rpg::Terrain::easyrpg_damage_can_kill,
	LDB_Reader::ChunkTerrain::easyrpg_damage_can_kill,
	"easyrpg_damage_can_kill",
	0,
	0
);


template <>
Field<rpg::Terrain> const* Struct<rpg::Terrain>::fields[] = {
	&static_name,
	&static_damage,
	&static_encounter_rate,
	&static_background_name,
	&static_boat_pass,
	&static_ship_pass,
	&static_airship_pass,
	&static_airship_land,
	&static_bush_depth,
	&static_footstep,
	&static_on_damage_se,
	&static_background_type,
	&static_background_a_name,
	&static_background_a_scrollh,
	&static_background_a_scrollv,
	&static_background_a_scrollh_speed,
	&static_background_a_scrollv_speed,
	&static_background_b,
	&static_background_b_name,
	&static_background_b_scrollh,
	&static_background_b_scrollv,
	&static_background_b_scrollh_speed,
	&static_background_b_scrollv_speed,
	&static_special_flags,
	&static_special_back_party,
	&static_special_back_enemies,
	&static_special_lateral_party,
	&static_special_lateral_enemies,
	&static_grid_location,
	&static_grid_top_y,
	&static_grid_elongation,
	&static_grid_inclination,
	&static_easyrpg_damage_in_percent,
	&static_easyrpg_damage_can_kill,
	NULL
};

template class Struct<rpg::Terrain>;

} //namespace lcf
