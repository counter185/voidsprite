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
#include "lcf/rpg/equipment.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Equipment& obj) {
	os << "Equipment{";
	os << "weapon_id="<< obj.weapon_id;
	os << ", shield_id="<< obj.shield_id;
	os << ", armor_id="<< obj.armor_id;
	os << ", helmet_id="<< obj.helmet_id;
	os << ", accessory_id="<< obj.accessory_id;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
