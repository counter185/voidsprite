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

#ifndef LCF_RPG_SAVEINVENTORY_H
#define LCF_RPG_SAVEINVENTORY_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveInventory class.
 */
namespace lcf {
namespace rpg {
	class SaveInventory {
	public:
		std::vector<int16_t> party;
		std::vector<int16_t> item_ids;
		std::vector<uint8_t> item_counts;
		std::vector<uint8_t> item_usage;
		int32_t gold = 0;
		int32_t timer1_frames = 0;
		bool timer1_active = false;
		bool timer1_visible = false;
		bool timer1_battle = false;
		int32_t timer2_frames = 0;
		bool timer2_active = false;
		bool timer2_visible = false;
		bool timer2_battle = false;
		int32_t battles = 0;
		int32_t defeats = 0;
		int32_t escapes = 0;
		int32_t victories = 0;
		int32_t turns = 0;
		int32_t steps = 0;
	};

	inline bool operator==(const SaveInventory& l, const SaveInventory& r) {
		return l.party == r.party
		&& l.item_ids == r.item_ids
		&& l.item_counts == r.item_counts
		&& l.item_usage == r.item_usage
		&& l.gold == r.gold
		&& l.timer1_frames == r.timer1_frames
		&& l.timer1_active == r.timer1_active
		&& l.timer1_visible == r.timer1_visible
		&& l.timer1_battle == r.timer1_battle
		&& l.timer2_frames == r.timer2_frames
		&& l.timer2_active == r.timer2_active
		&& l.timer2_visible == r.timer2_visible
		&& l.timer2_battle == r.timer2_battle
		&& l.battles == r.battles
		&& l.defeats == r.defeats
		&& l.escapes == r.escapes
		&& l.victories == r.victories
		&& l.turns == r.turns
		&& l.steps == r.steps;
	}

	inline bool operator!=(const SaveInventory& l, const SaveInventory& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveInventory& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveInventory& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
