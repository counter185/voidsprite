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
#include "lcf/rpg/eventpagecondition.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const EventPageCondition::Flags& obj) {
	for (size_t i = 0; i < obj.flags.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.flags[i];
	}
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const EventPageCondition& obj) {
	os << "EventPageCondition{";
	os << "flags="<< obj.flags;
	os << ", switch_a_id="<< obj.switch_a_id;
	os << ", switch_b_id="<< obj.switch_b_id;
	os << ", variable_id="<< obj.variable_id;
	os << ", variable_value="<< obj.variable_value;
	os << ", item_id="<< obj.item_id;
	os << ", actor_id="<< obj.actor_id;
	os << ", timer_sec="<< obj.timer_sec;
	os << ", timer2_sec="<< obj.timer2_sec;
	os << ", compare_operator="<< obj.compare_operator;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
