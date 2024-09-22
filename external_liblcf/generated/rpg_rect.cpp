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
#include "lcf/rpg/rect.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Rect& obj) {
	os << "Rect{";
	os << "l="<< obj.l;
	os << ", t="<< obj.t;
	os << ", r="<< obj.r;
	os << ", b="<< obj.b;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
