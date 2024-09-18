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
#include "lcf/rpg/savepicture.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SavePicture::Flags& obj) {
	for (size_t i = 0; i < obj.flags.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.flags[i];
	}
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const SavePicture& obj) {
	os << "SavePicture{";
	os << "name="<< obj.name;
	os << ", start_x="<< obj.start_x;
	os << ", start_y="<< obj.start_y;
	os << ", current_x="<< obj.current_x;
	os << ", current_y="<< obj.current_y;
	os << ", fixed_to_map="<< obj.fixed_to_map;
	os << ", current_magnify="<< obj.current_magnify;
	os << ", current_top_trans="<< obj.current_top_trans;
	os << ", use_transparent_color="<< obj.use_transparent_color;
	os << ", current_red="<< obj.current_red;
	os << ", current_green="<< obj.current_green;
	os << ", current_blue="<< obj.current_blue;
	os << ", current_sat="<< obj.current_sat;
	os << ", effect_mode="<< obj.effect_mode;
	os << ", current_effect_power="<< obj.current_effect_power;
	os << ", current_bot_trans="<< obj.current_bot_trans;
	os << ", spritesheet_cols="<< obj.spritesheet_cols;
	os << ", spritesheet_rows="<< obj.spritesheet_rows;
	os << ", spritesheet_frame="<< obj.spritesheet_frame;
	os << ", spritesheet_speed="<< obj.spritesheet_speed;
	os << ", frames="<< obj.frames;
	os << ", spritesheet_play_once="<< obj.spritesheet_play_once;
	os << ", map_layer="<< obj.map_layer;
	os << ", battle_layer="<< obj.battle_layer;
	os << ", flags="<< obj.flags;
	os << ", finish_x="<< obj.finish_x;
	os << ", finish_y="<< obj.finish_y;
	os << ", finish_magnify="<< obj.finish_magnify;
	os << ", finish_top_trans="<< obj.finish_top_trans;
	os << ", finish_bot_trans="<< obj.finish_bot_trans;
	os << ", finish_red="<< obj.finish_red;
	os << ", finish_green="<< obj.finish_green;
	os << ", finish_blue="<< obj.finish_blue;
	os << ", finish_sat="<< obj.finish_sat;
	os << ", finish_effect_power="<< obj.finish_effect_power;
	os << ", time_left="<< obj.time_left;
	os << ", current_rotation="<< obj.current_rotation;
	os << ", current_waver="<< obj.current_waver;
	os << ", easyrpg_flip="<< obj.easyrpg_flip;
	os << ", easyrpg_blend_mode="<< obj.easyrpg_blend_mode;
	os << ", easyrpg_type="<< obj.easyrpg_type;
	os << ", maniac_current_magnify_height="<< obj.maniac_current_magnify_height;
	os << ", maniac_finish_magnify_height="<< obj.maniac_finish_magnify_height;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
