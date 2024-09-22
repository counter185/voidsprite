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

// Read Start.

template <>
char const* const Struct<rpg::Start>::name = "Start";
static TypedField<rpg::Start, int32_t> static_party_map_id(
	&rpg::Start::party_map_id,
	LMT_Reader::ChunkStart::party_map_id,
	"party_map_id",
	0,
	0
);
static TypedField<rpg::Start, int32_t> static_party_x(
	&rpg::Start::party_x,
	LMT_Reader::ChunkStart::party_x,
	"party_x",
	0,
	0
);
static TypedField<rpg::Start, int32_t> static_party_y(
	&rpg::Start::party_y,
	LMT_Reader::ChunkStart::party_y,
	"party_y",
	0,
	0
);
static TypedField<rpg::Start, int32_t> static_boat_map_id(
	&rpg::Start::boat_map_id,
	LMT_Reader::ChunkStart::boat_map_id,
	"boat_map_id",
	0,
	0
);
static TypedField<rpg::Start, int32_t> static_boat_x(
	&rpg::Start::boat_x,
	LMT_Reader::ChunkStart::boat_x,
	"boat_x",
	0,
	0
);
static TypedField<rpg::Start, int32_t> static_boat_y(
	&rpg::Start::boat_y,
	LMT_Reader::ChunkStart::boat_y,
	"boat_y",
	0,
	0
);
static TypedField<rpg::Start, int32_t> static_ship_map_id(
	&rpg::Start::ship_map_id,
	LMT_Reader::ChunkStart::ship_map_id,
	"ship_map_id",
	0,
	0
);
static TypedField<rpg::Start, int32_t> static_ship_x(
	&rpg::Start::ship_x,
	LMT_Reader::ChunkStart::ship_x,
	"ship_x",
	0,
	0
);
static TypedField<rpg::Start, int32_t> static_ship_y(
	&rpg::Start::ship_y,
	LMT_Reader::ChunkStart::ship_y,
	"ship_y",
	0,
	0
);
static TypedField<rpg::Start, int32_t> static_airship_map_id(
	&rpg::Start::airship_map_id,
	LMT_Reader::ChunkStart::airship_map_id,
	"airship_map_id",
	0,
	0
);
static TypedField<rpg::Start, int32_t> static_airship_x(
	&rpg::Start::airship_x,
	LMT_Reader::ChunkStart::airship_x,
	"airship_x",
	0,
	0
);
static TypedField<rpg::Start, int32_t> static_airship_y(
	&rpg::Start::airship_y,
	LMT_Reader::ChunkStart::airship_y,
	"airship_y",
	0,
	0
);


template <>
Field<rpg::Start> const* Struct<rpg::Start>::fields[] = {
	&static_party_map_id,
	&static_party_x,
	&static_party_y,
	&static_boat_map_id,
	&static_boat_x,
	&static_boat_y,
	&static_ship_map_id,
	&static_ship_x,
	&static_ship_y,
	&static_airship_map_id,
	&static_airship_x,
	&static_airship_y,
	NULL
};

template class Struct<rpg::Start>;

} //namespace lcf
