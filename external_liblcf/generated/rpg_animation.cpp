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
#include "lcf/rpg/animation.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Animation& obj) {
	os << "Animation{";
	os << "name="<< obj.name;
	os << ", animation_name="<< obj.animation_name;
	os << ", large="<< obj.large;
	os << ", timings=";
	for (size_t i = 0; i < obj.timings.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.timings[i];
	}
	os << "]";
	os << ", scope="<< obj.scope;
	os << ", position="<< obj.position;
	os << ", frames=";
	for (size_t i = 0; i < obj.frames.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.frames[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
