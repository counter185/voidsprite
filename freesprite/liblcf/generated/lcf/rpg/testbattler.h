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

#ifndef LCF_RPG_TESTBATTLER_H
#define LCF_RPG_TESTBATTLER_H

// Headers
#include <stdint.h>
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::TestBattler class.
 */
namespace lcf {
namespace rpg {
	class TestBattler {
	public:
		int ID = 0;
		int32_t actor_id = 1;
		int32_t level = 1;
		int32_t weapon_id = 0;
		int32_t shield_id = 0;
		int32_t armor_id = 0;
		int32_t helmet_id = 0;
		int32_t accessory_id = 0;
	};

	inline bool operator==(const TestBattler& l, const TestBattler& r) {
		return l.actor_id == r.actor_id
		&& l.level == r.level
		&& l.weapon_id == r.weapon_id
		&& l.shield_id == r.shield_id
		&& l.armor_id == r.armor_id
		&& l.helmet_id == r.helmet_id
		&& l.accessory_id == r.accessory_id;
	}

	inline bool operator!=(const TestBattler& l, const TestBattler& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const TestBattler& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(TestBattler& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
