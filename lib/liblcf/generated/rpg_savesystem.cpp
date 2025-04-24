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
#include "lcf/rpg/savesystem.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveSystem& obj) {
	os << "SaveSystem{";
	os << "scene="<< obj.scene;
	os << ", frame_count="<< obj.frame_count;
	os << ", graphics_name="<< obj.graphics_name;
	os << ", message_stretch="<< obj.message_stretch;
	os << ", font_id="<< obj.font_id;
	os << ", switches=";
	for (size_t i = 0; i < obj.switches.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.switches[i];
	}
	os << "]";
	os << ", variables=";
	for (size_t i = 0; i < obj.variables.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.variables[i];
	}
	os << "]";
	os << ", message_transparent="<< obj.message_transparent;
	os << ", message_position="<< obj.message_position;
	os << ", message_prevent_overlap="<< obj.message_prevent_overlap;
	os << ", message_continue_events="<< obj.message_continue_events;
	os << ", face_name="<< obj.face_name;
	os << ", face_id="<< obj.face_id;
	os << ", face_right="<< obj.face_right;
	os << ", face_flip="<< obj.face_flip;
	os << ", event_message_active="<< obj.event_message_active;
	os << ", music_stopping="<< obj.music_stopping;
	os << ", title_music="<< obj.title_music;
	os << ", battle_music="<< obj.battle_music;
	os << ", battle_end_music="<< obj.battle_end_music;
	os << ", inn_music="<< obj.inn_music;
	os << ", current_music="<< obj.current_music;
	os << ", before_vehicle_music="<< obj.before_vehicle_music;
	os << ", before_battle_music="<< obj.before_battle_music;
	os << ", stored_music="<< obj.stored_music;
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
	os << ", teleport_allowed="<< obj.teleport_allowed;
	os << ", escape_allowed="<< obj.escape_allowed;
	os << ", save_allowed="<< obj.save_allowed;
	os << ", menu_allowed="<< obj.menu_allowed;
	os << ", background="<< obj.background;
	os << ", save_count="<< obj.save_count;
	os << ", save_slot="<< obj.save_slot;
	os << ", atb_mode="<< obj.atb_mode;
	os << ", maniac_strings=";
	for (size_t i = 0; i < obj.maniac_strings.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.maniac_strings[i];
	}
	os << "]";
	os << ", maniac_frameskip="<< obj.maniac_frameskip;
	os << ", maniac_picture_limit="<< obj.maniac_picture_limit;
	os << ", maniac_options=";
	for (size_t i = 0; i < obj.maniac_options.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.maniac_options[i];
	}
	os << "]";
	os << ", maniac_joypad_bindings=";
	for (size_t i = 0; i < obj.maniac_joypad_bindings.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.maniac_joypad_bindings[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
