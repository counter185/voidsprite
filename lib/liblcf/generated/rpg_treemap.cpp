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
#include "lcf/rpg/treemap.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const TreeMap& obj) {
	os << "TreeMap{";
	os << "maps=";
	for (size_t i = 0; i < obj.maps.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.maps[i];
	}
	os << "]";
	os << ", tree_order=";
	for (size_t i = 0; i < obj.tree_order.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.tree_order[i];
	}
	os << "]";
	os << ", active_node="<< obj.active_node;
	os << ", start="<< obj.start;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
