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
#include "lcf/rpg/saveeasyrpgwindow.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveEasyRpgWindow::Flags& obj) {
	for (size_t i = 0; i < obj.flags.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.flags[i];
	}
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const SaveEasyRpgWindow& obj) {
	os << "SaveEasyRpgWindow{";
	os << "texts=";
	for (size_t i = 0; i < obj.texts.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.texts[i];
	}
	os << "]";
	os << ", width="<< obj.width;
	os << ", height="<< obj.height;
	os << ", system_name="<< obj.system_name;
	os << ", message_stretch="<< obj.message_stretch;
	os << ", flags="<< obj.flags;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
