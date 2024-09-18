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
#include "lcf/rpg/savevehiclelocation.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveVehicleLocation& obj) {
	os << "SaveVehicleLocation{";
	os << "vehicle="<< obj.vehicle;
	os << ", remaining_ascent="<< obj.remaining_ascent;
	os << ", remaining_descent="<< obj.remaining_descent;
	os << ", orig_sprite_name="<< obj.orig_sprite_name;
	os << ", orig_sprite_id="<< obj.orig_sprite_id;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
