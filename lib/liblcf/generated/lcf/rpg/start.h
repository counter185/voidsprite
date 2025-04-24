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

#ifndef LCF_RPG_START_H
#define LCF_RPG_START_H

// Headers
#include <stdint.h>
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Start class.
 */
namespace lcf {
namespace rpg {
	class Start {
	public:
		int32_t party_map_id = 0;
		int32_t party_x = 0;
		int32_t party_y = 0;
		int32_t boat_map_id = 0;
		int32_t boat_x = 0;
		int32_t boat_y = 0;
		int32_t ship_map_id = 0;
		int32_t ship_x = 0;
		int32_t ship_y = 0;
		int32_t airship_map_id = 0;
		int32_t airship_x = 0;
		int32_t airship_y = 0;
	};

	inline bool operator==(const Start& l, const Start& r) {
		return l.party_map_id == r.party_map_id
		&& l.party_x == r.party_x
		&& l.party_y == r.party_y
		&& l.boat_map_id == r.boat_map_id
		&& l.boat_x == r.boat_x
		&& l.boat_y == r.boat_y
		&& l.ship_map_id == r.ship_map_id
		&& l.ship_x == r.ship_x
		&& l.ship_y == r.ship_y
		&& l.airship_map_id == r.airship_map_id
		&& l.airship_x == r.airship_x
		&& l.airship_y == r.airship_y;
	}

	inline bool operator!=(const Start& l, const Start& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Start& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Start& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
