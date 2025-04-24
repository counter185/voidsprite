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
#include "lcf/rpg/system.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const System& obj) {
	os << "System{";
	os << "ldb_id="<< obj.ldb_id;
	os << ", boat_name="<< obj.boat_name;
	os << ", ship_name="<< obj.ship_name;
	os << ", airship_name="<< obj.airship_name;
	os << ", boat_index="<< obj.boat_index;
	os << ", ship_index="<< obj.ship_index;
	os << ", airship_index="<< obj.airship_index;
	os << ", title_name="<< obj.title_name;
	os << ", gameover_name="<< obj.gameover_name;
	os << ", system_name="<< obj.system_name;
	os << ", system2_name="<< obj.system2_name;
	os << ", party=";
	for (size_t i = 0; i < obj.party.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.party[i];
	}
	os << "]";
	os << ", menu_commands=";
	for (size_t i = 0; i < obj.menu_commands.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.menu_commands[i];
	}
	os << "]";
	os << ", title_music="<< obj.title_music;
	os << ", battle_music="<< obj.battle_music;
	os << ", battle_end_music="<< obj.battle_end_music;
	os << ", inn_music="<< obj.inn_music;
	os << ", boat_music="<< obj.boat_music;
	os << ", ship_music="<< obj.ship_music;
	os << ", airship_music="<< obj.airship_music;
	os << ", gameover_music="<< obj.gameover_music;
	os << ", cursor_se="<< obj.cursor_se;
	os << ", decision_se="<< obj.decision_se;
	os << ", cancel_se="<< obj.cancel_se;
	os << ", buzzer_se="<< obj.buzzer_se;
	os << ", battle_se="<< obj.battle_se;
	os << ", escape_se="<< obj.escape_se;
	os << ", enemy_attack_se="<< obj.enemy_attack_se;
	os << ", enemy_damaged_se="<< obj.enemy_damaged_se;
	os << ", actor_damaged_se="<< obj.actor_damaged_se;
	os << ", dodge_se="<< obj.dodge_se;
	os << ", enemy_death_se="<< obj.enemy_death_se;
	os << ", item_se="<< obj.item_se;
	os << ", transition_out="<< obj.transition_out;
	os << ", transition_in="<< obj.transition_in;
	os << ", battle_start_fadeout="<< obj.battle_start_fadeout;
	os << ", battle_start_fadein="<< obj.battle_start_fadein;
	os << ", battle_end_fadeout="<< obj.battle_end_fadeout;
	os << ", battle_end_fadein="<< obj.battle_end_fadein;
	os << ", message_stretch="<< obj.message_stretch;
	os << ", font_id="<< obj.font_id;
	os << ", selected_condition="<< obj.selected_condition;
	os << ", selected_hero="<< obj.selected_hero;
	os << ", battletest_background="<< obj.battletest_background;
	os << ", battletest_data=";
	for (size_t i = 0; i < obj.battletest_data.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.battletest_data[i];
	}
	os << "]";
	os << ", save_count="<< obj.save_count;
	os << ", battletest_terrain="<< obj.battletest_terrain;
	os << ", battletest_formation="<< obj.battletest_formation;
	os << ", battletest_condition="<< obj.battletest_condition;
	os << ", equipment_setting="<< obj.equipment_setting;
	os << ", battletest_alt_terrain="<< obj.battletest_alt_terrain;
	os << ", show_frame="<< obj.show_frame;
	os << ", frame_name="<< obj.frame_name;
	os << ", invert_animations="<< obj.invert_animations;
	os << ", show_title="<< obj.show_title;
	os << ", easyrpg_alternative_exp="<< obj.easyrpg_alternative_exp;
	os << ", easyrpg_battle_options=";
	for (size_t i = 0; i < obj.easyrpg_battle_options.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.easyrpg_battle_options[i];
	}
	os << "]";
	os << ", easyrpg_max_actor_hp="<< obj.easyrpg_max_actor_hp;
	os << ", easyrpg_max_enemy_hp="<< obj.easyrpg_max_enemy_hp;
	os << ", easyrpg_max_damage="<< obj.easyrpg_max_damage;
	os << ", easyrpg_max_exp="<< obj.easyrpg_max_exp;
	os << ", easyrpg_max_level="<< obj.easyrpg_max_level;
	os << ", easyrpg_max_savefiles="<< obj.easyrpg_max_savefiles;
	os << ", easyrpg_max_item_count="<< obj.easyrpg_max_item_count;
	os << ", easyrpg_variable_min_value="<< obj.easyrpg_variable_min_value;
	os << ", easyrpg_variable_max_value="<< obj.easyrpg_variable_max_value;
	os << ", easyrpg_max_actor_sp="<< obj.easyrpg_max_actor_sp;
	os << ", easyrpg_max_enemy_sp="<< obj.easyrpg_max_enemy_sp;
	os << ", easyrpg_max_stat_base_value="<< obj.easyrpg_max_stat_base_value;
	os << ", easyrpg_max_stat_battle_value="<< obj.easyrpg_max_stat_battle_value;
	os << ", easyrpg_use_rpg2k_battle_system="<< obj.easyrpg_use_rpg2k_battle_system;
	os << ", easyrpg_battle_use_rpg2ke_strings="<< obj.easyrpg_battle_use_rpg2ke_strings;
	os << ", easyrpg_use_rpg2k_battle_commands="<< obj.easyrpg_use_rpg2k_battle_commands;
	os << ", easyrpg_default_actorai="<< obj.easyrpg_default_actorai;
	os << ", easyrpg_default_enemyai="<< obj.easyrpg_default_enemyai;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
