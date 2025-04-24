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
#include "lcf/rpg/mapinfo.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const MapInfo& obj) {
	os << "MapInfo{";
	os << "name="<< obj.name;
	os << ", parent_map="<< obj.parent_map;
	os << ", indentation="<< obj.indentation;
	os << ", type="<< obj.type;
	os << ", scrollbar_x="<< obj.scrollbar_x;
	os << ", scrollbar_y="<< obj.scrollbar_y;
	os << ", expanded_node="<< obj.expanded_node;
	os << ", music_type="<< obj.music_type;
	os << ", music="<< obj.music;
	os << ", background_type="<< obj.background_type;
	os << ", background_name="<< obj.background_name;
	os << ", teleport="<< obj.teleport;
	os << ", escape="<< obj.escape;
	os << ", save="<< obj.save;
	os << ", encounters=";
	for (size_t i = 0; i < obj.encounters.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.encounters[i];
	}
	os << "]";
	os << ", encounter_steps="<< obj.encounter_steps;
	os << ", area_rect="<< obj.area_rect;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
