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

// Read Item.

template <>
char const* const Struct<rpg::Item>::name = "Item";
static TypedField<rpg::Item, DBString> static_name(
	&rpg::Item::name,
	LDB_Reader::ChunkItem::name,
	"name",
	0,
	0
);
static TypedField<rpg::Item, DBString> static_description(
	&rpg::Item::description,
	LDB_Reader::ChunkItem::description,
	"description",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_type(
	&rpg::Item::type,
	LDB_Reader::ChunkItem::type,
	"type",
	1,
	0
);
static TypedField<rpg::Item, int32_t> static_price(
	&rpg::Item::price,
	LDB_Reader::ChunkItem::price,
	"price",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_uses(
	&rpg::Item::uses,
	LDB_Reader::ChunkItem::uses,
	"uses",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_atk_points1(
	&rpg::Item::atk_points1,
	LDB_Reader::ChunkItem::atk_points1,
	"atk_points1",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_def_points1(
	&rpg::Item::def_points1,
	LDB_Reader::ChunkItem::def_points1,
	"def_points1",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_spi_points1(
	&rpg::Item::spi_points1,
	LDB_Reader::ChunkItem::spi_points1,
	"spi_points1",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_agi_points1(
	&rpg::Item::agi_points1,
	LDB_Reader::ChunkItem::agi_points1,
	"agi_points1",
	0,
	0
);
static TypedField<rpg::Item, bool> static_two_handed(
	&rpg::Item::two_handed,
	LDB_Reader::ChunkItem::two_handed,
	"two_handed",
	1,
	0
);
static TypedField<rpg::Item, int32_t> static_sp_cost(
	&rpg::Item::sp_cost,
	LDB_Reader::ChunkItem::sp_cost,
	"sp_cost",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_hit(
	&rpg::Item::hit,
	LDB_Reader::ChunkItem::hit,
	"hit",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_critical_hit(
	&rpg::Item::critical_hit,
	LDB_Reader::ChunkItem::critical_hit,
	"critical_hit",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_animation_id(
	&rpg::Item::animation_id,
	LDB_Reader::ChunkItem::animation_id,
	"animation_id",
	0,
	0
);
static TypedField<rpg::Item, bool> static_preemptive(
	&rpg::Item::preemptive,
	LDB_Reader::ChunkItem::preemptive,
	"preemptive",
	0,
	0
);
static TypedField<rpg::Item, bool> static_dual_attack(
	&rpg::Item::dual_attack,
	LDB_Reader::ChunkItem::dual_attack,
	"dual_attack",
	0,
	0
);
static TypedField<rpg::Item, bool> static_attack_all(
	&rpg::Item::attack_all,
	LDB_Reader::ChunkItem::attack_all,
	"attack_all",
	0,
	0
);
static TypedField<rpg::Item, bool> static_ignore_evasion(
	&rpg::Item::ignore_evasion,
	LDB_Reader::ChunkItem::ignore_evasion,
	"ignore_evasion",
	0,
	0
);
static TypedField<rpg::Item, bool> static_prevent_critical(
	&rpg::Item::prevent_critical,
	LDB_Reader::ChunkItem::prevent_critical,
	"prevent_critical",
	0,
	0
);
static TypedField<rpg::Item, bool> static_raise_evasion(
	&rpg::Item::raise_evasion,
	LDB_Reader::ChunkItem::raise_evasion,
	"raise_evasion",
	0,
	0
);
static TypedField<rpg::Item, bool> static_half_sp_cost(
	&rpg::Item::half_sp_cost,
	LDB_Reader::ChunkItem::half_sp_cost,
	"half_sp_cost",
	0,
	0
);
static TypedField<rpg::Item, bool> static_no_terrain_damage(
	&rpg::Item::no_terrain_damage,
	LDB_Reader::ChunkItem::no_terrain_damage,
	"no_terrain_damage",
	0,
	0
);
static TypedField<rpg::Item, bool> static_cursed(
	&rpg::Item::cursed,
	LDB_Reader::ChunkItem::cursed,
	"cursed",
	0,
	1
);
static TypedField<rpg::Item, bool> static_entire_party(
	&rpg::Item::entire_party,
	LDB_Reader::ChunkItem::entire_party,
	"entire_party",
	1,
	0
);
static TypedField<rpg::Item, int32_t> static_recover_hp_rate(
	&rpg::Item::recover_hp_rate,
	LDB_Reader::ChunkItem::recover_hp_rate,
	"recover_hp_rate",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_recover_hp(
	&rpg::Item::recover_hp,
	LDB_Reader::ChunkItem::recover_hp,
	"recover_hp",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_recover_sp_rate(
	&rpg::Item::recover_sp_rate,
	LDB_Reader::ChunkItem::recover_sp_rate,
	"recover_sp_rate",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_recover_sp(
	&rpg::Item::recover_sp,
	LDB_Reader::ChunkItem::recover_sp,
	"recover_sp",
	0,
	0
);
static TypedField<rpg::Item, bool> static_occasion_field1(
	&rpg::Item::occasion_field1,
	LDB_Reader::ChunkItem::occasion_field1,
	"occasion_field1",
	0,
	0
);
static TypedField<rpg::Item, bool> static_ko_only(
	&rpg::Item::ko_only,
	LDB_Reader::ChunkItem::ko_only,
	"ko_only",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_max_hp_points(
	&rpg::Item::max_hp_points,
	LDB_Reader::ChunkItem::max_hp_points,
	"max_hp_points",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_max_sp_points(
	&rpg::Item::max_sp_points,
	LDB_Reader::ChunkItem::max_sp_points,
	"max_sp_points",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_atk_points2(
	&rpg::Item::atk_points2,
	LDB_Reader::ChunkItem::atk_points2,
	"atk_points2",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_def_points2(
	&rpg::Item::def_points2,
	LDB_Reader::ChunkItem::def_points2,
	"def_points2",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_spi_points2(
	&rpg::Item::spi_points2,
	LDB_Reader::ChunkItem::spi_points2,
	"spi_points2",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_agi_points2(
	&rpg::Item::agi_points2,
	LDB_Reader::ChunkItem::agi_points2,
	"agi_points2",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_using_message(
	&rpg::Item::using_message,
	LDB_Reader::ChunkItem::using_message,
	"using_message",
	1,
	0
);
static TypedField<rpg::Item, int32_t> static_skill_id(
	&rpg::Item::skill_id,
	LDB_Reader::ChunkItem::skill_id,
	"skill_id",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_switch_id(
	&rpg::Item::switch_id,
	LDB_Reader::ChunkItem::switch_id,
	"switch_id",
	0,
	0
);
static TypedField<rpg::Item, bool> static_occasion_field2(
	&rpg::Item::occasion_field2,
	LDB_Reader::ChunkItem::occasion_field2,
	"occasion_field2",
	0,
	0
);
static TypedField<rpg::Item, bool> static_occasion_battle(
	&rpg::Item::occasion_battle,
	LDB_Reader::ChunkItem::occasion_battle,
	"occasion_battle",
	0,
	0
);
static SizeField<rpg::Item, DBBitArray> static_size_actor_set(
	&rpg::Item::actor_set,
	LDB_Reader::ChunkItem::actor_set_size,
	0,
	0
);
static TypedField<rpg::Item, DBBitArray> static_actor_set(
	&rpg::Item::actor_set,
	LDB_Reader::ChunkItem::actor_set,
	"actor_set",
	1,
	0
);
static SizeField<rpg::Item, DBBitArray> static_size_state_set(
	&rpg::Item::state_set,
	LDB_Reader::ChunkItem::state_set_size,
	0,
	0
);
static TypedField<rpg::Item, DBBitArray> static_state_set(
	&rpg::Item::state_set,
	LDB_Reader::ChunkItem::state_set,
	"state_set",
	1,
	0
);
static SizeField<rpg::Item, DBBitArray> static_size_attribute_set(
	&rpg::Item::attribute_set,
	LDB_Reader::ChunkItem::attribute_set_size,
	0,
	0
);
static TypedField<rpg::Item, DBBitArray> static_attribute_set(
	&rpg::Item::attribute_set,
	LDB_Reader::ChunkItem::attribute_set,
	"attribute_set",
	1,
	0
);
static TypedField<rpg::Item, int32_t> static_state_chance(
	&rpg::Item::state_chance,
	LDB_Reader::ChunkItem::state_chance,
	"state_chance",
	0,
	0
);
static TypedField<rpg::Item, bool> static_reverse_state_effect(
	&rpg::Item::reverse_state_effect,
	LDB_Reader::ChunkItem::reverse_state_effect,
	"reverse_state_effect",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_weapon_animation(
	&rpg::Item::weapon_animation,
	LDB_Reader::ChunkItem::weapon_animation,
	"weapon_animation",
	0,
	1
);
static TypedField<rpg::Item, std::vector<rpg::BattlerAnimationItemSkill>> static_animation_data(
	&rpg::Item::animation_data,
	LDB_Reader::ChunkItem::animation_data,
	"animation_data",
	1,
	1
);
static TypedField<rpg::Item, bool> static_use_skill(
	&rpg::Item::use_skill,
	LDB_Reader::ChunkItem::use_skill,
	"use_skill",
	0,
	1
);
static SizeField<rpg::Item, DBBitArray> static_size_class_set(
	&rpg::Item::class_set,
	LDB_Reader::ChunkItem::class_set_size,
	0,
	1
);
static TypedField<rpg::Item, DBBitArray> static_class_set(
	&rpg::Item::class_set,
	LDB_Reader::ChunkItem::class_set,
	"class_set",
	1,
	1
);
static TypedField<rpg::Item, int32_t> static_ranged_trajectory(
	&rpg::Item::ranged_trajectory,
	LDB_Reader::ChunkItem::ranged_trajectory,
	"ranged_trajectory",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_ranged_target(
	&rpg::Item::ranged_target,
	LDB_Reader::ChunkItem::ranged_target,
	"ranged_target",
	0,
	0
);
static TypedField<rpg::Item, DBString> static_easyrpg_using_message(
	&rpg::Item::easyrpg_using_message,
	LDB_Reader::ChunkItem::easyrpg_using_message,
	"easyrpg_using_message",
	0,
	0
);
static TypedField<rpg::Item, int32_t> static_easyrpg_max_count(
	&rpg::Item::easyrpg_max_count,
	LDB_Reader::ChunkItem::easyrpg_max_count,
	"easyrpg_max_count",
	0,
	0
);


template <>
Field<rpg::Item> const* Struct<rpg::Item>::fields[] = {
	&static_name,
	&static_description,
	&static_type,
	&static_price,
	&static_uses,
	&static_atk_points1,
	&static_def_points1,
	&static_spi_points1,
	&static_agi_points1,
	&static_two_handed,
	&static_sp_cost,
	&static_hit,
	&static_critical_hit,
	&static_animation_id,
	&static_preemptive,
	&static_dual_attack,
	&static_attack_all,
	&static_ignore_evasion,
	&static_prevent_critical,
	&static_raise_evasion,
	&static_half_sp_cost,
	&static_no_terrain_damage,
	&static_cursed,
	&static_entire_party,
	&static_recover_hp_rate,
	&static_recover_hp,
	&static_recover_sp_rate,
	&static_recover_sp,
	&static_occasion_field1,
	&static_ko_only,
	&static_max_hp_points,
	&static_max_sp_points,
	&static_atk_points2,
	&static_def_points2,
	&static_spi_points2,
	&static_agi_points2,
	&static_using_message,
	&static_skill_id,
	&static_switch_id,
	&static_occasion_field2,
	&static_occasion_battle,
	&static_size_actor_set,
	&static_actor_set,
	&static_size_state_set,
	&static_state_set,
	&static_size_attribute_set,
	&static_attribute_set,
	&static_state_chance,
	&static_reverse_state_effect,
	&static_weapon_animation,
	&static_animation_data,
	&static_use_skill,
	&static_size_class_set,
	&static_class_set,
	&static_ranged_trajectory,
	&static_ranged_target,
	&static_easyrpg_using_message,
	&static_easyrpg_max_count,
	NULL
};

template class Struct<rpg::Item>;

} //namespace lcf
