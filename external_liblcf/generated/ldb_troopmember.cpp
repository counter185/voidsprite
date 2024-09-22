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

// Read TroopMember.

template <>
char const* const Struct<rpg::TroopMember>::name = "TroopMember";
static TypedField<rpg::TroopMember, int32_t> static_enemy_id(
	&rpg::TroopMember::enemy_id,
	LDB_Reader::ChunkTroopMember::enemy_id,
	"enemy_id",
	0,
	0
);
static TypedField<rpg::TroopMember, int32_t> static_x(
	&rpg::TroopMember::x,
	LDB_Reader::ChunkTroopMember::x,
	"x",
	0,
	0
);
static TypedField<rpg::TroopMember, int32_t> static_y(
	&rpg::TroopMember::y,
	LDB_Reader::ChunkTroopMember::y,
	"y",
	0,
	0
);
static TypedField<rpg::TroopMember, bool> static_invisible(
	&rpg::TroopMember::invisible,
	LDB_Reader::ChunkTroopMember::invisible,
	"invisible",
	0,
	0
);


template <>
Field<rpg::TroopMember> const* Struct<rpg::TroopMember>::fields[] = {
	&static_enemy_id,
	&static_x,
	&static_y,
	&static_invisible,
	NULL
};

template class Struct<rpg::TroopMember>;

} //namespace lcf
