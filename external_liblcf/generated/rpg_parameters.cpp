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
#include "lcf/rpg/parameters.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Parameters& obj) {
	os << "Parameters{";
	os << "maxhp=";
	for (size_t i = 0; i < obj.maxhp.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.maxhp[i];
	}
	os << "]";
	os << ", maxsp=";
	for (size_t i = 0; i < obj.maxsp.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.maxsp[i];
	}
	os << "]";
	os << ", attack=";
	for (size_t i = 0; i < obj.attack.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.attack[i];
	}
	os << "]";
	os << ", defense=";
	for (size_t i = 0; i < obj.defense.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.defense[i];
	}
	os << "]";
	os << ", spirit=";
	for (size_t i = 0; i < obj.spirit.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.spirit[i];
	}
	os << "]";
	os << ", agility=";
	for (size_t i = 0; i < obj.agility.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.agility[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
