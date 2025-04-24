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

#ifndef LCF_RPG_BATTLECOMMAND_H
#define LCF_RPG_BATTLECOMMAND_H

// Headers
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::BattleCommand class.
 */
namespace lcf {
namespace rpg {
	class BattleCommand {
	public:
		enum Type {
			Type_attack = 0,
			Type_skill = 1,
			Type_subskill = 2,
			Type_defense = 3,
			Type_item = 4,
			Type_escape = 5,
			Type_special = 6
		};
		static constexpr auto kTypeTags = lcf::makeEnumTags<Type>(
			"attack",
			"skill",
			"subskill",
			"defense",
			"item",
			"escape",
			"special"
		);

		int ID = 0;
		DBString name;
		int32_t type = 0;
	};
	inline std::ostream& operator<<(std::ostream& os, BattleCommand::Type code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const BattleCommand& l, const BattleCommand& r) {
		return l.name == r.name
		&& l.type == r.type;
	}

	inline bool operator!=(const BattleCommand& l, const BattleCommand& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const BattleCommand& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(BattleCommand& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<BattleCommand, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
