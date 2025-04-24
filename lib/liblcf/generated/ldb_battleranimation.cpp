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

// Read BattlerAnimation.

template <>
char const* const Struct<rpg::BattlerAnimation>::name = "BattlerAnimation";
static TypedField<rpg::BattlerAnimation, DBString> static_name(
	&rpg::BattlerAnimation::name,
	LDB_Reader::ChunkBattlerAnimation::name,
	"name",
	0,
	1
);
static TypedField<rpg::BattlerAnimation, int32_t> static_speed(
	&rpg::BattlerAnimation::speed,
	LDB_Reader::ChunkBattlerAnimation::speed,
	"speed",
	0,
	1
);
static TypedField<rpg::BattlerAnimation, std::vector<rpg::BattlerAnimationPose>> static_poses(
	&rpg::BattlerAnimation::poses,
	LDB_Reader::ChunkBattlerAnimation::poses,
	"poses",
	1,
	1
);
static TypedField<rpg::BattlerAnimation, std::vector<rpg::BattlerAnimationWeapon>> static_weapons(
	&rpg::BattlerAnimation::weapons,
	LDB_Reader::ChunkBattlerAnimation::weapons,
	"weapons",
	1,
	1
);


template <>
Field<rpg::BattlerAnimation> const* Struct<rpg::BattlerAnimation>::fields[] = {
	&static_name,
	&static_speed,
	&static_poses,
	&static_weapons,
	NULL
};

template class Struct<rpg::BattlerAnimation>;

} //namespace lcf
