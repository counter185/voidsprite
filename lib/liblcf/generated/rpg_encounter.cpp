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
#include "lcf/rpg/encounter.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Encounter& obj) {
	os << "Encounter{";
	os << "troop_id="<< obj.troop_id;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
