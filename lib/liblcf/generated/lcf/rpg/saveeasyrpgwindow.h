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

#ifndef LCF_RPG_SAVEEASYRPGWINDOW_H
#define LCF_RPG_SAVEEASYRPGWINDOW_H

// Headers
#include <array>
#include <stdint.h>
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/rpg/saveeasyrpgtext.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveEasyRpgWindow class.
 */
namespace lcf {
namespace rpg {
	class SaveEasyRpgWindow {
	public:
		int ID = 0;
		std::vector<SaveEasyRpgText> texts;
		int32_t width = 0;
		int32_t height = 0;
		DBString system_name;
		int32_t message_stretch = 0;
		struct Flags {
			union {
				struct {
					bool draw_frame;
					bool border_margin;
				};
				std::array<bool, 2> flags;
			};
			Flags() noexcept: draw_frame(true), border_margin(true)
			{}
		} flags;
	};

	inline bool operator==(const SaveEasyRpgWindow::Flags& l, const SaveEasyRpgWindow::Flags& r) {
		return l.flags == r.flags;
	}

	inline bool operator!=(const SaveEasyRpgWindow::Flags& l, const SaveEasyRpgWindow::Flags& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveEasyRpgWindow::Flags& obj);

	inline bool operator==(const SaveEasyRpgWindow& l, const SaveEasyRpgWindow& r) {
		return l.texts == r.texts
		&& l.width == r.width
		&& l.height == r.height
		&& l.system_name == r.system_name
		&& l.message_stretch == r.message_stretch
		&& l.flags == r.flags;
	}

	inline bool operator!=(const SaveEasyRpgWindow& l, const SaveEasyRpgWindow& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveEasyRpgWindow& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveEasyRpgWindow& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		for (int i = 0; i < static_cast<int>(obj.texts.size()); ++i) {
			const auto ctx1 = Context<SaveEasyRpgWindow, ParentCtx>{ "texts", i, &obj, parent_ctx };
			ForEachString(obj.texts[i], f, &ctx1);
		}
		const auto ctx4 = Context<SaveEasyRpgWindow, ParentCtx>{ "system_name", -1, &obj, parent_ctx };
		f(obj.system_name, ctx4);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
