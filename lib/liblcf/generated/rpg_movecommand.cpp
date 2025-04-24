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
#include "lcf/rpg/movecommand.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const MoveCommand& obj) {
	os << "MoveCommand{";
	os << "command_id="<< obj.command_id;
	os << ", parameter_string="<< obj.parameter_string;
	os << ", parameter_a="<< obj.parameter_a;
	os << ", parameter_b="<< obj.parameter_b;
	os << ", parameter_c="<< obj.parameter_c;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
