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

// Read TestBattler.

template <>
char const* const Struct<rpg::TestBattler>::name = "TestBattler";
static TypedField<rpg::TestBattler, int32_t> static_actor_id(
	&rpg::TestBattler::actor_id,
	LDB_Reader::ChunkTestBattler::actor_id,
	"actor_id",
	0,
	0
);
static TypedField<rpg::TestBattler, int32_t> static_level(
	&rpg::TestBattler::level,
	LDB_Reader::ChunkTestBattler::level,
	"level",
	0,
	0
);
static TypedField<rpg::TestBattler, int32_t> static_weapon_id(
	&rpg::TestBattler::weapon_id,
	LDB_Reader::ChunkTestBattler::weapon_id,
	"weapon_id",
	0,
	0
);
static TypedField<rpg::TestBattler, int32_t> static_shield_id(
	&rpg::TestBattler::shield_id,
	LDB_Reader::ChunkTestBattler::shield_id,
	"shield_id",
	0,
	0
);
static TypedField<rpg::TestBattler, int32_t> static_armor_id(
	&rpg::TestBattler::armor_id,
	LDB_Reader::ChunkTestBattler::armor_id,
	"armor_id",
	0,
	0
);
static TypedField<rpg::TestBattler, int32_t> static_helmet_id(
	&rpg::TestBattler::helmet_id,
	LDB_Reader::ChunkTestBattler::helmet_id,
	"helmet_id",
	0,
	0
);
static TypedField<rpg::TestBattler, int32_t> static_accessory_id(
	&rpg::TestBattler::accessory_id,
	LDB_Reader::ChunkTestBattler::accessory_id,
	"accessory_id",
	0,
	0
);


template <>
Field<rpg::TestBattler> const* Struct<rpg::TestBattler>::fields[] = {
	&static_actor_id,
	&static_level,
	&static_weapon_id,
	&static_shield_id,
	&static_armor_id,
	&static_helmet_id,
	&static_accessory_id,
	NULL
};

template class Struct<rpg::TestBattler>;

} //namespace lcf
