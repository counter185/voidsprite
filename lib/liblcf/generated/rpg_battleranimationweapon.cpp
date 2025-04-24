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
#include "lcf/rpg/battleranimationweapon.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const BattlerAnimationWeapon& obj) {
	os << "BattlerAnimationWeapon{";
	os << "name="<< obj.name;
	os << ", weapon_name="<< obj.weapon_name;
	os << ", weapon_index="<< obj.weapon_index;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
