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
#include "lcf/rpg/savemapinfo.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveMapInfo& obj) {
	os << "SaveMapInfo{";
	os << "position_x="<< obj.position_x;
	os << ", position_y="<< obj.position_y;
	os << ", encounter_steps="<< obj.encounter_steps;
	os << ", chipset_id="<< obj.chipset_id;
	os << ", events=";
	for (size_t i = 0; i < obj.events.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.events[i];
	}
	os << "]";
	os << ", lower_tiles=";
	for (size_t i = 0; i < obj.lower_tiles.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.lower_tiles[i];
	}
	os << "]";
	os << ", upper_tiles=";
	for (size_t i = 0; i < obj.upper_tiles.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.upper_tiles[i];
	}
	os << "]";
	os << ", parallax_name="<< obj.parallax_name;
	os << ", parallax_horz="<< obj.parallax_horz;
	os << ", parallax_vert="<< obj.parallax_vert;
	os << ", parallax_horz_auto="<< obj.parallax_horz_auto;
	os << ", parallax_horz_speed="<< obj.parallax_horz_speed;
	os << ", parallax_vert_auto="<< obj.parallax_vert_auto;
	os << ", parallax_vert_speed="<< obj.parallax_vert_speed;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
