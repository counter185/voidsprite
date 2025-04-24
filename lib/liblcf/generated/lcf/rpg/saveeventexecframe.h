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

#ifndef LCF_RPG_SAVEEVENTEXECFRAME_H
#define LCF_RPG_SAVEEVENTEXECFRAME_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/rpg/eventcommand.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveEventExecFrame class.
 */
namespace lcf {
namespace rpg {
	class SaveEventExecFrame {
	public:
		int ID = 0;
		std::vector<EventCommand> commands;
		int32_t current_command = 0;
		int32_t event_id = 0;
		bool triggered_by_decision_key = false;
		std::vector<uint8_t> subcommand_path;
		int32_t maniac_event_info = 0;
		int32_t maniac_event_id = 0;
		int32_t maniac_event_page_id = 0;
		int32_t maniac_loop_info_size = 0;
		std::vector<int32_t> maniac_loop_info;
	};

	inline bool operator==(const SaveEventExecFrame& l, const SaveEventExecFrame& r) {
		return l.commands == r.commands
		&& l.current_command == r.current_command
		&& l.event_id == r.event_id
		&& l.triggered_by_decision_key == r.triggered_by_decision_key
		&& l.subcommand_path == r.subcommand_path
		&& l.maniac_event_info == r.maniac_event_info
		&& l.maniac_event_id == r.maniac_event_id
		&& l.maniac_event_page_id == r.maniac_event_page_id
		&& l.maniac_loop_info_size == r.maniac_loop_info_size
		&& l.maniac_loop_info == r.maniac_loop_info;
	}

	inline bool operator!=(const SaveEventExecFrame& l, const SaveEventExecFrame& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveEventExecFrame& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveEventExecFrame& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		for (int i = 0; i < static_cast<int>(obj.commands.size()); ++i) {
			const auto ctx1 = Context<SaveEventExecFrame, ParentCtx>{ "commands", i, &obj, parent_ctx };
			ForEachString(obj.commands[i], f, &ctx1);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
