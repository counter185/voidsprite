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

#ifndef LCF_RPG_TROOPPAGE_H
#define LCF_RPG_TROOPPAGE_H

// Headers
#include <vector>
#include "lcf/rpg/eventcommand.h"
#include "lcf/rpg/trooppagecondition.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::TroopPage class.
 */
namespace lcf {
namespace rpg {
	class TroopPage {
	public:
		int ID = 0;
		TroopPageCondition condition;
		std::vector<EventCommand> event_commands;
	};

	inline bool operator==(const TroopPage& l, const TroopPage& r) {
		return l.condition == r.condition
		&& l.event_commands == r.event_commands;
	}

	inline bool operator!=(const TroopPage& l, const TroopPage& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const TroopPage& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(TroopPage& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<TroopPage, ParentCtx>{ "condition", -1, &obj, parent_ctx };
		ForEachString(obj.condition, f, &ctx1);
		for (int i = 0; i < static_cast<int>(obj.event_commands.size()); ++i) {
			const auto ctx2 = Context<TroopPage, ParentCtx>{ "event_commands", i, &obj, parent_ctx };
			ForEachString(obj.event_commands[i], f, &ctx2);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
