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

// Read Class.

template <>
char const* const Struct<rpg::Class>::name = "Class";
static TypedField<rpg::Class, DBString> static_name(
	&rpg::Class::name,
	LDB_Reader::ChunkClass::name,
	"name",
	0,
	0
);
static TypedField<rpg::Class, bool> static_two_weapon(
	&rpg::Class::two_weapon,
	LDB_Reader::ChunkClass::two_weapon,
	"two_weapon",
	0,
	0
);
static TypedField<rpg::Class, bool> static_lock_equipment(
	&rpg::Class::lock_equipment,
	LDB_Reader::ChunkClass::lock_equipment,
	"lock_equipment",
	0,
	0
);
static TypedField<rpg::Class, bool> static_auto_battle(
	&rpg::Class::auto_battle,
	LDB_Reader::ChunkClass::auto_battle,
	"auto_battle",
	0,
	0
);
static TypedField<rpg::Class, bool> static_super_guard(
	&rpg::Class::super_guard,
	LDB_Reader::ChunkClass::super_guard,
	"super_guard",
	0,
	0
);
static TypedField<rpg::Class, rpg::Parameters> static_parameters(
	&rpg::Class::parameters,
	LDB_Reader::ChunkClass::parameters,
	"parameters",
	0,
	0
);
static TypedField<rpg::Class, int32_t> static_exp_base(
	&rpg::Class::exp_base,
	LDB_Reader::ChunkClass::exp_base,
	"exp_base",
	0,
	0
);
static TypedField<rpg::Class, int32_t> static_exp_inflation(
	&rpg::Class::exp_inflation,
	LDB_Reader::ChunkClass::exp_inflation,
	"exp_inflation",
	0,
	0
);
static TypedField<rpg::Class, int32_t> static_exp_correction(
	&rpg::Class::exp_correction,
	LDB_Reader::ChunkClass::exp_correction,
	"exp_correction",
	0,
	0
);
static TypedField<rpg::Class, int32_t> static_battler_animation(
	&rpg::Class::battler_animation,
	LDB_Reader::ChunkClass::battler_animation,
	"battler_animation",
	0,
	0
);
static TypedField<rpg::Class, std::vector<rpg::Learning>> static_skills(
	&rpg::Class::skills,
	LDB_Reader::ChunkClass::skills,
	"skills",
	1,
	0
);
static SizeField<rpg::Class, std::vector<uint8_t>> static_size_state_ranks(
	&rpg::Class::state_ranks,
	LDB_Reader::ChunkClass::state_ranks_size,
	0,
	0
);
static TypedField<rpg::Class, std::vector<uint8_t>> static_state_ranks(
	&rpg::Class::state_ranks,
	LDB_Reader::ChunkClass::state_ranks,
	"state_ranks",
	1,
	0
);
static SizeField<rpg::Class, std::vector<uint8_t>> static_size_attribute_ranks(
	&rpg::Class::attribute_ranks,
	LDB_Reader::ChunkClass::attribute_ranks_size,
	0,
	0
);
static TypedField<rpg::Class, std::vector<uint8_t>> static_attribute_ranks(
	&rpg::Class::attribute_ranks,
	LDB_Reader::ChunkClass::attribute_ranks,
	"attribute_ranks",
	1,
	0
);
static TypedField<rpg::Class, std::vector<int32_t>> static_battle_commands(
	&rpg::Class::battle_commands,
	LDB_Reader::ChunkClass::battle_commands,
	"battle_commands",
	1,
	0
);


template <>
Field<rpg::Class> const* Struct<rpg::Class>::fields[] = {
	&static_name,
	&static_two_weapon,
	&static_lock_equipment,
	&static_auto_battle,
	&static_super_guard,
	&static_parameters,
	&static_exp_base,
	&static_exp_inflation,
	&static_exp_correction,
	&static_battler_animation,
	&static_skills,
	&static_size_state_ranks,
	&static_state_ranks,
	&static_size_attribute_ranks,
	&static_attribute_ranks,
	&static_battle_commands,
	NULL
};

template class Struct<rpg::Class>;

} //namespace lcf
