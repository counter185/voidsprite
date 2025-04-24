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
#include "lcf/rpg/animationtiming.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const AnimationTiming& obj) {
	os << "AnimationTiming{";
	os << "frame="<< obj.frame;
	os << ", se="<< obj.se;
	os << ", flash_scope="<< obj.flash_scope;
	os << ", flash_red="<< obj.flash_red;
	os << ", flash_green="<< obj.flash_green;
	os << ", flash_blue="<< obj.flash_blue;
	os << ", flash_power="<< obj.flash_power;
	os << ", screen_shake="<< obj.screen_shake;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
