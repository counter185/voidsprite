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
#include "lcf/rpg/attribute.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Attribute& obj) {
	os << "Attribute{";
	os << "name="<< obj.name;
	os << ", type="<< obj.type;
	os << ", a_rate="<< obj.a_rate;
	os << ", b_rate="<< obj.b_rate;
	os << ", c_rate="<< obj.c_rate;
	os << ", d_rate="<< obj.d_rate;
	os << ", e_rate="<< obj.e_rate;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
