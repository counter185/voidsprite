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
#include "lcf/rpg/terrain.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Terrain::Flags& obj) {
	for (size_t i = 0; i < obj.flags.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.flags[i];
	}
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Terrain& obj) {
	os << "Terrain{";
	os << "name="<< obj.name;
	os << ", damage="<< obj.damage;
	os << ", encounter_rate="<< obj.encounter_rate;
	os << ", background_name="<< obj.background_name;
	os << ", boat_pass="<< obj.boat_pass;
	os << ", ship_pass="<< obj.ship_pass;
	os << ", airship_pass="<< obj.airship_pass;
	os << ", airship_land="<< obj.airship_land;
	os << ", bush_depth="<< obj.bush_depth;
	os << ", footstep="<< obj.footstep;
	os << ", on_damage_se="<< obj.on_damage_se;
	os << ", background_type="<< obj.background_type;
	os << ", background_a_name="<< obj.background_a_name;
	os << ", background_a_scrollh="<< obj.background_a_scrollh;
	os << ", background_a_scrollv="<< obj.background_a_scrollv;
	os << ", background_a_scrollh_speed="<< obj.background_a_scrollh_speed;
	os << ", background_a_scrollv_speed="<< obj.background_a_scrollv_speed;
	os << ", background_b="<< obj.background_b;
	os << ", background_b_name="<< obj.background_b_name;
	os << ", background_b_scrollh="<< obj.background_b_scrollh;
	os << ", background_b_scrollv="<< obj.background_b_scrollv;
	os << ", background_b_scrollh_speed="<< obj.background_b_scrollh_speed;
	os << ", background_b_scrollv_speed="<< obj.background_b_scrollv_speed;
	os << ", special_flags="<< obj.special_flags;
	os << ", special_back_party="<< obj.special_back_party;
	os << ", special_back_enemies="<< obj.special_back_enemies;
	os << ", special_lateral_party="<< obj.special_lateral_party;
	os << ", special_lateral_enemies="<< obj.special_lateral_enemies;
	os << ", grid_location="<< obj.grid_location;
	os << ", grid_top_y="<< obj.grid_top_y;
	os << ", grid_elongation="<< obj.grid_elongation;
	os << ", grid_inclination="<< obj.grid_inclination;
	os << ", easyrpg_damage_in_percent="<< obj.easyrpg_damage_in_percent;
	os << ", easyrpg_damage_can_kill="<< obj.easyrpg_damage_can_kill;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
