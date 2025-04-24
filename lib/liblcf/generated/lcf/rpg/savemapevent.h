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

#ifndef LCF_RPG_SAVEMAPEVENT_H
#define LCF_RPG_SAVEMAPEVENT_H

// Headers
#include "lcf/rpg/savemapeventbase.h"
#include <stdint.h>
#include "lcf/rpg/saveeventexecstate.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveMapEvent class.
 */
namespace lcf {
namespace rpg {
	class SaveMapEvent : public SaveMapEventBase {
	public:
		int ID = 0;
		bool waiting_execution = false;
		int32_t original_move_route_index = 0;
		bool triggered_by_decision_key = false;
		SaveEventExecState parallel_event_execstate;
	};

	inline bool operator==(const SaveMapEvent& l, const SaveMapEvent& r) {
		return l.waiting_execution == r.waiting_execution
		&& l.original_move_route_index == r.original_move_route_index
		&& l.triggered_by_decision_key == r.triggered_by_decision_key
		&& l.parallel_event_execstate == r.parallel_event_execstate;
	}

	inline bool operator!=(const SaveMapEvent& l, const SaveMapEvent& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveMapEvent& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveMapEvent& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx4 = Context<SaveMapEvent, ParentCtx>{ "parallel_event_execstate", -1, &obj, parent_ctx };
		ForEachString(obj.parallel_event_execstate, f, &ctx4);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
