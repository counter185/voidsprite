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
#include "lcf/rpg/saveeasyrpgtext.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveEasyRpgText::Flags& obj) {
	for (size_t i = 0; i < obj.flags.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.flags[i];
	}
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const SaveEasyRpgText& obj) {
	os << "SaveEasyRpgText{";
	os << "text="<< obj.text;
	os << ", position_x="<< obj.position_x;
	os << ", position_y="<< obj.position_y;
	os << ", font_name="<< obj.font_name;
	os << ", font_size="<< obj.font_size;
	os << ", letter_spacing="<< obj.letter_spacing;
	os << ", line_spacing="<< obj.line_spacing;
	os << ", flags="<< obj.flags;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
