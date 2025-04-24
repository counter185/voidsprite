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

#ifndef LCF_RPG_ANIMATIONFRAME_H
#define LCF_RPG_ANIMATIONFRAME_H

// Headers
#include <vector>
#include "lcf/rpg/animationcelldata.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::AnimationFrame class.
 */
namespace lcf {
namespace rpg {
	class AnimationFrame {
	public:
		int ID = 0;
		std::vector<AnimationCellData> cells;
	};

	inline bool operator==(const AnimationFrame& l, const AnimationFrame& r) {
		return l.cells == r.cells;
	}

	inline bool operator!=(const AnimationFrame& l, const AnimationFrame& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const AnimationFrame& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(AnimationFrame& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		for (int i = 0; i < static_cast<int>(obj.cells.size()); ++i) {
			const auto ctx1 = Context<AnimationFrame, ParentCtx>{ "cells", i, &obj, parent_ctx };
			ForEachString(obj.cells[i], f, &ctx1);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
