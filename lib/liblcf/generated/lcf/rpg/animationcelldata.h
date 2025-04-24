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

#ifndef LCF_RPG_ANIMATIONCELLDATA_H
#define LCF_RPG_ANIMATIONCELLDATA_H

// Headers
#include <stdint.h>
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::AnimationCellData class.
 */
namespace lcf {
namespace rpg {
	class AnimationCellData {
	public:
		int ID = 0;
		int32_t valid = 1;
		int32_t cell_id = 0;
		int32_t x = 0;
		int32_t y = 0;
		int32_t zoom = 100;
		int32_t tone_red = 100;
		int32_t tone_green = 100;
		int32_t tone_blue = 100;
		int32_t tone_gray = 100;
		int32_t transparency = 0;
	};

	inline bool operator==(const AnimationCellData& l, const AnimationCellData& r) {
		return l.valid == r.valid
		&& l.cell_id == r.cell_id
		&& l.x == r.x
		&& l.y == r.y
		&& l.zoom == r.zoom
		&& l.tone_red == r.tone_red
		&& l.tone_green == r.tone_green
		&& l.tone_blue == r.tone_blue
		&& l.tone_gray == r.tone_gray
		&& l.transparency == r.transparency;
	}

	inline bool operator!=(const AnimationCellData& l, const AnimationCellData& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const AnimationCellData& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(AnimationCellData& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
