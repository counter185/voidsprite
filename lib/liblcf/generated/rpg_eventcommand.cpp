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
#include "lcf/rpg/eventcommand.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const EventCommand& obj) {
	os << "EventCommand{";
	os << "code="<< obj.code;
	os << ", indent="<< obj.indent;
	os << ", string="<< obj.string;
	os << ", parameters=";
	for (size_t i = 0; i < obj.parameters.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.parameters[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
