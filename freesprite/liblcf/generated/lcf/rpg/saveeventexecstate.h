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

#ifndef LCF_RPG_SAVEEVENTEXECSTATE_H
#define LCF_RPG_SAVEEVENTEXECSTATE_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/rpg/saveeventexecframe.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveEventExecState class.
 */
namespace lcf {
namespace rpg {
	class SaveEventExecState {
	public:
		std::vector<SaveEventExecFrame> stack;
		bool show_message = false;
		bool abort_on_escape = false;
		bool wait_movement = false;
		bool keyinput_wait = false;
		uint8_t keyinput_variable = 0;
		bool keyinput_all_directions = false;
		int32_t keyinput_decision = 0;
		int32_t keyinput_cancel = 0;
		int32_t keyinput_2kshift_2k3numbers = 0;
		int32_t keyinput_2kdown_2k3operators = 0;
		int32_t keyinput_2kleft_2k3shift = 0;
		int32_t keyinput_2kright = 0;
		int32_t keyinput_2kup = 0;
		int32_t wait_time = 0;
		int32_t keyinput_time_variable = 0;
		int32_t keyinput_2k3down = 0;
		int32_t keyinput_2k3left = 0;
		int32_t keyinput_2k3right = 0;
		int32_t keyinput_2k3up = 0;
		bool keyinput_timed = false;
		bool wait_key_enter = false;
		bool easyrpg_active = false;
		DBString easyrpg_string;
		std::vector<int32_t> easyrpg_parameters;
	};

	inline bool operator==(const SaveEventExecState& l, const SaveEventExecState& r) {
		return l.stack == r.stack
		&& l.show_message == r.show_message
		&& l.abort_on_escape == r.abort_on_escape
		&& l.wait_movement == r.wait_movement
		&& l.keyinput_wait == r.keyinput_wait
		&& l.keyinput_variable == r.keyinput_variable
		&& l.keyinput_all_directions == r.keyinput_all_directions
		&& l.keyinput_decision == r.keyinput_decision
		&& l.keyinput_cancel == r.keyinput_cancel
		&& l.keyinput_2kshift_2k3numbers == r.keyinput_2kshift_2k3numbers
		&& l.keyinput_2kdown_2k3operators == r.keyinput_2kdown_2k3operators
		&& l.keyinput_2kleft_2k3shift == r.keyinput_2kleft_2k3shift
		&& l.keyinput_2kright == r.keyinput_2kright
		&& l.keyinput_2kup == r.keyinput_2kup
		&& l.wait_time == r.wait_time
		&& l.keyinput_time_variable == r.keyinput_time_variable
		&& l.keyinput_2k3down == r.keyinput_2k3down
		&& l.keyinput_2k3left == r.keyinput_2k3left
		&& l.keyinput_2k3right == r.keyinput_2k3right
		&& l.keyinput_2k3up == r.keyinput_2k3up
		&& l.keyinput_timed == r.keyinput_timed
		&& l.wait_key_enter == r.wait_key_enter
		&& l.easyrpg_active == r.easyrpg_active
		&& l.easyrpg_string == r.easyrpg_string
		&& l.easyrpg_parameters == r.easyrpg_parameters;
	}

	inline bool operator!=(const SaveEventExecState& l, const SaveEventExecState& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveEventExecState& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveEventExecState& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		for (int i = 0; i < static_cast<int>(obj.stack.size()); ++i) {
			const auto ctx1 = Context<SaveEventExecState, ParentCtx>{ "stack", i, &obj, parent_ctx };
			ForEachString(obj.stack[i], f, &ctx1);
		}
		const auto ctx24 = Context<SaveEventExecState, ParentCtx>{ "easyrpg_string", -1, &obj, parent_ctx };
		f(obj.easyrpg_string, ctx24);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
