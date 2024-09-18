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

// Read Skill.

template <>
char const* const Struct<rpg::Skill>::name = "Skill";
static TypedField<rpg::Skill, DBString> static_name(
	&rpg::Skill::name,
	LDB_Reader::ChunkSkill::name,
	"name",
	0,
	0
);
static TypedField<rpg::Skill, DBString> static_description(
	&rpg::Skill::description,
	LDB_Reader::ChunkSkill::description,
	"description",
	0,
	0
);
static TypedField<rpg::Skill, DBString> static_using_message1(
	&rpg::Skill::using_message1,
	LDB_Reader::ChunkSkill::using_message1,
	"using_message1",
	0,
	0
);
static TypedField<rpg::Skill, DBString> static_using_message2(
	&rpg::Skill::using_message2,
	LDB_Reader::ChunkSkill::using_message2,
	"using_message2",
	0,
	0
);
static TypedField<rpg::Skill, int32_t> static_failure_message(
	&rpg::Skill::failure_message,
	LDB_Reader::ChunkSkill::failure_message,
	"failure_message",
	0,
	0
);
static TypedField<rpg::Skill, int32_t> static_type(
	&rpg::Skill::type,
	LDB_Reader::ChunkSkill::type,
	"type",
	1,
	0
);
static TypedField<rpg::Skill, int32_t> static_sp_type(
	&rpg::Skill::sp_type,
	LDB_Reader::ChunkSkill::sp_type,
	"sp_type",
	0,
	1
);
static TypedField<rpg::Skill, int32_t> static_sp_percent(
	&rpg::Skill::sp_percent,
	LDB_Reader::ChunkSkill::sp_percent,
	"sp_percent",
	0,
	1
);
static TypedField<rpg::Skill, int32_t> static_sp_cost(
	&rpg::Skill::sp_cost,
	LDB_Reader::ChunkSkill::sp_cost,
	"sp_cost",
	0,
	0
);
static TypedField<rpg::Skill, int32_t> static_scope(
	&rpg::Skill::scope,
	LDB_Reader::ChunkSkill::scope,
	"scope",
	1,
	0
);
static TypedField<rpg::Skill, int32_t> static_switch_id(
	&rpg::Skill::switch_id,
	LDB_Reader::ChunkSkill::switch_id,
	"switch_id",
	0,
	0
);
static TypedField<rpg::Skill, int32_t> static_animation_id(
	&rpg::Skill::animation_id,
	LDB_Reader::ChunkSkill::animation_id,
	"animation_id",
	0,
	0
);
static TypedField<rpg::Skill, rpg::Sound> static_sound_effect(
	&rpg::Skill::sound_effect,
	LDB_Reader::ChunkSkill::sound_effect,
	"sound_effect",
	1,
	0
);
static TypedField<rpg::Skill, bool> static_occasion_field(
	&rpg::Skill::occasion_field,
	LDB_Reader::ChunkSkill::occasion_field,
	"occasion_field",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_occasion_battle(
	&rpg::Skill::occasion_battle,
	LDB_Reader::ChunkSkill::occasion_battle,
	"occasion_battle",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_reverse_state_effect(
	&rpg::Skill::reverse_state_effect,
	LDB_Reader::ChunkSkill::reverse_state_effect,
	"reverse_state_effect",
	0,
	1
);
static TypedField<rpg::Skill, int32_t> static_physical_rate(
	&rpg::Skill::physical_rate,
	LDB_Reader::ChunkSkill::physical_rate,
	"physical_rate",
	0,
	0
);
static TypedField<rpg::Skill, int32_t> static_magical_rate(
	&rpg::Skill::magical_rate,
	LDB_Reader::ChunkSkill::magical_rate,
	"magical_rate",
	0,
	0
);
static TypedField<rpg::Skill, int32_t> static_variance(
	&rpg::Skill::variance,
	LDB_Reader::ChunkSkill::variance,
	"variance",
	0,
	0
);
static TypedField<rpg::Skill, int32_t> static_power(
	&rpg::Skill::power,
	LDB_Reader::ChunkSkill::power,
	"power",
	0,
	0
);
static TypedField<rpg::Skill, int32_t> static_hit(
	&rpg::Skill::hit,
	LDB_Reader::ChunkSkill::hit,
	"hit",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_affect_hp(
	&rpg::Skill::affect_hp,
	LDB_Reader::ChunkSkill::affect_hp,
	"affect_hp",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_affect_sp(
	&rpg::Skill::affect_sp,
	LDB_Reader::ChunkSkill::affect_sp,
	"affect_sp",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_affect_attack(
	&rpg::Skill::affect_attack,
	LDB_Reader::ChunkSkill::affect_attack,
	"affect_attack",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_affect_defense(
	&rpg::Skill::affect_defense,
	LDB_Reader::ChunkSkill::affect_defense,
	"affect_defense",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_affect_spirit(
	&rpg::Skill::affect_spirit,
	LDB_Reader::ChunkSkill::affect_spirit,
	"affect_spirit",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_affect_agility(
	&rpg::Skill::affect_agility,
	LDB_Reader::ChunkSkill::affect_agility,
	"affect_agility",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_absorb_damage(
	&rpg::Skill::absorb_damage,
	LDB_Reader::ChunkSkill::absorb_damage,
	"absorb_damage",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_ignore_defense(
	&rpg::Skill::ignore_defense,
	LDB_Reader::ChunkSkill::ignore_defense,
	"ignore_defense",
	0,
	0
);
static SizeField<rpg::Skill, DBBitArray> static_size_state_effects(
	&rpg::Skill::state_effects,
	LDB_Reader::ChunkSkill::state_effects_size,
	0,
	0
);
static TypedField<rpg::Skill, DBBitArray> static_state_effects(
	&rpg::Skill::state_effects,
	LDB_Reader::ChunkSkill::state_effects,
	"state_effects",
	1,
	0
);
static SizeField<rpg::Skill, DBBitArray> static_size_attribute_effects(
	&rpg::Skill::attribute_effects,
	LDB_Reader::ChunkSkill::attribute_effects_size,
	0,
	0
);
static TypedField<rpg::Skill, DBBitArray> static_attribute_effects(
	&rpg::Skill::attribute_effects,
	LDB_Reader::ChunkSkill::attribute_effects,
	"attribute_effects",
	1,
	0
);
static TypedField<rpg::Skill, bool> static_affect_attr_defence(
	&rpg::Skill::affect_attr_defence,
	LDB_Reader::ChunkSkill::affect_attr_defence,
	"affect_attr_defence",
	0,
	0
);
static TypedField<rpg::Skill, int32_t> static_battler_animation(
	&rpg::Skill::battler_animation,
	LDB_Reader::ChunkSkill::battler_animation,
	"battler_animation",
	0,
	1
);
static TypedField<rpg::Skill, std::vector<rpg::BattlerAnimationItemSkill>> static_battler_animation_data(
	&rpg::Skill::battler_animation_data,
	LDB_Reader::ChunkSkill::battler_animation_data,
	"battler_animation_data",
	1,
	1
);
static TypedField<rpg::Skill, DBString> static_easyrpg_battle2k3_message(
	&rpg::Skill::easyrpg_battle2k3_message,
	LDB_Reader::ChunkSkill::easyrpg_battle2k3_message,
	"easyrpg_battle2k3_message",
	0,
	1
);
static TypedField<rpg::Skill, bool> static_easyrpg_ignore_reflect(
	&rpg::Skill::easyrpg_ignore_reflect,
	LDB_Reader::ChunkSkill::easyrpg_ignore_reflect,
	"easyrpg_ignore_reflect",
	0,
	1
);
static TypedField<rpg::Skill, int32_t> static_easyrpg_state_hit(
	&rpg::Skill::easyrpg_state_hit,
	LDB_Reader::ChunkSkill::easyrpg_state_hit,
	"easyrpg_state_hit",
	0,
	0
);
static TypedField<rpg::Skill, int32_t> static_easyrpg_attribute_hit(
	&rpg::Skill::easyrpg_attribute_hit,
	LDB_Reader::ChunkSkill::easyrpg_attribute_hit,
	"easyrpg_attribute_hit",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_easyrpg_ignore_restrict_skill(
	&rpg::Skill::easyrpg_ignore_restrict_skill,
	LDB_Reader::ChunkSkill::easyrpg_ignore_restrict_skill,
	"easyrpg_ignore_restrict_skill",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_easyrpg_ignore_restrict_magic(
	&rpg::Skill::easyrpg_ignore_restrict_magic,
	LDB_Reader::ChunkSkill::easyrpg_ignore_restrict_magic,
	"easyrpg_ignore_restrict_magic",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_easyrpg_enable_stat_absorbing(
	&rpg::Skill::easyrpg_enable_stat_absorbing,
	LDB_Reader::ChunkSkill::easyrpg_enable_stat_absorbing,
	"easyrpg_enable_stat_absorbing",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_easyrpg_affected_by_evade_all_physical_attacks(
	&rpg::Skill::easyrpg_affected_by_evade_all_physical_attacks,
	LDB_Reader::ChunkSkill::easyrpg_affected_by_evade_all_physical_attacks,
	"easyrpg_affected_by_evade_all_physical_attacks",
	0,
	1
);
static TypedField<rpg::Skill, int32_t> static_easyrpg_critical_hit_chance(
	&rpg::Skill::easyrpg_critical_hit_chance,
	LDB_Reader::ChunkSkill::easyrpg_critical_hit_chance,
	"easyrpg_critical_hit_chance",
	0,
	0
);
static TypedField<rpg::Skill, bool> static_easyrpg_affected_by_row_modifiers(
	&rpg::Skill::easyrpg_affected_by_row_modifiers,
	LDB_Reader::ChunkSkill::easyrpg_affected_by_row_modifiers,
	"easyrpg_affected_by_row_modifiers",
	0,
	1
);
static TypedField<rpg::Skill, int32_t> static_easyrpg_hp_type(
	&rpg::Skill::easyrpg_hp_type,
	LDB_Reader::ChunkSkill::easyrpg_hp_type,
	"easyrpg_hp_type",
	0,
	1
);
static TypedField<rpg::Skill, int32_t> static_easyrpg_hp_percent(
	&rpg::Skill::easyrpg_hp_percent,
	LDB_Reader::ChunkSkill::easyrpg_hp_percent,
	"easyrpg_hp_percent",
	0,
	1
);
static TypedField<rpg::Skill, int32_t> static_easyrpg_hp_cost(
	&rpg::Skill::easyrpg_hp_cost,
	LDB_Reader::ChunkSkill::easyrpg_hp_cost,
	"easyrpg_hp_cost",
	0,
	0
);


template <>
Field<rpg::Skill> const* Struct<rpg::Skill>::fields[] = {
	&static_name,
	&static_description,
	&static_using_message1,
	&static_using_message2,
	&static_failure_message,
	&static_type,
	&static_sp_type,
	&static_sp_percent,
	&static_sp_cost,
	&static_scope,
	&static_switch_id,
	&static_animation_id,
	&static_sound_effect,
	&static_occasion_field,
	&static_occasion_battle,
	&static_reverse_state_effect,
	&static_physical_rate,
	&static_magical_rate,
	&static_variance,
	&static_power,
	&static_hit,
	&static_affect_hp,
	&static_affect_sp,
	&static_affect_attack,
	&static_affect_defense,
	&static_affect_spirit,
	&static_affect_agility,
	&static_absorb_damage,
	&static_ignore_defense,
	&static_size_state_effects,
	&static_state_effects,
	&static_size_attribute_effects,
	&static_attribute_effects,
	&static_affect_attr_defence,
	&static_battler_animation,
	&static_battler_animation_data,
	&static_easyrpg_battle2k3_message,
	&static_easyrpg_ignore_reflect,
	&static_easyrpg_state_hit,
	&static_easyrpg_attribute_hit,
	&static_easyrpg_ignore_restrict_skill,
	&static_easyrpg_ignore_restrict_magic,
	&static_easyrpg_enable_stat_absorbing,
	&static_easyrpg_affected_by_evade_all_physical_attacks,
	&static_easyrpg_critical_hit_chance,
	&static_easyrpg_affected_by_row_modifiers,
	&static_easyrpg_hp_type,
	&static_easyrpg_hp_percent,
	&static_easyrpg_hp_cost,
	NULL
};

template class Struct<rpg::Skill>;

} //namespace lcf
