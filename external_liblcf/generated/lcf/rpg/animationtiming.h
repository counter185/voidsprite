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

#ifndef LCF_RPG_ANIMATIONTIMING_H
#define LCF_RPG_ANIMATIONTIMING_H

// Headers
#include <stdint.h>
#include "lcf/enum_tags.h"
#include "lcf/rpg/sound.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::AnimationTiming class.
 */
namespace lcf {
namespace rpg {
	class AnimationTiming {
	public:
		enum FlashScope {
			FlashScope_nothing = 0,
			FlashScope_target = 1,
			FlashScope_screen = 2
		};
		static constexpr auto kFlashScopeTags = lcf::makeEnumTags<FlashScope>(
			"nothing",
			"target",
			"screen"
		);
		enum ScreenShake {
			ScreenShake_nothing = 0,
			ScreenShake_target = 1,
			ScreenShake_screen = 2
		};
		static constexpr auto kScreenShakeTags = lcf::makeEnumTags<ScreenShake>(
			"nothing",
			"target",
			"screen"
		);

		int ID = 0;
		int32_t frame = 0;
		Sound se;
		int32_t flash_scope = 0;
		int32_t flash_red = 31;
		int32_t flash_green = 31;
		int32_t flash_blue = 31;
		int32_t flash_power = 31;
		int32_t screen_shake = 0;
	};
	inline std::ostream& operator<<(std::ostream& os, AnimationTiming::FlashScope code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, AnimationTiming::ScreenShake code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const AnimationTiming& l, const AnimationTiming& r) {
		return l.frame == r.frame
		&& l.se == r.se
		&& l.flash_scope == r.flash_scope
		&& l.flash_red == r.flash_red
		&& l.flash_green == r.flash_green
		&& l.flash_blue == r.flash_blue
		&& l.flash_power == r.flash_power
		&& l.screen_shake == r.screen_shake;
	}

	inline bool operator!=(const AnimationTiming& l, const AnimationTiming& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const AnimationTiming& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(AnimationTiming& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx2 = Context<AnimationTiming, ParentCtx>{ "se", -1, &obj, parent_ctx };
		ForEachString(obj.se, f, &ctx2);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
