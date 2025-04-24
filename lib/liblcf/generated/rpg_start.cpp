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
#include "lcf/rpg/start.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Start& obj) {
	os << "Start{";
	os << "party_map_id="<< obj.party_map_id;
	os << ", party_x="<< obj.party_x;
	os << ", party_y="<< obj.party_y;
	os << ", boat_map_id="<< obj.boat_map_id;
	os << ", boat_x="<< obj.boat_x;
	os << ", boat_y="<< obj.boat_y;
	os << ", ship_map_id="<< obj.ship_map_id;
	os << ", ship_x="<< obj.ship_x;
	os << ", ship_y="<< obj.ship_y;
	os << ", airship_map_id="<< obj.airship_map_id;
	os << ", airship_x="<< obj.airship_x;
	os << ", airship_y="<< obj.airship_y;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
