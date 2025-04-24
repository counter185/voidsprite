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

#ifndef LCF_RPG_SAVESCREEN_H
#define LCF_RPG_SAVESCREEN_H

// Headers
#include <stdint.h>
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveScreen class.
 */
namespace lcf {
namespace rpg {
	class SaveScreen {
	public:
		int32_t tint_finish_red = 100;
		int32_t tint_finish_green = 100;
		int32_t tint_finish_blue = 100;
		int32_t tint_finish_sat = 100;
		double tint_current_red = 100.0;
		double tint_current_green = 100.0;
		double tint_current_blue = 100.0;
		double tint_current_sat = 100.0;
		int32_t tint_time_left = 0;
		bool flash_continuous = false;
		int32_t flash_red = 0;
		int32_t flash_green = 0;
		int32_t flash_blue = 0;
		double flash_current_level = 0.0;
		int32_t flash_time_left = 0;
		bool shake_continuous = false;
		int32_t shake_strength = 0;
		int32_t shake_speed = 0;
		int32_t shake_position = 0;
		int32_t shake_position_y = 0;
		int32_t shake_time_left = 0;
		int32_t pan_x = 0;
		int32_t pan_y = 0;
		int32_t battleanim_id = 0;
		int32_t battleanim_target = 0;
		int32_t battleanim_frame = 0;
		bool battleanim_active = false;
		bool battleanim_global = false;
		int32_t weather = 0;
		int32_t weather_strength = 0;
	};

	inline bool operator==(const SaveScreen& l, const SaveScreen& r) {
		return l.tint_finish_red == r.tint_finish_red
		&& l.tint_finish_green == r.tint_finish_green
		&& l.tint_finish_blue == r.tint_finish_blue
		&& l.tint_finish_sat == r.tint_finish_sat
		&& l.tint_current_red == r.tint_current_red
		&& l.tint_current_green == r.tint_current_green
		&& l.tint_current_blue == r.tint_current_blue
		&& l.tint_current_sat == r.tint_current_sat
		&& l.tint_time_left == r.tint_time_left
		&& l.flash_continuous == r.flash_continuous
		&& l.flash_red == r.flash_red
		&& l.flash_green == r.flash_green
		&& l.flash_blue == r.flash_blue
		&& l.flash_current_level == r.flash_current_level
		&& l.flash_time_left == r.flash_time_left
		&& l.shake_continuous == r.shake_continuous
		&& l.shake_strength == r.shake_strength
		&& l.shake_speed == r.shake_speed
		&& l.shake_position == r.shake_position
		&& l.shake_position_y == r.shake_position_y
		&& l.shake_time_left == r.shake_time_left
		&& l.pan_x == r.pan_x
		&& l.pan_y == r.pan_y
		&& l.battleanim_id == r.battleanim_id
		&& l.battleanim_target == r.battleanim_target
		&& l.battleanim_frame == r.battleanim_frame
		&& l.battleanim_active == r.battleanim_active
		&& l.battleanim_global == r.battleanim_global
		&& l.weather == r.weather
		&& l.weather_strength == r.weather_strength;
	}

	inline bool operator!=(const SaveScreen& l, const SaveScreen& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveScreen& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveScreen& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
