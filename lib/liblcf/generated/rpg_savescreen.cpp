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
#include "lcf/rpg/savescreen.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveScreen& obj) {
	os << "SaveScreen{";
	os << "tint_finish_red="<< obj.tint_finish_red;
	os << ", tint_finish_green="<< obj.tint_finish_green;
	os << ", tint_finish_blue="<< obj.tint_finish_blue;
	os << ", tint_finish_sat="<< obj.tint_finish_sat;
	os << ", tint_current_red="<< obj.tint_current_red;
	os << ", tint_current_green="<< obj.tint_current_green;
	os << ", tint_current_blue="<< obj.tint_current_blue;
	os << ", tint_current_sat="<< obj.tint_current_sat;
	os << ", tint_time_left="<< obj.tint_time_left;
	os << ", flash_continuous="<< obj.flash_continuous;
	os << ", flash_red="<< obj.flash_red;
	os << ", flash_green="<< obj.flash_green;
	os << ", flash_blue="<< obj.flash_blue;
	os << ", flash_current_level="<< obj.flash_current_level;
	os << ", flash_time_left="<< obj.flash_time_left;
	os << ", shake_continuous="<< obj.shake_continuous;
	os << ", shake_strength="<< obj.shake_strength;
	os << ", shake_speed="<< obj.shake_speed;
	os << ", shake_position="<< obj.shake_position;
	os << ", shake_position_y="<< obj.shake_position_y;
	os << ", shake_time_left="<< obj.shake_time_left;
	os << ", pan_x="<< obj.pan_x;
	os << ", pan_y="<< obj.pan_y;
	os << ", battleanim_id="<< obj.battleanim_id;
	os << ", battleanim_target="<< obj.battleanim_target;
	os << ", battleanim_frame="<< obj.battleanim_frame;
	os << ", battleanim_active="<< obj.battleanim_active;
	os << ", battleanim_global="<< obj.battleanim_global;
	os << ", weather="<< obj.weather;
	os << ", weather_strength="<< obj.weather_strength;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
