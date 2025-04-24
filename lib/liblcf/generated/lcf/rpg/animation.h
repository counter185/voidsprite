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

#ifndef LCF_RPG_ANIMATION_H
#define LCF_RPG_ANIMATION_H

// Headers
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/rpg/animationframe.h"
#include "lcf/rpg/animationtiming.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Animation class.
 */
namespace lcf {
namespace rpg {
	class Animation {
	public:
		enum Scope {
			Scope_target = 0,
			Scope_screen = 1
		};
		static constexpr auto kScopeTags = lcf::makeEnumTags<Scope>(
			"target",
			"screen"
		);
		enum Position {
			Position_up = 0,
			Position_middle = 1,
			Position_down = 2
		};
		static constexpr auto kPositionTags = lcf::makeEnumTags<Position>(
			"up",
			"middle",
			"down"
		);

		int ID = 0;
		DBString name;
		DBString animation_name;
		bool large = false;
		std::vector<AnimationTiming> timings;
		int32_t scope = 0;
		int32_t position = 2;
		std::vector<AnimationFrame> frames;
	};
	inline std::ostream& operator<<(std::ostream& os, Animation::Scope code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, Animation::Position code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const Animation& l, const Animation& r) {
		return l.name == r.name
		&& l.animation_name == r.animation_name
		&& l.large == r.large
		&& l.timings == r.timings
		&& l.scope == r.scope
		&& l.position == r.position
		&& l.frames == r.frames;
	}

	inline bool operator!=(const Animation& l, const Animation& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Animation& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Animation& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Animation, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		const auto ctx2 = Context<Animation, ParentCtx>{ "animation_name", -1, &obj, parent_ctx };
		f(obj.animation_name, ctx2);
		for (int i = 0; i < static_cast<int>(obj.timings.size()); ++i) {
			const auto ctx4 = Context<Animation, ParentCtx>{ "timings", i, &obj, parent_ctx };
			ForEachString(obj.timings[i], f, &ctx4);
		}
		for (int i = 0; i < static_cast<int>(obj.frames.size()); ++i) {
			const auto ctx7 = Context<Animation, ParentCtx>{ "frames", i, &obj, parent_ctx };
			ForEachString(obj.frames[i], f, &ctx7);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
