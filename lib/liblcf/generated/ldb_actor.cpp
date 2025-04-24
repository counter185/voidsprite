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

// Read Actor.

template <>
char const* const Struct<rpg::Actor>::name = "Actor";
static TypedField<rpg::Actor, DBString> static_name(
	&rpg::Actor::name,
	LDB_Reader::ChunkActor::name,
	"name",
	0,
	0
);
static TypedField<rpg::Actor, DBString> static_title(
	&rpg::Actor::title,
	LDB_Reader::ChunkActor::title,
	"title",
	0,
	0
);
static TypedField<rpg::Actor, DBString> static_character_name(
	&rpg::Actor::character_name,
	LDB_Reader::ChunkActor::character_name,
	"character_name",
	0,
	0
);
static TypedField<rpg::Actor, int32_t> static_character_index(
	&rpg::Actor::character_index,
	LDB_Reader::ChunkActor::character_index,
	"character_index",
	0,
	0
);
static TypedField<rpg::Actor, bool> static_transparent(
	&rpg::Actor::transparent,
	LDB_Reader::ChunkActor::transparent,
	"transparent",
	0,
	0
);
static TypedField<rpg::Actor, int32_t> static_initial_level(
	&rpg::Actor::initial_level,
	LDB_Reader::ChunkActor::initial_level,
	"initial_level",
	0,
	0
);
static TypedField<rpg::Actor, int32_t> static_final_level(
	&rpg::Actor::final_level,
	LDB_Reader::ChunkActor::final_level,
	"final_level",
	0,
	0
);
static TypedField<rpg::Actor, bool> static_critical_hit(
	&rpg::Actor::critical_hit,
	LDB_Reader::ChunkActor::critical_hit,
	"critical_hit",
	0,
	0
);
static TypedField<rpg::Actor, int32_t> static_critical_hit_chance(
	&rpg::Actor::critical_hit_chance,
	LDB_Reader::ChunkActor::critical_hit_chance,
	"critical_hit_chance",
	0,
	0
);
static TypedField<rpg::Actor, DBString> static_face_name(
	&rpg::Actor::face_name,
	LDB_Reader::ChunkActor::face_name,
	"face_name",
	0,
	0
);
static TypedField<rpg::Actor, int32_t> static_face_index(
	&rpg::Actor::face_index,
	LDB_Reader::ChunkActor::face_index,
	"face_index",
	0,
	0
);
static TypedField<rpg::Actor, bool> static_two_weapon(
	&rpg::Actor::two_weapon,
	LDB_Reader::ChunkActor::two_weapon,
	"two_weapon",
	0,
	0
);
static TypedField<rpg::Actor, bool> static_lock_equipment(
	&rpg::Actor::lock_equipment,
	LDB_Reader::ChunkActor::lock_equipment,
	"lock_equipment",
	0,
	0
);
static TypedField<rpg::Actor, bool> static_auto_battle(
	&rpg::Actor::auto_battle,
	LDB_Reader::ChunkActor::auto_battle,
	"auto_battle",
	0,
	0
);
static TypedField<rpg::Actor, bool> static_super_guard(
	&rpg::Actor::super_guard,
	LDB_Reader::ChunkActor::super_guard,
	"super_guard",
	0,
	0
);
static TypedField<rpg::Actor, rpg::Parameters> static_parameters(
	&rpg::Actor::parameters,
	LDB_Reader::ChunkActor::parameters,
	"parameters",
	1,
	0
);
static TypedField<rpg::Actor, int32_t> static_exp_base(
	&rpg::Actor::exp_base,
	LDB_Reader::ChunkActor::exp_base,
	"exp_base",
	0,
	0
);
static TypedField<rpg::Actor, int32_t> static_exp_inflation(
	&rpg::Actor::exp_inflation,
	LDB_Reader::ChunkActor::exp_inflation,
	"exp_inflation",
	0,
	0
);
static TypedField<rpg::Actor, int32_t> static_exp_correction(
	&rpg::Actor::exp_correction,
	LDB_Reader::ChunkActor::exp_correction,
	"exp_correction",
	0,
	0
);
static TypedField<rpg::Actor, rpg::Equipment> static_initial_equipment(
	&rpg::Actor::initial_equipment,
	LDB_Reader::ChunkActor::initial_equipment,
	"initial_equipment",
	1,
	0
);
static TypedField<rpg::Actor, int32_t> static_unarmed_animation(
	&rpg::Actor::unarmed_animation,
	LDB_Reader::ChunkActor::unarmed_animation,
	"unarmed_animation",
	0,
	0
);
static TypedField<rpg::Actor, int32_t> static_class_id(
	&rpg::Actor::class_id,
	LDB_Reader::ChunkActor::class_id,
	"class_id",
	0,
	1
);
static TypedField<rpg::Actor, int32_t> static_battle_x(
	&rpg::Actor::battle_x,
	LDB_Reader::ChunkActor::battle_x,
	"battle_x",
	0,
	1
);
static TypedField<rpg::Actor, int32_t> static_battle_y(
	&rpg::Actor::battle_y,
	LDB_Reader::ChunkActor::battle_y,
	"battle_y",
	0,
	1
);
static TypedField<rpg::Actor, int32_t> static_battler_animation(
	&rpg::Actor::battler_animation,
	LDB_Reader::ChunkActor::battler_animation,
	"battler_animation",
	0,
	1
);
static TypedField<rpg::Actor, std::vector<rpg::Learning>> static_skills(
	&rpg::Actor::skills,
	LDB_Reader::ChunkActor::skills,
	"skills",
	1,
	0
);
static TypedField<rpg::Actor, bool> static_rename_skill(
	&rpg::Actor::rename_skill,
	LDB_Reader::ChunkActor::rename_skill,
	"rename_skill",
	0,
	0
);
static TypedField<rpg::Actor, DBString> static_skill_name(
	&rpg::Actor::skill_name,
	LDB_Reader::ChunkActor::skill_name,
	"skill_name",
	0,
	0
);
static SizeField<rpg::Actor, std::vector<uint8_t>> static_size_state_ranks(
	&rpg::Actor::state_ranks,
	LDB_Reader::ChunkActor::state_ranks_size,
	0,
	0
);
static TypedField<rpg::Actor, std::vector<uint8_t>> static_state_ranks(
	&rpg::Actor::state_ranks,
	LDB_Reader::ChunkActor::state_ranks,
	"state_ranks",
	1,
	0
);
static SizeField<rpg::Actor, std::vector<uint8_t>> static_size_attribute_ranks(
	&rpg::Actor::attribute_ranks,
	LDB_Reader::ChunkActor::attribute_ranks_size,
	0,
	0
);
static TypedField<rpg::Actor, std::vector<uint8_t>> static_attribute_ranks(
	&rpg::Actor::attribute_ranks,
	LDB_Reader::ChunkActor::attribute_ranks,
	"attribute_ranks",
	1,
	0
);
static TypedField<rpg::Actor, std::vector<int32_t>> static_battle_commands(
	&rpg::Actor::battle_commands,
	LDB_Reader::ChunkActor::battle_commands,
	"battle_commands",
	1,
	1
);
static TypedField<rpg::Actor, int32_t> static_easyrpg_actorai(
	&rpg::Actor::easyrpg_actorai,
	LDB_Reader::ChunkActor::easyrpg_actorai,
	"easyrpg_actorai",
	0,
	0
);
static TypedField<rpg::Actor, bool> static_easyrpg_prevent_critical(
	&rpg::Actor::easyrpg_prevent_critical,
	LDB_Reader::ChunkActor::easyrpg_prevent_critical,
	"easyrpg_prevent_critical",
	0,
	0
);
static TypedField<rpg::Actor, bool> static_easyrpg_raise_evasion(
	&rpg::Actor::easyrpg_raise_evasion,
	LDB_Reader::ChunkActor::easyrpg_raise_evasion,
	"easyrpg_raise_evasion",
	0,
	0
);
static TypedField<rpg::Actor, bool> static_easyrpg_immune_to_attribute_downshifts(
	&rpg::Actor::easyrpg_immune_to_attribute_downshifts,
	LDB_Reader::ChunkActor::easyrpg_immune_to_attribute_downshifts,
	"easyrpg_immune_to_attribute_downshifts",
	0,
	0
);
static TypedField<rpg::Actor, bool> static_easyrpg_ignore_evasion(
	&rpg::Actor::easyrpg_ignore_evasion,
	LDB_Reader::ChunkActor::easyrpg_ignore_evasion,
	"easyrpg_ignore_evasion",
	0,
	0
);
static TypedField<rpg::Actor, int32_t> static_easyrpg_unarmed_hit(
	&rpg::Actor::easyrpg_unarmed_hit,
	LDB_Reader::ChunkActor::easyrpg_unarmed_hit,
	"easyrpg_unarmed_hit",
	0,
	0
);
static SizeField<rpg::Actor, DBBitArray> static_size_easyrpg_unarmed_state_set(
	&rpg::Actor::easyrpg_unarmed_state_set,
	LDB_Reader::ChunkActor::easyrpg_unarmed_state_set_size,
	0,
	0
);
static TypedField<rpg::Actor, DBBitArray> static_easyrpg_unarmed_state_set(
	&rpg::Actor::easyrpg_unarmed_state_set,
	LDB_Reader::ChunkActor::easyrpg_unarmed_state_set,
	"easyrpg_unarmed_state_set",
	1,
	0
);
static TypedField<rpg::Actor, int32_t> static_easyrpg_unarmed_state_chance(
	&rpg::Actor::easyrpg_unarmed_state_chance,
	LDB_Reader::ChunkActor::easyrpg_unarmed_state_chance,
	"easyrpg_unarmed_state_chance",
	0,
	0
);
static SizeField<rpg::Actor, DBBitArray> static_size_easyrpg_unarmed_attribute_set(
	&rpg::Actor::easyrpg_unarmed_attribute_set,
	LDB_Reader::ChunkActor::easyrpg_unarmed_attribute_set_size,
	0,
	0
);
static TypedField<rpg::Actor, DBBitArray> static_easyrpg_unarmed_attribute_set(
	&rpg::Actor::easyrpg_unarmed_attribute_set,
	LDB_Reader::ChunkActor::easyrpg_unarmed_attribute_set,
	"easyrpg_unarmed_attribute_set",
	1,
	0
);
static TypedField<rpg::Actor, bool> static_easyrpg_dual_attack(
	&rpg::Actor::easyrpg_dual_attack,
	LDB_Reader::ChunkActor::easyrpg_dual_attack,
	"easyrpg_dual_attack",
	0,
	0
);
static TypedField<rpg::Actor, bool> static_easyrpg_attack_all(
	&rpg::Actor::easyrpg_attack_all,
	LDB_Reader::ChunkActor::easyrpg_attack_all,
	"easyrpg_attack_all",
	0,
	0
);


