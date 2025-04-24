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
#include "lcf/rpg/animationcelldata.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const AnimationCellData& obj) {
	os << "AnimationCellData{";
	os << "valid="<< obj.valid;
	os << ", cell_id="<< obj.cell_id;
	os << ", x="<< obj.x;
	os << ", y="<< obj.y;
	os << ", zoom="<< obj.zoom;
	os << ", tone_red="<< obj.tone_red;
	os << ", tone_green="<< obj.tone_green;
	os << ", tone_blue="<< obj.tone_blue;
	os << ", tone_gray="<< obj.tone_gray;
	os << ", transparency="<< obj.transparency;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
