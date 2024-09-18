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
#include "lcf/rpg/event.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Event& obj) {
	os << "Event{";
	os << "name="<< obj.name;
	os << ", x="<< obj.x;
	os << ", y="<< obj.y;
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
