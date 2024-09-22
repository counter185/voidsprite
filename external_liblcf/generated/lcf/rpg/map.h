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

#ifndef LCF_RPG_MAP_H
#define LCF_RPG_MAP_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/rpg/event.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Map class.
 */
namespace lcf {
namespace rpg {
	class Map {
	public:
		std::string lmu_header;
		enum ScrollType {
			ScrollType_none = 0,
			ScrollType_vertical = 1,
			ScrollType_horizontal = 2,
			ScrollType_both = 3
		};
		static constexpr auto kScrollTypeTags = lcf::makeEnumTags<ScrollType>(
			"none",
			"vertical",
			"horizontal",
			"both"
		);
		enum GeneratorMode {
			GeneratorMode_single_passage = 0,
			GeneratorMode_linked_rooms = 1,
			GeneratorMode_maze_passage = 2,
			GeneratorMode_open_room = 3
		};
		static constexpr auto kGeneratorModeTags = lcf::makeEnumTags<GeneratorMode>(
			"single_passage",
			"linked_rooms",
			"maze_passage",
			"open_room"
		);
		enum GeneratorTiles {
			GeneratorTiles_one = 0,
			GeneratorTiles_two = 1
		};
		static constexpr auto kGeneratorTilesTags = lcf::makeEnumTags<GeneratorTiles>(
			"one",
			"two"
		);

		int32_t chipset_id = 1;
		int32_t width = 20;
		int32_t height = 15;
		int32_t scroll_type = 0;
		bool parallax_flag = false;
		DBString parallax_name;
		bool parallax_loop_x = false;
		bool parallax_loop_y = false;
		bool parallax_auto_loop_x = false;
		int32_t parallax_sx = 0;
		bool parallax_auto_loop_y = false;
		int32_t parallax_sy = 0;
		bool generator_flag = false;
		int32_t generator_mode = 0;
		bool top_level = false;
		int32_t generator_tiles = 0;
		int32_t generator_width = 4;
		int32_t generator_height = 1;
		bool generator_surround = true;
		bool generator_upper_wall = true;
		bool generator_floor_b = true;
		bool generator_floor_c = true;
		bool generator_extra_b = true;
		bool generator_extra_c = true;
		std::vector<uint32_t> generator_x;
		std::vector<uint32_t> generator_y;
		std::vector<int16_t> generator_tile_ids;
		std::vector<int16_t> lower_layer;
		std::vector<int16_t> upper_layer;
		std::vector<Event> events;
		int32_t save_count_2k3e = 0;
		int32_t save_count = 0;
	};
	inline std::ostream& operator<<(std::ostream& os, Map::ScrollType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, Map::GeneratorMode code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, Map::GeneratorTiles code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const Map& l, const Map& r) {
		return l.chipset_id == r.chipset_id
		&& l.width == r.width
		&& l.height == r.height
		&& l.scroll_type == r.scroll_type
		&& l.parallax_flag == r.parallax_flag
		&& l.parallax_name == r.parallax_name
		&& l.parallax_loop_x == r.parallax_loop_x
		&& l.parallax_loop_y == r.parallax_loop_y
		&& l.parallax_auto_loop_x == r.parallax_auto_loop_x
		&& l.parallax_sx == r.parallax_sx
		&& l.parallax_auto_loop_y == r.parallax_auto_loop_y
		&& l.parallax_sy == r.parallax_sy
		&& l.generator_flag == r.generator_flag
		&& l.generator_mode == r.generator_mode
		&& l.top_level == r.top_level
		&& l.generator_tiles == r.generator_tiles
		&& l.generator_width == r.generator_width
		&& l.generator_height == r.generator_height
		&& l.generator_surround == r.generator_surround
		&& l.generator_upper_wall == r.generator_upper_wall
		&& l.generator_floor_b == r.generator_floor_b
		&& l.generator_floor_c == r.generator_floor_c
		&& l.generator_extra_b == r.generator_extra_b
		&& l.generator_extra_c == r.generator_extra_c
		&& l.generator_x == r.generator_x
		&& l.generator_y == r.generator_y
		&& l.generator_tile_ids == r.generator_tile_ids
		&& l.lower_layer == r.lower_layer
		&& l.upper_layer == r.upper_layer
		&& l.events == r.events
		&& l.save_count_2k3e == r.save_count_2k3e
		&& l.save_count == r.save_count;
	}

	inline bool operator!=(const Map& l, const Map& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Map& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Map& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx6 = Context<Map, ParentCtx>{ "parallax_name", -1, &obj, parent_ctx };
		f(obj.parallax_name, ctx6);
		for (int i = 0; i < static_cast<int>(obj.events.size()); ++i) {
			const auto ctx30 = Context<Map, ParentCtx>{ "events", i, &obj, parent_ctx };
			ForEachString(obj.events[i], f, &ctx30);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
