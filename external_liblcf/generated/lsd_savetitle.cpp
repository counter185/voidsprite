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

// Read SaveTitle.

template <>
char const* const Struct<rpg::SaveTitle>::name = "SaveTitle";
static TypedField<rpg::SaveTitle, double> static_timestamp(
	&rpg::SaveTitle::timestamp,
	LSD_Reader::ChunkSaveTitle::timestamp,
	"timestamp",
	0,
	0
);
static TypedField<rpg::SaveTitle, std::string> static_hero_name(
	&rpg::SaveTitle::hero_name,
	LSD_Reader::ChunkSaveTitle::hero_name,
	"hero_name",
	0,
	0
);
static TypedField<rpg::SaveTitle, int32_t> static_hero_level(
	&rpg::SaveTitle::hero_level,
	LSD_Reader::ChunkSaveTitle::hero_level,
	"hero_level",
	0,
	0
);
static TypedField<rpg::SaveTitle, int32_t> static_hero_hp(
	&rpg::SaveTitle::hero_hp,
	LSD_Reader::ChunkSaveTitle::hero_hp,
	"hero_hp",
	0,
	0
);
static TypedField<rpg::SaveTitle, std::string> static_face1_name(
	&rpg::SaveTitle::face1_name,
	LSD_Reader::ChunkSaveTitle::face1_name,
	"face1_name",
	0,
	0
);
static TypedField<rpg::SaveTitle, int32_t> static_face1_id(
	&rpg::SaveTitle::face1_id,
	LSD_Reader::ChunkSaveTitle::face1_id,
	"face1_id",
	0,
	0
);
static TypedField<rpg::SaveTitle, std::string> static_face2_name(
	&rpg::SaveTitle::face2_name,
	LSD_Reader::ChunkSaveTitle::face2_name,
	"face2_name",
	0,
	0
);
static TypedField<rpg::SaveTitle, int32_t> static_face2_id(
	&rpg::SaveTitle::face2_id,
	LSD_Reader::ChunkSaveTitle::face2_id,
	"face2_id",
	0,
	0
);
static TypedField<rpg::SaveTitle, std::string> static_face3_name(
	&rpg::SaveTitle::face3_name,
	LSD_Reader::ChunkSaveTitle::face3_name,
	"face3_name",
	0,
	0
);
static TypedField<rpg::SaveTitle, int32_t> static_face3_id(
	&rpg::SaveTitle::face3_id,
	LSD_Reader::ChunkSaveTitle::face3_id,
	"face3_id",
	0,
	0
);
static TypedField<rpg::SaveTitle, std::string> static_face4_name(
	&rpg::SaveTitle::face4_name,
	LSD_Reader::ChunkSaveTitle::face4_name,
	"face4_name",
	0,
	0
);
static TypedField<rpg::SaveTitle, int32_t> static_face4_id(
	&rpg::SaveTitle::face4_id,
	LSD_Reader::ChunkSaveTitle::face4_id,
	"face4_id",
	0,
	0
);


template <>
Field<rpg::SaveTitle> const* Struct<rpg::SaveTitle>::fields[] = {
	&static_timestamp,
	&static_hero_name,
	&static_hero_level,
	&static_hero_hp,
	&static_face1_name,
	&static_face1_id,
	&static_face2_name,
	&static_face2_id,
	&static_face3_name,
	&static_face3_id,
	&static_face4_name,
	&static_face4_id,
	NULL
};

template class Struct<rpg::SaveTitle>;

} //namespace lcf
