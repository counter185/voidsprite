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
#include "lcf/rpg/trooppagecondition.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const TroopPageCondition::Flags& obj) {
	for (size_t i = 0; i < obj.flags.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.flags[i];
	}
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const TroopPageCondition& obj) {
	os << "TroopPageCondition{";
	os << "flags="<< obj.flags;
	os << ", switch_a_id="<< obj.switch_a_id;
	os << ", switch_b_id="<< obj.switch_b_id;
	os << ", variable_id="<< obj.variable_id;
	os << ", variable_value="<< obj.variable_value;
	os << ", turn_a="<< obj.turn_a;
	os << ", turn_b="<< obj.turn_b;
	os << ", fatigue_min="<< obj.fatigue_min;
	os << ", fatigue_max="<< obj.fatigue_max;
	os << ", enemy_id="<< obj.enemy_id;
	os << ", enemy_hp_min="<< obj.enemy_hp_min;
	os << ", enemy_hp_max="<< obj.enemy_hp_max;
	os << ", actor_id="<< obj.actor_id;
	os << ", actor_hp_min="<< obj.actor_hp_min;
	os << ", actor_hp_max="<< obj.actor_hp_max;
	os << ", turn_enemy_id="<< obj.turn_enemy_id;
	os << ", turn_enemy_a="<< obj.turn_enemy_a;
	os << ", turn_enemy_b="<< obj.turn_enemy_b;
	os << ", turn_actor_id="<< obj.turn_actor_id;
	os << ", turn_actor_a="<< obj.turn_actor_a;
	os << ", turn_actor_b="<< obj.turn_actor_b;
	os << ", command_actor_id="<< obj.command_actor_id;
	os << ", command_id="<< obj.command_id;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
