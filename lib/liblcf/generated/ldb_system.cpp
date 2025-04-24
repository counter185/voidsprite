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

// Read System.

template <>
char const* const Struct<rpg::System>::name = "System";
static TypedField<rpg::System, int32_t> static_ldb_id(
	&rpg::System::ldb_id,
	LDB_Reader::ChunkSystem::ldb_id,
	"ldb_id",
	0,
	0
);
static TypedField<rpg::System, DBString> static_boat_name(
	&rpg::System::boat_name,
	LDB_Reader::ChunkSystem::boat_name,
	"boat_name",
	0,
	0
);
static TypedField<rpg::System, DBString> static_ship_name(
	&rpg::System::ship_name,
	LDB_Reader::ChunkSystem::ship_name,
	"ship_name",
	0,
	0
);
static TypedField<rpg::System, DBString> static_airship_name(
	&rpg::System::airship_name,
	LDB_Reader::ChunkSystem::airship_name,
	"airship_name",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_boat_index(
	&rpg::System::boat_index,
	LDB_Reader::ChunkSystem::boat_index,
	"boat_index",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_ship_index(
	&rpg::System::ship_index,
	LDB_Reader::ChunkSystem::ship_index,
	"ship_index",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_airship_index(
	&rpg::System::airship_index,
	LDB_Reader::ChunkSystem::airship_index,
	"airship_index",
	0,
	0
);
static TypedField<rpg::System, DBString> static_title_name(
	&rpg::System::title_name,
	LDB_Reader::ChunkSystem::title_name,
	"title_name",
	0,
	0
);
static TypedField<rpg::System, DBString> static_gameover_name(
	&rpg::System::gameover_name,
	LDB_Reader::ChunkSystem::gameover_name,
	"gameover_name",
	0,
	0
);
static TypedField<rpg::System, DBString> static_system_name(
	&rpg::System::system_name,
	LDB_Reader::ChunkSystem::system_name,
	"system_name",
	0,
	0
);
static TypedField<rpg::System, DBString> static_system2_name(
	&rpg::System::system2_name,
	LDB_Reader::ChunkSystem::system2_name,
	"system2_name",
	0,
	1
);
static CountField<rpg::System, std::vector<int16_t>> static_size_party(
	&rpg::System::party,
	LDB_Reader::ChunkSystem::party_size,
	0,
	0
);
static TypedField<rpg::System, std::vector<int16_t>> static_party(
	&rpg::System::party,
	LDB_Reader::ChunkSystem::party,
	"party",
	1,
	0
);
static CountField<rpg::System, std::vector<int16_t>> static_size_menu_commands(
	&rpg::System::menu_commands,
	LDB_Reader::ChunkSystem::menu_commands_size,
	0,
	1
);
static TypedField<rpg::System, std::vector<int16_t>> static_menu_commands(
	&rpg::System::menu_commands,
	LDB_Reader::ChunkSystem::menu_commands,
	"menu_commands",
	1,
	1
);
static TypedField<rpg::System, rpg::Music> static_title_music(
	&rpg::System::title_music,
	LDB_Reader::ChunkSystem::title_music,
	"title_music",
	1,
	0
);
static TypedField<rpg::System, rpg::Music> static_battle_music(
	&rpg::System::battle_music,
	LDB_Reader::ChunkSystem::battle_music,
	"battle_music",
	1,
	0
);
static TypedField<rpg::System, rpg::Music> static_battle_end_music(
	&rpg::System::battle_end_music,
	LDB_Reader::ChunkSystem::battle_end_music,
	"battle_end_music",
	1,
	0
);
static TypedField<rpg::System, rpg::Music> static_inn_music(
	&rpg::System::inn_music,
	LDB_Reader::ChunkSystem::inn_music,
	"inn_music",
	1,
	0
);
static TypedField<rpg::System, rpg::Music> static_boat_music(
	&rpg::System::boat_music,
	LDB_Reader::ChunkSystem::boat_music,
	"boat_music",
	1,
	0
);
static TypedField<rpg::System, rpg::Music> static_ship_music(
	&rpg::System::ship_music,
	LDB_Reader::ChunkSystem::ship_music,
	"ship_music",
	1,
	0
);
static TypedField<rpg::System, rpg::Music> static_airship_music(
	&rpg::System::airship_music,
	LDB_Reader::ChunkSystem::airship_music,
	"airship_music",
	1,
	0
);
static TypedField<rpg::System, rpg::Music> static_gameover_music(
	&rpg::System::gameover_music,
	LDB_Reader::ChunkSystem::gameover_music,
	"gameover_music",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_cursor_se(
	&rpg::System::cursor_se,
	LDB_Reader::ChunkSystem::cursor_se,
	"cursor_se",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_decision_se(
	&rpg::System::decision_se,
	LDB_Reader::ChunkSystem::decision_se,
	"decision_se",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_cancel_se(
	&rpg::System::cancel_se,
	LDB_Reader::ChunkSystem::cancel_se,
	"cancel_se",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_buzzer_se(
	&rpg::System::buzzer_se,
	LDB_Reader::ChunkSystem::buzzer_se,
	"buzzer_se",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_battle_se(
	&rpg::System::battle_se,
	LDB_Reader::ChunkSystem::battle_se,
	"battle_se",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_escape_se(
	&rpg::System::escape_se,
	LDB_Reader::ChunkSystem::escape_se,
	"escape_se",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_enemy_attack_se(
	&rpg::System::enemy_attack_se,
	LDB_Reader::ChunkSystem::enemy_attack_se,
	"enemy_attack_se",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_enemy_damaged_se(
	&rpg::System::enemy_damaged_se,
	LDB_Reader::ChunkSystem::enemy_damaged_se,
	"enemy_damaged_se",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_actor_damaged_se(
	&rpg::System::actor_damaged_se,
	LDB_Reader::ChunkSystem::actor_damaged_se,
	"actor_damaged_se",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_dodge_se(
	&rpg::System::dodge_se,
	LDB_Reader::ChunkSystem::dodge_se,
	"dodge_se",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_enemy_death_se(
	&rpg::System::enemy_death_se,
	LDB_Reader::ChunkSystem::enemy_death_se,
	"enemy_death_se",
	1,
	0
);
static TypedField<rpg::System, rpg::Sound> static_item_se(
	&rpg::System::item_se,
	LDB_Reader::ChunkSystem::item_se,
	"item_se",
	1,
	0
);
static TypedField<rpg::System, int32_t> static_transition_out(
	&rpg::System::transition_out,
	LDB_Reader::ChunkSystem::transition_out,
	"transition_out",
	1,
	0
);
static TypedField<rpg::System, int32_t> static_transition_in(
	&rpg::System::transition_in,
	LDB_Reader::ChunkSystem::transition_in,
	"transition_in",
	1,
	0
);
static TypedField<rpg::System, int32_t> static_battle_start_fadeout(
	&rpg::System::battle_start_fadeout,
	LDB_Reader::ChunkSystem::battle_start_fadeout,
	"battle_start_fadeout",
	1,
	0
);
static TypedField<rpg::System, int32_t> static_battle_start_fadein(
	&rpg::System::battle_start_fadein,
	LDB_Reader::ChunkSystem::battle_start_fadein,
	"battle_start_fadein",
	1,
	0
);
static TypedField<rpg::System, int32_t> static_battle_end_fadeout(
	&rpg::System::battle_end_fadeout,
	LDB_Reader::ChunkSystem::battle_end_fadeout,
	"battle_end_fadeout",
	1,
	0
);
static TypedField<rpg::System, int32_t> static_battle_end_fadein(
	&rpg::System::battle_end_fadein,
	LDB_Reader::ChunkSystem::battle_end_fadein,
	"battle_end_fadein",
	1,
	0
);
static TypedField<rpg::System, int32_t> static_message_stretch(
	&rpg::System::message_stretch,
	LDB_Reader::ChunkSystem::message_stretch,
	"message_stretch",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_font_id(
	&rpg::System::font_id,
	LDB_Reader::ChunkSystem::font_id,
	"font_id",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_selected_condition(
	&rpg::System::selected_condition,
	LDB_Reader::ChunkSystem::selected_condition,
	"selected_condition",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_selected_hero(
	&rpg::System::selected_hero,
	LDB_Reader::ChunkSystem::selected_hero,
	"selected_hero",
	1,
	0
);
static TypedField<rpg::System, DBString> static_battletest_background(
	&rpg::System::battletest_background,
	LDB_Reader::ChunkSystem::battletest_background,
	"battletest_background",
	0,
	0
);
static TypedField<rpg::System, std::vector<rpg::TestBattler>> static_battletest_data(
	&rpg::System::battletest_data,
	LDB_Reader::ChunkSystem::battletest_data,
	"battletest_data",
	1,
	0
);
static TypedField<rpg::System, int32_t> static_save_count(
	&rpg::System::save_count,
	LDB_Reader::ChunkSystem::save_count,
	"save_count",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_battletest_terrain(
	&rpg::System::battletest_terrain,
	LDB_Reader::ChunkSystem::battletest_terrain,
	"battletest_terrain",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_battletest_formation(
	&rpg::System::battletest_formation,
	LDB_Reader::ChunkSystem::battletest_formation,
	"battletest_formation",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_battletest_condition(
	&rpg::System::battletest_condition,
	LDB_Reader::ChunkSystem::battletest_condition,
	"battletest_condition",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_equipment_setting(
	&rpg::System::equipment_setting,
	LDB_Reader::ChunkSystem::equipment_setting,
	"equipment_setting",
	0,
	1
);
static TypedField<rpg::System, int32_t> static_battletest_alt_terrain(
	&rpg::System::battletest_alt_terrain,
	LDB_Reader::ChunkSystem::battletest_alt_terrain,
	"battletest_alt_terrain",
	0,
	1
);
static TypedField<rpg::System, bool> static_show_frame(
	&rpg::System::show_frame,
	LDB_Reader::ChunkSystem::show_frame,
	"show_frame",
	0,
	1
);
static TypedField<rpg::System, DBString> static_frame_name(
	&rpg::System::frame_name,
	LDB_Reader::ChunkSystem::frame_name,
	"frame_name",
	0,
	1
);
static TypedField<rpg::System, bool> static_invert_animations(
	&rpg::System::invert_animations,
	LDB_Reader::ChunkSystem::invert_animations,
	"invert_animations",
	0,
	1
);
static TypedField<rpg::System, bool> static_show_title(
	&rpg::System::show_title,
	LDB_Reader::ChunkSystem::show_title,
	"show_title",
	0,
	1
);
static TypedField<rpg::System, int32_t> static_easyrpg_alternative_exp(
	&rpg::System::easyrpg_alternative_exp,
	LDB_Reader::ChunkSystem::easyrpg_alternative_exp,
	"easyrpg_alternative_exp",
	0,
	0
);
static TypedField<rpg::System, std::vector<int16_t>> static_easyrpg_battle_options(
	&rpg::System::easyrpg_battle_options,
	LDB_Reader::ChunkSystem::easyrpg_battle_options,
	"easyrpg_battle_options",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_max_actor_hp(
	&rpg::System::easyrpg_max_actor_hp,
	LDB_Reader::ChunkSystem::easyrpg_max_actor_hp,
	"easyrpg_max_actor_hp",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_max_enemy_hp(
	&rpg::System::easyrpg_max_enemy_hp,
	LDB_Reader::ChunkSystem::easyrpg_max_enemy_hp,
	"easyrpg_max_enemy_hp",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_max_damage(
	&rpg::System::easyrpg_max_damage,
	LDB_Reader::ChunkSystem::easyrpg_max_damage,
	"easyrpg_max_damage",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_max_exp(
	&rpg::System::easyrpg_max_exp,
	LDB_Reader::ChunkSystem::easyrpg_max_exp,
	"easyrpg_max_exp",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_max_level(
	&rpg::System::easyrpg_max_level,
	LDB_Reader::ChunkSystem::easyrpg_max_level,
	"easyrpg_max_level",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_max_savefiles(
	&rpg::System::easyrpg_max_savefiles,
	LDB_Reader::ChunkSystem::easyrpg_max_savefiles,
	"easyrpg_max_savefiles",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_max_item_count(
	&rpg::System::easyrpg_max_item_count,
	LDB_Reader::ChunkSystem::easyrpg_max_item_count,
	"easyrpg_max_item_count",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_variable_min_value(
	&rpg::System::easyrpg_variable_min_value,
	LDB_Reader::ChunkSystem::easyrpg_variable_min_value,
	"easyrpg_variable_min_value",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_variable_max_value(
	&rpg::System::easyrpg_variable_max_value,
	LDB_Reader::ChunkSystem::easyrpg_variable_max_value,
	"easyrpg_variable_max_value",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_max_actor_sp(
	&rpg::System::easyrpg_max_actor_sp,
	LDB_Reader::ChunkSystem::easyrpg_max_actor_sp,
	"easyrpg_max_actor_sp",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_max_enemy_sp(
	&rpg::System::easyrpg_max_enemy_sp,
	LDB_Reader::ChunkSystem::easyrpg_max_enemy_sp,
	"easyrpg_max_enemy_sp",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_max_stat_base_value(
	&rpg::System::easyrpg_max_stat_base_value,
	LDB_Reader::ChunkSystem::easyrpg_max_stat_base_value,
	"easyrpg_max_stat_base_value",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_max_stat_battle_value(
	&rpg::System::easyrpg_max_stat_battle_value,
	LDB_Reader::ChunkSystem::easyrpg_max_stat_battle_value,
	"easyrpg_max_stat_battle_value",
	0,
	0
);
static TypedField<rpg::System, bool> static_easyrpg_use_rpg2k_battle_system(
	&rpg::System::easyrpg_use_rpg2k_battle_system,
	LDB_Reader::ChunkSystem::easyrpg_use_rpg2k_battle_system,
	"easyrpg_use_rpg2k_battle_system",
	0,
	1
);
static TypedField<rpg::System, bool> static_easyrpg_battle_use_rpg2ke_strings(
	&rpg::System::easyrpg_battle_use_rpg2ke_strings,
	LDB_Reader::ChunkSystem::easyrpg_battle_use_rpg2ke_strings,
	"easyrpg_battle_use_rpg2ke_strings",
	0,
	1
);
static TypedField<rpg::System, bool> static_easyrpg_use_rpg2k_battle_commands(
	&rpg::System::easyrpg_use_rpg2k_battle_commands,
	LDB_Reader::ChunkSystem::easyrpg_use_rpg2k_battle_commands,
	"easyrpg_use_rpg2k_battle_commands",
	0,
	1
);
static TypedField<rpg::System, int32_t> static_easyrpg_default_actorai(
	&rpg::System::easyrpg_default_actorai,
	LDB_Reader::ChunkSystem::easyrpg_default_actorai,
	"easyrpg_default_actorai",
	0,
	0
);
static TypedField<rpg::System, int32_t> static_easyrpg_default_enemyai(
	&rpg::System::easyrpg_default_enemyai,
	LDB_Reader::ChunkSystem::easyrpg_default_enemyai,
	"easyrpg_default_enemyai",
	0,
	0
);


template <>
Field<rpg::System> const* Struct<rpg::System>::fields[] = {
	&static_ldb_id,
	&static_boat_name,
	&static_ship_name,
	&static_airship_name,
	&static_boat_index,
	&static_ship_index,
	&static_airship_index,
	&static_title_name,
	&static_gameover_name,
	&static_system_name,
	&static_system2_name,
	&static_size_party,
	&static_party,
	&static_size_menu_commands,
	&static_menu_commands,
	&static_title_music,
	&static_battle_music,
	&static_battle_end_music,
	&static_inn_music,
	&static_boat_music,
	&static_ship_music,
	&static_airship_music,
	&static_gameover_music,
	&static_cursor_se,
	&static_decision_se,
	&static_cancel_se,
	&static_buzzer_se,
	&static_battle_se,
	&static_escape_se,
	&static_enemy_attack_se,
	&static_enemy_damaged_se,
	&static_actor_damaged_se,
	&static_dodge_se,
	&static_enemy_death_se,
	&static_item_se,
	&static_transition_out,
	&static_transition_in,
	&static_battle_start_fadeout,
	&static_battle_start_fadein,
	&static_battle_end_fadeout,
	&static_battle_end_fadein,
	&static_message_stretch,
	&static_font_id,
	&static_selected_condition,
	&static_selected_hero,
	&static_battletest_background,
	&static_battletest_data,
	&static_save_count,
	&static_battletest_terrain,
	&static_battletest_formation,
	&static_battletest_condition,
	&static_equipment_setting,
	&static_battletest_alt_terrain,
	&static_show_frame,
	&static_frame_name,
	&static_invert_animations,
	&static_show_title,
	&static_easyrpg_alternative_exp,
	&static_easyrpg_battle_options,
	&static_easyrpg_max_actor_hp,
	&static_easyrpg_max_enemy_hp,
	&static_easyrpg_max_damage,
	&static_easyrpg_max_exp,
	&static_easyrpg_max_level,
	&static_easyrpg_max_savefiles,
	&static_easyrpg_max_item_count,
	&static_easyrpg_variable_min_value,
	&static_easyrpg_variable_max_value,
	&static_easyrpg_max_actor_sp,
	&static_easyrpg_max_enemy_sp,
	&static_easyrpg_max_stat_base_value,
	&static_easyrpg_max_stat_battle_value,
	&static_easyrpg_use_rpg2k_battle_system,
	&static_easyrpg_battle_use_rpg2ke_strings,
	&static_easyrpg_use_rpg2k_battle_commands,
	&static_easyrpg_default_actorai,
	&static_easyrpg_default_enemyai,
	NULL
};

template class Struct<rpg::System>;

} //namespace lcf
