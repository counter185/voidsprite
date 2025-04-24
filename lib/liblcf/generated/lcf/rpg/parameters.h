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

#ifndef LCF_RPG_PARAMETERS_H
#define LCF_RPG_PARAMETERS_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Parameters class.
 */
namespace lcf {
namespace rpg {
	class Parameters {
	public:
		void Setup(int final_level);
		std::vector<int16_t> maxhp;
		std::vector<int16_t> maxsp;
		std::vector<int16_t> attack;
		std::vector<int16_t> defense;
		std::vector<int16_t> spirit;
		std::vector<int16_t> agility;
	};

	inline bool operator==(const Parameters& l, const Parameters& r) {
		return l.maxhp == r.maxhp
		&& l.maxsp == r.maxsp
		&& l.attack == r.attack
		&& l.defense == r.defense
		&& l.spirit == r.spirit
		&& l.agility == r.agility;
	}

	inline bool operator!=(const Parameters& l, const Parameters& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Parameters& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Parameters& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
