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

#ifndef LCF_RPG_TERRAIN_H
#define LCF_RPG_TERRAIN_H

// Headers
#include <array>
#include <stdint.h>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/rpg/sound.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Terrain class.
 */
namespace lcf {
namespace rpg {
	class Terrain {
	public:
		enum BushDepth {
			BushDepth_normal = 0,
			BushDepth_third = 1,
			BushDepth_half = 2,
			BushDepth_full = 3
		};
		static constexpr auto kBushDepthTags = lcf::makeEnumTags<BushDepth>(
			"normal",
			"third",
			"half",
			"full"
		);
		enum BGAssociation {
			BGAssociation_background = 0,
			BGAssociation_frame = 1
		};
		static constexpr auto kBGAssociationTags = lcf::makeEnumTags<BGAssociation>(
			"background",
			"frame"
		);

		int ID = 0;
		DBString name;
		int32_t damage = 0;
		int32_t encounter_rate = 100;
		DBString background_name;
		bool boat_pass = false;
		bool ship_pass = false;
		bool airship_pass = true;
		bool airship_land = true;
		int32_t bush_depth = 0;
		Sound footstep;
		bool on_damage_se = false;
		int32_t background_type = 0;
		DBString background_a_name;
		bool background_a_scrollh = false;
		bool background_a_scrollv = false;
		int32_t background_a_scrollh_speed = 0;
		int32_t background_a_scrollv_speed = 0;
		bool background_b = false;
		DBString background_b_name;
		bool background_b_scrollh = false;
		bool background_b_scrollv = false;
		int32_t background_b_scrollh_speed = 0;
		int32_t background_b_scrollv_speed = 0;
		struct Flags {
			union {
				struct {
					bool back_party;
					bool back_enemies;
					bool lateral_party;
					bool lateral_enemies;
				};
				std::array<bool, 4> flags;
			};
			Flags() noexcept: back_party(false), back_enemies(false), lateral_party(false), lateral_enemies(false)
			{}
		} special_flags;
		int32_t special_back_party = 15;
		int32_t special_back_enemies = 10;
		int32_t special_lateral_party = 10;
		int32_t special_lateral_enemies = 5;
		int32_t grid_location = 0;
		int32_t grid_top_y = 120;
		int32_t grid_elongation = 392;
		int32_t grid_inclination = 16000;
		bool easyrpg_damage_in_percent = false;
		bool easyrpg_damage_can_kill = false;
	};
	inline std::ostream& operator<<(std::ostream& os, Terrain::BushDepth code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, Terrain::BGAssociation code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const Terrain::Flags& l, const Terrain::Flags& r) {
		return l.flags == r.flags;
	}

	inline bool operator!=(const Terrain::Flags& l, const Terrain::Flags& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Terrain::Flags& obj);

	inline bool operator==(const Terrain& l, const Terrain& r) {
		return l.name == r.name
		&& l.damage == r.damage
		&& l.encounter_rate == r.encounter_rate
		&& l.background_name == r.background_name
		&& l.boat_pass == r.boat_pass
		&& l.ship_pass == r.ship_pass
		&& l.airship_pass == r.airship_pass
		&& l.airship_land == r.airship_land
		&& l.bush_depth == r.bush_depth
		&& l.footstep == r.footstep
		&& l.on_damage_se == r.on_damage_se
		&& l.background_type == r.background_type
		&& l.background_a_name == r.background_a_name
		&& l.background_a_scrollh == r.background_a_scrollh
		&& l.background_a_scrollv == r.background_a_scrollv
		&& l.background_a_scrollh_speed == r.background_a_scrollh_speed
		&& l.background_a_scrollv_speed == r.background_a_scrollv_speed
		&& l.background_b == r.background_b
		&& l.background_b_name == r.background_b_name
		&& l.background_b_scrollh == r.background_b_scrollh
		&& l.background_b_scrollv == r.background_b_scrollv
		&& l.background_b_scrollh_speed == r.background_b_scrollh_speed
		&& l.background_b_scrollv_speed == r.background_b_scrollv_speed
		&& l.special_flags == r.special_flags
		&& l.special_back_party == r.special_back_party
		&& l.special_back_enemies == r.special_back_enemies
		&& l.special_lateral_party == r.special_lateral_party
		&& l.special_lateral_enemies == r.special_lateral_enemies
		&& l.grid_location == r.grid_location
		&& l.grid_top_y == r.grid_top_y
		&& l.grid_elongation == r.grid_elongation
		&& l.grid_inclination == r.grid_inclination
		&& l.easyrpg_damage_in_percent == r.easyrpg_damage_in_percent
		&& l.easyrpg_damage_can_kill == r.easyrpg_damage_can_kill;
	}

	inline bool operator!=(const Terrain& l, const Terrain& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Terrain& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Terrain& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Terrain, ParentCtx>{ "name", -1, &obj, parent_ctx };
		f(obj.name, ctx1);
		const auto ctx4 = Context<Terrain, ParentCtx>{ "background_name", -1, &obj, parent_ctx };
		f(obj.background_name, ctx4);
		const auto ctx10 = Context<Terrain, ParentCtx>{ "footstep", -1, &obj, parent_ctx };
		ForEachString(obj.footstep, f, &ctx10);
		const auto ctx13 = Context<Terrain, ParentCtx>{ "background_a_name", -1, &obj, parent_ctx };
		f(obj.background_a_name, ctx13);
		const auto ctx19 = Context<Terrain, ParentCtx>{ "background_b_name", -1, &obj, parent_ctx };
		f(obj.background_b_name, ctx19);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
