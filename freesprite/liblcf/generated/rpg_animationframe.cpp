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
#include "lcf/rpg/animationframe.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const AnimationFrame& obj) {
	os << "AnimationFrame{";
	os << "cells=";
	for (size_t i = 0; i < obj.cells.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.cells[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
