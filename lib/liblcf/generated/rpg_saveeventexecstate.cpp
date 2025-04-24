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
#include "lcf/rpg/saveeventexecstate.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveEventExecState& obj) {
	os << "SaveEventExecState{";
	os << "stack=";
	for (size_t i = 0; i < obj.stack.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.stack[i];
	}
	os << "]";
	os << ", show_message="<< obj.show_message;
	os << ", abort_on_escape="<< obj.abort_on_escape;
	os << ", wait_movement="<< obj.wait_movement;
	os << ", keyinput_wait="<< obj.keyinput_wait;
	os << ", keyinput_variable="<< obj.keyinput_variable;
	os << ", keyinput_all_directions="<< obj.keyinput_all_directions;
	os << ", keyinput_decision="<< obj.keyinput_decision;
	os << ", keyinput_cancel="<< obj.keyinput_cancel;
	os << ", keyinput_2kshift_2k3numbers="<< obj.keyinput_2kshift_2k3numbers;
	os << ", keyinput_2kdown_2k3operators="<< obj.keyinput_2kdown_2k3operators;
	os << ", keyinput_2kleft_2k3shift="<< obj.keyinput_2kleft_2k3shift;
	os << ", keyinput_2kright="<< obj.keyinput_2kright;
	os << ", keyinput_2kup="<< obj.keyinput_2kup;
	os << ", wait_time="<< obj.wait_time;
	os << ", keyinput_time_variable="<< obj.keyinput_time_variable;
	os << ", keyinput_2k3down="<< obj.keyinput_2k3down;
	os << ", keyinput_2k3left="<< obj.keyinput_2k3left;
	os << ", keyinput_2k3right="<< obj.keyinput_2k3right;
	os << ", keyinput_2k3up="<< obj.keyinput_2k3up;
	os << ", keyinput_timed="<< obj.keyinput_timed;
	os << ", wait_key_enter="<< obj.wait_key_enter;
	os << ", easyrpg_active="<< obj.easyrpg_active;
	os << ", easyrpg_string="<< obj.easyrpg_string;
	os << ", easyrpg_parameters=";
	for (size_t i = 0; i < obj.easyrpg_parameters.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.easyrpg_parameters[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
