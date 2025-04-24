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

#ifndef LCF_RPG_CLASS_H
#define LCF_RPG_CLASS_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/rpg/learning.h"
#include "lcf/rpg/parameters.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Class class.
 */
namespace lcf {
namespace rpg {
	class Class {
	public:
		int ID = 0;
		DBString name;
		bool two_weapon = false;
		bool lock_equipment = false;
		bool auto_battle = false;
		bool super_guard = false;
		Parameters parameters;
		int32_t exp_base = 300;
		int32_t exp_inflation = 300;
		int32_t exp_correction = 0;
		int32_t battler_animation = 0;
		std::vector<Learning> skills;
		std::vector<uint8_t> state_ranks;
		std::vector<uint8_t> attribute_ranks;
		std::vector<int32_t> battle_commands;
	};

	inline bool operator==(const Class& l, const Class& r) {
		return l.name == r.name
		&& l.two_weapon == r.two_weapon
		&& l.lock_equipment == r.lock_equipment
		&& l.auto_battle == r.auto_battle
		&& l.super_guard == r.super_guard
		&& l.parameters == r.parameters
		&& l.exp_base == r.exp_base
		&& l.exp_inflation == r.exp_inflation
		&& l.exp_correction == r.exp_correction
		&& l.battler_animation == r.battler_animation
		&& l.skills == r.skills
		&& l.state_ranks == r.state_ranks
		&& l.attribute_ranks == r.attribute_ranks
		&& l.battle_commands == r.battle_commands;
	}

	inline bool operator!=(const Class& l, const Class& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Class& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Class& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Class, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		const auto ctx6 = Context<Class, ParentCtx>{ "parameters", -1, &obj, parent_ctx };
		ForEachString(obj.parameters, f, &ctx6);
		for (int i = 0; i < static_cast<int>(obj.skills.size()); ++i) {
			const auto ctx11 = Context<Class, ParentCtx>{ "skills", i, &obj, parent_ctx };
			ForEachString(obj.skills[i], f, &ctx11);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
