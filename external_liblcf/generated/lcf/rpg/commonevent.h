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

#ifndef LCF_RPG_COMMONEVENT_H
#define LCF_RPG_COMMONEVENT_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/rpg/eventcommand.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::CommonEvent class.
 */
namespace lcf {
namespace rpg {
	class CommonEvent {
	public:
		enum Trigger {
			Trigger_automatic = 3,
			Trigger_parallel = 4,
			Trigger_call = 5
		};
		static constexpr auto kTriggerTags = lcf::EnumTags{
			Trigger_automatic, "automatic",
			Trigger_parallel, "parallel",
			Trigger_call, "call"
		};

		int ID = 0;
		DBString name;
		int32_t trigger = 0;
		bool switch_flag = false;
		int32_t switch_id = 1;
		std::vector<EventCommand> event_commands;
	};
	inline std::ostream& operator<<(std::ostream& os, CommonEvent::Trigger code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const CommonEvent& l, const CommonEvent& r) {
		return l.name == r.name
		&& l.trigger == r.trigger
		&& l.switch_flag == r.switch_flag
		&& l.switch_id == r.switch_id
		&& l.event_commands == r.event_commands;
	}

	inline bool operator!=(const CommonEvent& l, const CommonEvent& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const CommonEvent& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(CommonEvent& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<CommonEvent, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		for (int i = 0; i < static_cast<int>(obj.event_commands.size()); ++i) {
			const auto ctx5 = Context<CommonEvent, ParentCtx>{ "event_commands", i, &obj, parent_ctx };
			ForEachString(obj.event_commands[i], f, &ctx5);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
