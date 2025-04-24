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
#include "lcf/rpg/saveeventexecframe.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const SaveEventExecFrame& obj) {
	os << "SaveEventExecFrame{";
	os << "commands=";
	for (size_t i = 0; i < obj.commands.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.commands[i];
	}
	os << "]";
	os << ", current_command="<< obj.current_command;
	os << ", event_id="<< obj.event_id;
	os << ", triggered_by_decision_key="<< obj.triggered_by_decision_key;
	os << ", subcommand_path=";
	for (size_t i = 0; i < obj.subcommand_path.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.subcommand_path[i];
	}
	os << "]";
	os << ", maniac_event_info="<< obj.maniac_event_info;
	os << ", maniac_event_id="<< obj.maniac_event_id;
	os << ", maniac_event_page_id="<< obj.maniac_event_page_id;
	os << ", maniac_loop_info_size="<< obj.maniac_loop_info_size;
	os << ", maniac_loop_info=";
	for (size_t i = 0; i < obj.maniac_loop_info.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.maniac_loop_info[i];
	}
	os << "]";
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
