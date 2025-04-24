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

#ifndef LCF_RPG_SAVEEASYRPGTEXT_H
#define LCF_RPG_SAVEEASYRPGTEXT_H

// Headers
#include <array>
#include <stdint.h>
#include "lcf/dbstring.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveEasyRpgText class.
 */
namespace lcf {
namespace rpg {
	class SaveEasyRpgText {
	public:
		DBString text;
		int32_t position_x = 0;
		int32_t position_y = 0;
		DBString font_name;
		int32_t font_size = 12;
		int32_t letter_spacing = 0;
		int32_t line_spacing = 4;
		struct Flags {
			union {
				struct {
					bool draw_gradient;
					bool draw_shadow;
					bool bold;
					bool italic;
				};
				std::array<bool, 4> flags;
			};
			Flags() noexcept: draw_gradient(true), draw_shadow(true), bold(false), italic(false)
			{}
		} flags;
	};

	inline bool operator==(const SaveEasyRpgText::Flags& l, const SaveEasyRpgText::Flags& r) {
		return l.flags == r.flags;
	}

	inline bool operator!=(const SaveEasyRpgText::Flags& l, const SaveEasyRpgText::Flags& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveEasyRpgText::Flags& obj);

	inline bool operator==(const SaveEasyRpgText& l, const SaveEasyRpgText& r) {
		return l.text == r.text
		&& l.position_x == r.position_x
		&& l.position_y == r.position_y
		&& l.font_name == r.font_name
		&& l.font_size == r.font_size
		&& l.letter_spacing == r.letter_spacing
		&& l.line_spacing == r.line_spacing
		&& l.flags == r.flags;
	}

	inline bool operator!=(const SaveEasyRpgText& l, const SaveEasyRpgText& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveEasyRpgText& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveEasyRpgText& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<SaveEasyRpgText, ParentCtx>{ "text", -1, &obj, parent_ctx };
		f(obj.text, ctx1);
		const auto ctx4 = Context<SaveEasyRpgText, ParentCtx>{ "font_name", -1, &obj, parent_ctx };
		f(obj.font_name, ctx4);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
