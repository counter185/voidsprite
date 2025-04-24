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
#include "lcf/rpg/troop.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Troop& obj) {
	os << "Troop{";
	os << "name="<< obj.name;
	os << ", members=";
	for (size_t i = 0; i < obj.members.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.members[i];
	}
	os << "]";
	os << ", auto_alignment="<< obj.auto_alignment;
	os << ", terrain_set=";
	for (size_t i = 0; i < obj.terrain_set.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.terrain_set[i];
	}
	os << "]";
	os << ", appear_randomly="<< obj.appear_randomly;
	os << ", pages=";
	for (size_t i = 0; i < obj.pages.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.pages[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
