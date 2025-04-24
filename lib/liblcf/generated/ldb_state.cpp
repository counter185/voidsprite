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

// Read State.

template <>
char const* const Struct<rpg::State>::name = "State";
static TypedField<rpg::State, DBString> static_name(
	&rpg::State::name,
	LDB_Reader::ChunkState::name,
	"name",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_type(
	&rpg::State::type,
	LDB_Reader::ChunkState::type,
	"type",
	1,
	0
);
static TypedField<rpg::State, int32_t> static_color(
	&rpg::State::color,
	LDB_Reader::ChunkState::color,
	"color",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_priority(
	&rpg::State::priority,
	LDB_Reader::ChunkState::priority,
	"priority",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_restriction(
	&rpg::State::restriction,
	LDB_Reader::ChunkState::restriction,
	"restriction",
	1,
	0
);
static TypedField<rpg::State, int32_t> static_a_rate(
	&rpg::State::a_rate,
	LDB_Reader::ChunkState::a_rate,
	"a_rate",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_b_rate(
	&rpg::State::b_rate,
	LDB_Reader::ChunkState::b_rate,
	"b_rate",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_c_rate(
	&rpg::State::c_rate,
	LDB_Reader::ChunkState::c_rate,
	"c_rate",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_d_rate(
	&rpg::State::d_rate,
	LDB_Reader::ChunkState::d_rate,
	"d_rate",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_e_rate(
	&rpg::State::e_rate,
	LDB_Reader::ChunkState::e_rate,
	"e_rate",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_hold_turn(
	&rpg::State::hold_turn,
	LDB_Reader::ChunkState::hold_turn,
	"hold_turn",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_auto_release_prob(
	&rpg::State::auto_release_prob,
	LDB_Reader::ChunkState::auto_release_prob,
	"auto_release_prob",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_release_by_damage(
	&rpg::State::release_by_damage,
	LDB_Reader::ChunkState::release_by_damage,
	"release_by_damage",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_affect_type(
	&rpg::State::affect_type,
	LDB_Reader::ChunkState::affect_type,
	"affect_type",
	0,
	1
);
static TypedField<rpg::State, bool> static_affect_attack(
	&rpg::State::affect_attack,
	LDB_Reader::ChunkState::affect_attack,
	"affect_attack",
	0,
	0
);
static TypedField<rpg::State, bool> static_affect_defense(
	&rpg::State::affect_defense,
	LDB_Reader::ChunkState::affect_defense,
	"affect_defense",
	0,
	0
);
static TypedField<rpg::State, bool> static_affect_spirit(
	&rpg::State::affect_spirit,
	LDB_Reader::ChunkState::affect_spirit,
	"affect_spirit",
	0,
	0
);
static TypedField<rpg::State, bool> static_affect_agility(
	&rpg::State::affect_agility,
	LDB_Reader::ChunkState::affect_agility,
	"affect_agility",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_reduce_hit_ratio(
	&rpg::State::reduce_hit_ratio,
	LDB_Reader::ChunkState::reduce_hit_ratio,
	"reduce_hit_ratio",
	0,
	0
);
static TypedField<rpg::State, bool> static_avoid_attacks(
	&rpg::State::avoid_attacks,
	LDB_Reader::ChunkState::avoid_attacks,
	"avoid_attacks",
	0,
	1
);
static TypedField<rpg::State, bool> static_reflect_magic(
	&rpg::State::reflect_magic,
	LDB_Reader::ChunkState::reflect_magic,
	"reflect_magic",
	0,
	1
);
static TypedField<rpg::State, bool> static_cursed(
	&rpg::State::cursed,
	LDB_Reader::ChunkState::cursed,
	"cursed",
	0,
	1
);
static TypedField<rpg::State, int32_t> static_battler_animation_id(
	&rpg::State::battler_animation_id,
	LDB_Reader::ChunkState::battler_animation_id,
	"battler_animation_id",
	0,
	1
);
static TypedField<rpg::State, bool> static_restrict_skill(
	&rpg::State::restrict_skill,
	LDB_Reader::ChunkState::restrict_skill,
	"restrict_skill",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_restrict_skill_level(
	&rpg::State::restrict_skill_level,
	LDB_Reader::ChunkState::restrict_skill_level,
	"restrict_skill_level",
	0,
	0
);
static TypedField<rpg::State, bool> static_restrict_magic(
	&rpg::State::restrict_magic,
	LDB_Reader::ChunkState::restrict_magic,
	"restrict_magic",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_restrict_magic_level(
	&rpg::State::restrict_magic_level,
	LDB_Reader::ChunkState::restrict_magic_level,
	"restrict_magic_level",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_hp_change_type(
	&rpg::State::hp_change_type,
	LDB_Reader::ChunkState::hp_change_type,
	"hp_change_type",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_sp_change_type(
	&rpg::State::sp_change_type,
	LDB_Reader::ChunkState::sp_change_type,
	"sp_change_type",
	0,
	0
);
static TypedField<rpg::State, DBString> static_message_actor(
	&rpg::State::message_actor,
	LDB_Reader::ChunkState::message_actor,
	"message_actor",
	0,
	0
);
static TypedField<rpg::State, DBString> static_message_enemy(
	&rpg::State::message_enemy,
	LDB_Reader::ChunkState::message_enemy,
	"message_enemy",
	0,
	0
);
static TypedField<rpg::State, DBString> static_message_already(
	&rpg::State::message_already,
	LDB_Reader::ChunkState::message_already,
	"message_already",
	0,
	0
);
static TypedField<rpg::State, DBString> static_message_affected(
	&rpg::State::message_affected,
	LDB_Reader::ChunkState::message_affected,
	"message_affected",
	0,
	0
);
static TypedField<rpg::State, DBString> static_message_recovery(
	&rpg::State::message_recovery,
	LDB_Reader::ChunkState::message_recovery,
	"message_recovery",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_hp_change_max(
	&rpg::State::hp_change_max,
	LDB_Reader::ChunkState::hp_change_max,
	"hp_change_max",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_hp_change_val(
	&rpg::State::hp_change_val,
	LDB_Reader::ChunkState::hp_change_val,
	"hp_change_val",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_hp_change_map_steps(
	&rpg::State::hp_change_map_steps,
	LDB_Reader::ChunkState::hp_change_map_steps,
	"hp_change_map_steps",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_hp_change_map_val(
	&rpg::State::hp_change_map_val,
	LDB_Reader::ChunkState::hp_change_map_val,
	"hp_change_map_val",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_sp_change_max(
	&rpg::State::sp_change_max,
	LDB_Reader::ChunkState::sp_change_max,
	"sp_change_max",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_sp_change_val(
	&rpg::State::sp_change_val,
	LDB_Reader::ChunkState::sp_change_val,
	"sp_change_val",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_sp_change_map_steps(
	&rpg::State::sp_change_map_steps,
	LDB_Reader::ChunkState::sp_change_map_steps,
	"sp_change_map_steps",
	0,
	0
);
static TypedField<rpg::State, int32_t> static_sp_change_map_val(
	&rpg::State::sp_change_map_val,
	LDB_Reader::ChunkState::sp_change_map_val,
	"sp_change_map_val",
	0,
	0
);
static SizeField<rpg::State, DBBitArray> static_size_easyrpg_immune_states(
	&rpg::State::easyrpg_immune_states,
	LDB_Reader::ChunkState::easyrpg_immune_states_size,
	0,
	0
);
static TypedField<rpg::State, DBBitArray> static_easyrpg_immune_states(
	&rpg::State::easyrpg_immune_states,
	LDB_Reader::ChunkState::easyrpg_immune_states,
	"easyrpg_immune_states",
	1,
	0
);


template <>
Field<rpg::State> const* Struct<rpg::State>::fields[] = {
	&static_name,
	&static_type,
	&static_color,
	&static_priority,
	&static_restriction,
	&static_a_rate,
	&static_b_rate,
	&static_c_rate,
	&static_d_rate,
	&static_e_rate,
	&static_hold_turn,
	&static_auto_release_prob,
	&static_release_by_damage,
	&static_affect_type,
	&static_affect_attack,
	&static_affect_defense,
	&static_affect_spirit,
	&static_affect_agility,
	&static_reduce_hit_ratio,
	&static_avoid_attacks,
	&static_reflect_magic,
	&static_cursed,
	&static_battler_animation_id,
	&static_restrict_skill,
	&static_restrict_skill_level,
	&static_restrict_magic,
	&static_restrict_magic_level,
	&static_hp_change_type,
	&static_sp_change_type,
	&static_message_actor,
	&static_message_enemy,
	&static_message_already,
	&static_message_affected,
	&static_message_recovery,
	&static_hp_change_max,
	&static_hp_change_val,
	&static_hp_change_map_steps,
	&static_hp_change_map_val,
	&static_sp_change_max,
	&static_sp_change_val,
	&static_sp_change_map_steps,
	&static_sp_change_map_val,
	&static_size_easyrpg_immune_states,
	&static_easyrpg_immune_states,
	NULL
};

template class Struct<rpg::State>;

} //namespace lcf
