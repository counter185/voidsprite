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

// Read BattlerAnimationPose.

template <>
char const* const Struct<rpg::BattlerAnimationPose>::name = "BattlerAnimationPose";
static TypedField<rpg::BattlerAnimationPose, DBString> static_name(
	&rpg::BattlerAnimationPose::name,
	LDB_Reader::ChunkBattlerAnimationPose::name,
	"name",
	0,
	1
);
static TypedField<rpg::BattlerAnimationPose, DBString> static_battler_name(
	&rpg::BattlerAnimationPose::battler_name,
	LDB_Reader::ChunkBattlerAnimationPose::battler_name,
	"battler_name",
	0,
	1
);
static TypedField<rpg::BattlerAnimationPose, int32_t> static_battler_index(
	&rpg::BattlerAnimationPose::battler_index,
	LDB_Reader::ChunkBattlerAnimationPose::battler_index,
	"battler_index",
	0,
	1
);
static TypedField<rpg::BattlerAnimationPose, int32_t> static_animation_type(
	&rpg::BattlerAnimationPose::animation_type,
	LDB_Reader::ChunkBattlerAnimationPose::animation_type,
	"animation_type",
	0,
	1
);
static TypedField<rpg::BattlerAnimationPose, int32_t> static_battle_animation_id(
	&rpg::BattlerAnimationPose::battle_animation_id,
	LDB_Reader::ChunkBattlerAnimationPose::battle_animation_id,
	"battle_animation_id",
	0,
	1
);


template <>
Field<rpg::BattlerAnimationPose> const* Struct<rpg::BattlerAnimationPose>::fields[] = {
	&static_name,
	&static_battler_name,
	&static_battler_index,
	&static_animation_type,
	&static_battle_animation_id,
	NULL
};

template class Struct<rpg::BattlerAnimationPose>;

} //namespace lcf
