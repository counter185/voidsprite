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

// Read BattlerAnimationWeapon.

template <>
char const* const Struct<rpg::BattlerAnimationWeapon>::name = "BattlerAnimationWeapon";
static TypedField<rpg::BattlerAnimationWeapon, DBString> static_name(
	&rpg::BattlerAnimationWeapon::name,
	LDB_Reader::ChunkBattlerAnimationWeapon::name,
	"name",
	0,
	1
);
static TypedField<rpg::BattlerAnimationWeapon, DBString> static_weapon_name(
	&rpg::BattlerAnimationWeapon::weapon_name,
	LDB_Reader::ChunkBattlerAnimationWeapon::weapon_name,
	"weapon_name",
	0,
	1
);
static TypedField<rpg::BattlerAnimationWeapon, int32_t> static_weapon_index(
	&rpg::BattlerAnimationWeapon::weapon_index,
	LDB_Reader::ChunkBattlerAnimationWeapon::weapon_index,
	"weapon_index",
	0,
	1
);


template <>
Field<rpg::BattlerAnimationWeapon> const* Struct<rpg::BattlerAnimationWeapon>::fields[] = {
	&static_name,
	&static_weapon_name,
	&static_weapon_index,
	NULL
};

template class Struct<rpg::BattlerAnimationWeapon>;

} //namespace lcf