template <>
Field<rpg::Actor> const* Struct<rpg::Actor>::fields[] = {
	&static_name,
	&static_title,
	&static_character_name,
	&static_character_index,
	&static_transparent,
	&static_initial_level,
	&static_final_level,
	&static_critical_hit,
	&static_critical_hit_chance,
	&static_face_name,
	&static_face_index,
	&static_two_weapon,
	&static_lock_equipment,
	&static_auto_battle,
	&static_super_guard,
	&static_parameters,
	&static_exp_base,
	&static_exp_inflation,
	&static_exp_correction,
	&static_initial_equipment,
	&static_unarmed_animation,
	&static_class_id,
	&static_battle_x,
	&static_battle_y,
	&static_battler_animation,
	&static_skills,
	&static_rename_skill,
	&static_skill_name,
	&static_size_state_ranks,
	&static_state_ranks,
	&static_size_attribute_ranks,
	&static_attribute_ranks,
	&static_battle_commands,
	&static_easyrpg_actorai,
	&static_easyrpg_prevent_critical,
	&static_easyrpg_raise_evasion,
	&static_easyrpg_immune_to_attribute_downshifts,
	&static_easyrpg_ignore_evasion,
	&static_easyrpg_unarmed_hit,
	&static_size_easyrpg_unarmed_state_set,
	&static_easyrpg_unarmed_state_set,
	&static_easyrpg_unarmed_state_chance,
	&static_size_easyrpg_unarmed_attribute_set,
	&static_easyrpg_unarmed_attribute_set,
	&static_easyrpg_dual_attack,
	&static_easyrpg_attack_all,
	NULL
};

template class Struct<rpg::Actor>;

} //namespace lcf
