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

#ifndef LCF_RPG_SAVECOMMONEVENT_H
#define LCF_RPG_SAVECOMMONEVENT_H

// Headers
#include "lcf/rpg/saveeventexecstate.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveCommonEvent class.
 */
namespace lcf {
namespace rpg {
	class SaveCommonEvent {
	public:
		int ID = 0;
		SaveEventExecState parallel_event_execstate;
	};

	inline bool operator==(const SaveCommonEvent& l, const SaveCommonEvent& r) {
		return l.parallel_event_execstate == r.parallel_event_execstate;
	}

	inline bool operator!=(const SaveCommonEvent& l, const SaveCommonEvent& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveCommonEvent& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveCommonEvent& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<SaveCommonEvent, ParentCtx>{ "parallel_event_execstate", -1, &obj, parent_ctx };
		ForEachString(obj.parallel_event_execstate, f, &ctx1);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
