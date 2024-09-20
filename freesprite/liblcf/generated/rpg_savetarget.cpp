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
#include "lcf/rpg/savetarget.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveTarget& obj) {
	os << "SaveTarget{";
	os << "map_id="<< obj.map_id;
	os << ", map_x="<< obj.map_x;
	os << ", map_y="<< obj.map_y;
	os << ", switch_on="<< obj.switch_on;
	os << ", switch_id="<< obj.switch_id;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
