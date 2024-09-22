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

#ifndef LCF_RPG_SAVEPANORAMA_H
#define LCF_RPG_SAVEPANORAMA_H

// Headers
#include <stdint.h>
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SavePanorama class.
 */
namespace lcf {
namespace rpg {
	class SavePanorama {
	public:
		int32_t pan_x = 0;
		int32_t pan_y = 0;
	};

	inline bool operator==(const SavePanorama& l, const SavePanorama& r) {
		return l.pan_x == r.pan_x
		&& l.pan_y == r.pan_y;
	}

	inline bool operator!=(const SavePanorama& l, const SavePanorama& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SavePanorama& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SavePanorama& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
