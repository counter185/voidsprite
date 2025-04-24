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

// Read BattlerAnimationItemSkill.

template <>
char const* const Struct<rpg::BattlerAnimationItemSkill>::name = "BattlerAnimationItemSkill";
static TypedField<rpg::BattlerAnimationItemSkill, int32_t> static_unknown02(
	&rpg::BattlerAnimationItemSkill::unknown02,
	LDB_Reader::ChunkBattlerAnimationItemSkill::unknown02,
	"unknown02",
	0,
	1
);
static TypedField<rpg::BattlerAnimationItemSkill, int32_t> static_type(
	&rpg::BattlerAnimationItemSkill::type,
	LDB_Reader::ChunkBattlerAnimationItemSkill::type,
	"type",
	0,
	1
);
static TypedField<rpg::BattlerAnimationItemSkill, int32_t> static_weapon_animation_id(
	&rpg::BattlerAnimationItemSkill::weapon_animation_id,
	LDB_Reader::ChunkBattlerAnimationItemSkill::weapon_animation_id,
	"weapon_animation_id",
	0,
	1
);
static TypedField<rpg::BattlerAnimationItemSkill, int32_t> static_movement(
	&rpg::BattlerAnimationItemSkill::movement,
	LDB_Reader::ChunkBattlerAnimationItemSkill::movement,
	"movement",
	0,
	1
);
static TypedField<rpg::BattlerAnimationItemSkill, int32_t> static_after_image(
	&rpg::BattlerAnimationItemSkill::after_image,
	LDB_Reader::ChunkBattlerAnimationItemSkill::after_image,
	"after_image",
	0,
	1
);
static TypedField<rpg::BattlerAnimationItemSkill, int32_t> static_attacks(
	&rpg::BattlerAnimationItemSkill::attacks,
	LDB_Reader::ChunkBattlerAnimationItemSkill::attacks,
	"attacks",
	0,
	1
);
static TypedField<rpg::BattlerAnimationItemSkill, bool> static_ranged(
	&rpg::BattlerAnimationItemSkill::ranged,
	LDB_Reader::ChunkBattlerAnimationItemSkill::ranged,
	"ranged",
	0,
	1
);
static TypedField<rpg::BattlerAnimationItemSkill, int32_t> static_ranged_animation_id(
	&rpg::BattlerAnimationItemSkill::ranged_animation_id,
	LDB_Reader::ChunkBattlerAnimationItemSkill::ranged_animation_id,
	"ranged_animation_id",
	0,
	1
);
static TypedField<rpg::BattlerAnimationItemSkill, int32_t> static_ranged_speed(
	&rpg::BattlerAnimationItemSkill::ranged_speed,
	LDB_Reader::ChunkBattlerAnimationItemSkill::ranged_speed,
	"ranged_speed",
	0,
	1
);
static TypedField<rpg::BattlerAnimationItemSkill, int32_t> static_battle_animation_id(
	&rpg::BattlerAnimationItemSkill::battle_animation_id,
	LDB_Reader::ChunkBattlerAnimationItemSkill::battle_animation_id,
	"battle_animation_id",
	0,
	1
);
static TypedField<rpg::BattlerAnimationItemSkill, int32_t> static_pose(
	&rpg::BattlerAnimationItemSkill::pose,
	LDB_Reader::ChunkBattlerAnimationItemSkill::pose,
	"pose",
	0,
	1
);


template <>
Field<rpg::BattlerAnimationItemSkill> const* Struct<rpg::BattlerAnimationItemSkill>::fields[] = {
	&static_unknown02,
	&static_type,
	&static_weapon_animation_id,
	&static_movement,
	&static_after_image,
	&static_attacks,
	&static_ranged,
	&static_ranged_animation_id,
	&static_ranged_speed,
	&static_battle_animation_id,
	&static_pose,
	NULL
};

template class Struct<rpg::BattlerAnimationItemSkill>;

} //namespace lcf
