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

#ifndef LCF_RPG_MAPINFO_H
#define LCF_RPG_MAPINFO_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/rpg/encounter.h"
#include "lcf/rpg/music.h"
#include "lcf/rpg/rect.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::MapInfo class.
 */
namespace lcf {
namespace rpg {
	class MapInfo {
	public:
		enum MusicType {
			MusicType_parent = 0,
			MusicType_event = 1,
			MusicType_specific = 2
		};
		static constexpr auto kMusicTypeTags = lcf::makeEnumTags<MusicType>(
			"parent",
			"event",
			"specific"
		);
		enum BGMType {
			BGMType_parent = 0,
			BGMType_terrain = 1,
			BGMType_specific = 2
		};
		static constexpr auto kBGMTypeTags = lcf::makeEnumTags<BGMType>(
			"parent",
			"terrain",
			"specific"
		);
		enum TriState {
			TriState_parent = 0,
			TriState_allow = 1,
			TriState_forbid = 2
		};
		static constexpr auto kTriStateTags = lcf::makeEnumTags<TriState>(
			"parent",
			"allow",
			"forbid"
		);

		int ID = 0;
		DBString name;
		int32_t parent_map = 0;
		int32_t indentation = 0;
		int32_t type = -1;
		int32_t scrollbar_x = 0;
		int32_t scrollbar_y = 0;
		bool expanded_node = false;
		int32_t music_type = 0;
		Music music;
		int32_t background_type = 0;
		DBString background_name;
		int32_t teleport = 0;
		int32_t escape = 0;
		int32_t save = 0;
		std::vector<Encounter> encounters;
		int32_t encounter_steps = 25;
		Rect area_rect;
	};
	inline std::ostream& operator<<(std::ostream& os, MapInfo::MusicType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, MapInfo::BGMType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, MapInfo::TriState code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const MapInfo& l, const MapInfo& r) {
		return l.name == r.name
		&& l.parent_map == r.parent_map
		&& l.indentation == r.indentation
		&& l.type == r.type
		&& l.scrollbar_x == r.scrollbar_x
		&& l.scrollbar_y == r.scrollbar_y
		&& l.expanded_node == r.expanded_node
		&& l.music_type == r.music_type
		&& l.music == r.music
		&& l.background_type == r.background_type
		&& l.background_name == r.background_name
		&& l.teleport == r.teleport
		&& l.escape == r.escape
		&& l.save == r.save
		&& l.encounters == r.encounters
		&& l.encounter_steps == r.encounter_steps
		&& l.area_rect == r.area_rect;
	}

	inline bool operator!=(const MapInfo& l, const MapInfo& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const MapInfo& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(MapInfo& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<MapInfo, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		const auto ctx9 = Context<MapInfo, ParentCtx>{ "music", -1, &obj, parent_ctx };
		ForEachString(obj.music, f, &ctx9);
		const auto ctx11 = Context<MapInfo, ParentCtx>{ "background_name", -1, &obj, parent_ctx };
		f(obj.background_name, ctx11);
		for (int i = 0; i < static_cast<int>(obj.encounters.size()); ++i) {
			const auto ctx15 = Context<MapInfo, ParentCtx>{ "encounters", i, &obj, parent_ctx };
			ForEachString(obj.encounters[i], f, &ctx15);
		}
		const auto ctx17 = Context<MapInfo, ParentCtx>{ "area_rect", -1, &obj, parent_ctx };
		ForEachString(obj.area_rect, f, &ctx17);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
