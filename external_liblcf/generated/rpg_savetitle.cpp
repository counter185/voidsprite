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
#include "lcf/rpg/savetitle.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveTitle& obj) {
	os << "SaveTitle{";
	os << "timestamp="<< obj.timestamp;
	os << ", hero_name="<< obj.hero_name;
	os << ", hero_level="<< obj.hero_level;
	os << ", hero_hp="<< obj.hero_hp;
	os << ", face1_name="<< obj.face1_name;
	os << ", face1_id="<< obj.face1_id;
	os << ", face2_name="<< obj.face2_name;
	os << ", face2_id="<< obj.face2_id;
	os << ", face3_name="<< obj.face3_name;
	os << ", face3_id="<< obj.face3_id;
	os << ", face4_name="<< obj.face4_name;
	os << ", face4_id="<< obj.face4_id;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
