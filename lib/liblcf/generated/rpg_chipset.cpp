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
#include "lcf/rpg/chipset.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Chipset& obj) {
	os << "Chipset{";
	os << "name="<< obj.name;
	os << ", chipset_name="<< obj.chipset_name;
	os << ", terrain_data=";
	for (size_t i = 0; i < obj.terrain_data.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.terrain_data[i];
	}
	os << "]";
	os << ", passable_data_lower=";
	for (size_t i = 0; i < obj.passable_data_lower.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.passable_data_lower[i];
	}
	os << "]";
	os << ", passable_data_upper=";
	for (size_t i = 0; i < obj.passable_data_upper.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.passable_data_upper[i];
	}
	os << "]";
	os << ", animation_type="<< obj.animation_type;
	os << ", animation_speed="<< obj.animation_speed;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
