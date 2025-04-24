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

#ifndef LCF_RPG_TROOP_H
#define LCF_RPG_TROOP_H

// Headers
#include <vector>
#include "lcf/dbbitarray.h"
#include "lcf/dbstring.h"
#include "lcf/rpg/troopmember.h"
#include "lcf/rpg/trooppage.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Troop class.
 */
namespace lcf {
namespace rpg {
	class Troop {
	public:
		int ID = 0;
		DBString name;
		std::vector<TroopMember> members;
		bool auto_alignment = false;
		DBBitArray terrain_set;
		bool appear_randomly = false;
		std::vector<TroopPage> pages;
	};

	inline bool operator==(const Troop& l, const Troop& r) {
		return l.name == r.name
		&& l.members == r.members
		&& l.auto_alignment == r.auto_alignment
		&& l.terrain_set == r.terrain_set
		&& l.appear_randomly == r.appear_randomly
		&& l.pages == r.pages;
	}

	inline bool operator!=(const Troop& l, const Troop& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Troop& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Troop& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Troop, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		for (int i = 0; i < static_cast<int>(obj.members.size()); ++i) {
			const auto ctx2 = Context<Troop, ParentCtx>{ "members", i, &obj, parent_ctx };
			ForEachString(obj.members[i], f, &ctx2);
		}
		for (int i = 0; i < static_cast<int>(obj.pages.size()); ++i) {
			const auto ctx6 = Context<Troop, ParentCtx>{ "pages", i, &obj, parent_ctx };
			ForEachString(obj.pages[i], f, &ctx6);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
