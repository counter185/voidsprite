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

#ifndef LCF_RPG_SAVEMAPINFO_H
#define LCF_RPG_SAVEMAPINFO_H

// Headers
#include <stdint.h>
#include <string>
#include <vector>
#include "lcf/rpg/savemapevent.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveMapInfo class.
 */
namespace lcf {
namespace rpg {
	class SaveMapInfo {
	public:
		int32_t position_x = 0;
		int32_t position_y = 0;
		int32_t encounter_steps = -1;
		int32_t chipset_id = 0;
		std::vector<SaveMapEvent> events;
		std::vector<uint8_t> lower_tiles = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143};
		std::vector<uint8_t> upper_tiles = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143};
		std::string parallax_name;
		bool parallax_horz = false;
		bool parallax_vert = false;
		bool parallax_horz_auto = false;
		int32_t parallax_horz_speed = 0;
		bool parallax_vert_auto = false;
		int32_t parallax_vert_speed = 0;
	};

	inline bool operator==(const SaveMapInfo& l, const SaveMapInfo& r) {
		return l.position_x == r.position_x
		&& l.position_y == r.position_y
		&& l.encounter_steps == r.encounter_steps
		&& l.chipset_id == r.chipset_id
		&& l.events == r.events
		&& l.lower_tiles == r.lower_tiles
		&& l.upper_tiles == r.upper_tiles
		&& l.parallax_name == r.parallax_name
		&& l.parallax_horz == r.parallax_horz
		&& l.parallax_vert == r.parallax_vert
		&& l.parallax_horz_auto == r.parallax_horz_auto
		&& l.parallax_horz_speed == r.parallax_horz_speed
		&& l.parallax_vert_auto == r.parallax_vert_auto
		&& l.parallax_vert_speed == r.parallax_vert_speed;
	}

	inline bool operator!=(const SaveMapInfo& l, const SaveMapInfo& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveMapInfo& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveMapInfo& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		for (int i = 0; i < static_cast<int>(obj.events.size()); ++i) {
			const auto ctx5 = Context<SaveMapInfo, ParentCtx>{ "events", i, &obj, parent_ctx };
			ForEachString(obj.events[i], f, &ctx5);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
