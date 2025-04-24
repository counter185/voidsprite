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
#include "lcf/rpg/battleranimation.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const BattlerAnimation& obj) {
	os << "BattlerAnimation{";
	os << "name="<< obj.name;
	os << ", speed="<< obj.speed;
	os << ", poses=";
	for (size_t i = 0; i < obj.poses.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.poses[i];
	}
	os << "]";
	os << ", weapons=";
	for (size_t i = 0; i < obj.weapons.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.weapons[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
