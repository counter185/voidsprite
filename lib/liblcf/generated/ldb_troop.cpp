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

// Read Troop.

template <>
char const* const Struct<rpg::Troop>::name = "Troop";
static TypedField<rpg::Troop, DBString> static_name(
	&rpg::Troop::name,
	LDB_Reader::ChunkTroop::name,
	"name",
	0,
	0
);
static TypedField<rpg::Troop, std::vector<rpg::TroopMember>> static_members(
	&rpg::Troop::members,
	LDB_Reader::ChunkTroop::members,
	"members",
	1,
	0
);
static TypedField<rpg::Troop, bool> static_auto_alignment(
	&rpg::Troop::auto_alignment,
	LDB_Reader::ChunkTroop::auto_alignment,
	"auto_alignment",
	0,
	1
);
static SizeField<rpg::Troop, DBBitArray> static_size_terrain_set(
	&rpg::Troop::terrain_set,
	LDB_Reader::ChunkTroop::terrain_set_size,
	0,
	0
);
static TypedField<rpg::Troop, DBBitArray> static_terrain_set(
	&rpg::Troop::terrain_set,
	LDB_Reader::ChunkTroop::terrain_set,
	"terrain_set",
	1,
	0
);
static TypedField<rpg::Troop, bool> static_appear_randomly(
	&rpg::Troop::appear_randomly,
	LDB_Reader::ChunkTroop::appear_randomly,
	"appear_randomly",
	0,
	1
);
static TypedField<rpg::Troop, std::vector<rpg::TroopPage>> static_pages(
	&rpg::Troop::pages,
	LDB_Reader::ChunkTroop::pages,
	"pages",
	1,
	0
);


template <>
Field<rpg::Troop> const* Struct<rpg::Troop>::fields[] = {
	&static_name,
	&static_members,
	&static_auto_alignment,
	&static_size_terrain_set,
	&static_terrain_set,
	&static_appear_randomly,
	&static_pages,
	NULL
};

template class Struct<rpg::Troop>;

} //namespace lcf
