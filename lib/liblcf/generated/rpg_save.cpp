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
#include "lcf/rpg/save.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Save& obj) {
	os << "Save{";
	os << "title="<< obj.title;
	os << ", system="<< obj.system;
	os << ", screen="<< obj.screen;
	os << ", pictures=";
	for (size_t i = 0; i < obj.pictures.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.pictures[i];
	}
	os << "]";
	os << ", party_location="<< obj.party_location;
	os << ", boat_location="<< obj.boat_location;
	os << ", ship_location="<< obj.ship_location;
	os << ", airship_location="<< obj.airship_location;
	os << ", actors=";
	for (size_t i = 0; i < obj.actors.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.actors[i];
	}
	os << "]";
	os << ", inventory="<< obj.inventory;
	os << ", targets=";
	for (size_t i = 0; i < obj.targets.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.targets[i];
	}
	os << "]";
	os << ", map_info="<< obj.map_info;
	os << ", panorama="<< obj.panorama;
	os << ", foreground_event_execstate="<< obj.foreground_event_execstate;
	os << ", common_events=";
	for (size_t i = 0; i < obj.common_events.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.common_events[i];
	}
	os << "]";
	os << ", easyrpg_data="<< obj.easyrpg_data;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
