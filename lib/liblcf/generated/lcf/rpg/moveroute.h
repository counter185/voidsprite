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

#ifndef LCF_RPG_MOVEROUTE_H
#define LCF_RPG_MOVEROUTE_H

// Headers
#include <vector>
#include "lcf/rpg/movecommand.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::MoveRoute class.
 */
namespace lcf {
namespace rpg {
	class MoveRoute {
	public:
		std::vector<MoveCommand> move_commands;
		bool repeat = true;
		bool skippable = false;
	};

	inline bool operator==(const MoveRoute& l, const MoveRoute& r) {
		return l.move_commands == r.move_commands
		&& l.repeat == r.repeat
		&& l.skippable == r.skippable;
	}

	inline bool operator!=(const MoveRoute& l, const MoveRoute& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const MoveRoute& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(MoveRoute& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		for (int i = 0; i < static_cast<int>(obj.move_commands.size()); ++i) {
			const auto ctx1 = Context<MoveRoute, ParentCtx>{ "move_commands", i, &obj, parent_ctx };
			ForEachString(obj.move_commands[i], f, &ctx1);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
