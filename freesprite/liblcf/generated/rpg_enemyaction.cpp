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
#include "lcf/rpg/enemyaction.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const EnemyAction& obj) {
	os << "EnemyAction{";
	os << "kind="<< obj.kind;
	os << ", basic="<< obj.basic;
	os << ", skill_id="<< obj.skill_id;
	os << ", enemy_id="<< obj.enemy_id;
	os << ", condition_type="<< obj.condition_type;
	os << ", condition_param1="<< obj.condition_param1;
	os << ", condition_param2="<< obj.condition_param2;
	os << ", switch_id="<< obj.switch_id;
	os << ", switch_on="<< obj.switch_on;
	os << ", switch_on_id="<< obj.switch_on_id;
	os << ", switch_off="<< obj.switch_off;
	os << ", switch_off_id="<< obj.switch_off_id;
	os << ", rating="<< obj.rating;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
