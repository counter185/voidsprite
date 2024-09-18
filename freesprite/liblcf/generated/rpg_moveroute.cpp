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
#include "lcf/rpg/moveroute.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const MoveRoute& obj) {
	os << "MoveRoute{";
	os << "move_commands=";
	for (size_t i = 0; i < obj.move_commands.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.move_commands[i];
	}
	os << "]";
	os << ", repeat="<< obj.repeat;
	os << ", skippable="<< obj.skippable;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
