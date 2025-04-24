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

#ifndef LCF_RPG_BATTLECOMMANDS_H
#define LCF_RPG_BATTLECOMMANDS_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/enum_tags.h"
#include "lcf/rpg/battlecommand.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::BattleCommands class.
 */
namespace lcf {
namespace rpg {
	class BattleCommands {
	public:
		enum Placement {
			Placement_manual = 0,
			Placement_automatic = 1
		};
		static constexpr auto kPlacementTags = lcf::makeEnumTags<Placement>(
			"manual",
			"automatic"
		);
		enum RowShown {
			RowShown_front = 0,
			RowShown_back = 1
		};
		static constexpr auto kRowShownTags = lcf::makeEnumTags<RowShown>(
			"front",
			"back"
		);
		enum BattleType {
			BattleType_traditional = 0,
			BattleType_alternative = 1,
			BattleType_gauge = 2
		};
		static constexpr auto kBattleTypeTags = lcf::makeEnumTags<BattleType>(
			"traditional",
			"alternative",
			"gauge"
		);
		enum WindowSize {
			WindowSize_large = 0,
			WindowSize_small = 1
		};
		static constexpr auto kWindowSizeTags = lcf::makeEnumTags<WindowSize>(
			"large",
			"small"
		);
		enum Transparency {
			Transparency_opaque = 0,
			Transparency_transparent = 1
		};
		static constexpr auto kTransparencyTags = lcf::makeEnumTags<Transparency>(
			"opaque",
			"transparent"
		);
		enum Facing {
			Facing_retain = 0,
			Facing_up = 1,
			Facing_right = 2,
			Facing_down = 3,
			Facing_left = 4
		};
		static constexpr auto kFacingTags = lcf::makeEnumTags<Facing>(
			"retain",
			"up",
			"right",
			"down",
			"left"
		);

		int32_t placement = 0;
		bool death_handler_unused = 0;
		int32_t row = 0;
		int32_t battle_type = 0;
		bool unused_display_normal_parameters = true;
		std::vector<BattleCommand> commands;
		bool death_handler = 0;
		int32_t death_event = 1;
		int32_t window_size = 0;
		int32_t transparency = 0;
		bool death_teleport = false;
		int32_t death_teleport_id = 1;
		int32_t death_teleport_x = 0;
		int32_t death_teleport_y = 0;
		int32_t death_teleport_face = 0;
		int32_t easyrpg_default_atb_mode = 0;
		bool easyrpg_enable_battle_row_command = true;
		bool easyrpg_sequential_order = false;
		bool easyrpg_disable_row_feature = false;
		int32_t easyrpg_fixed_actor_facing_direction = 0;
		int32_t easyrpg_fixed_enemy_facing_direction = 0;
	};
	inline std::ostream& operator<<(std::ostream& os, BattleCommands::Placement code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, BattleCommands::RowShown code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, BattleCommands::BattleType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, BattleCommands::WindowSize code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, BattleCommands::Transparency code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, BattleCommands::Facing code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const BattleCommands& l, const BattleCommands& r) {
		return l.placement == r.placement
		&& l.death_handler_unused == r.death_handler_unused
		&& l.row == r.row
		&& l.battle_type == r.battle_type
		&& l.unused_display_normal_parameters == r.unused_display_normal_parameters
		&& l.commands == r.commands
		&& l.death_handler == r.death_handler
		&& l.death_event == r.death_event
		&& l.window_size == r.window_size
		&& l.transparency == r.transparency
		&& l.death_teleport == r.death_teleport
		&& l.death_teleport_id == r.death_teleport_id
		&& l.death_teleport_x == r.death_teleport_x
		&& l.death_teleport_y == r.death_teleport_y
		&& l.death_teleport_face == r.death_teleport_face
		&& l.easyrpg_default_atb_mode == r.easyrpg_default_atb_mode
		&& l.easyrpg_enable_battle_row_command == r.easyrpg_enable_battle_row_command
		&& l.easyrpg_sequential_order == r.easyrpg_sequential_order
		&& l.easyrpg_disable_row_feature == r.easyrpg_disable_row_feature
		&& l.easyrpg_fixed_actor_facing_direction == r.easyrpg_fixed_actor_facing_direction
		&& l.easyrpg_fixed_enemy_facing_direction == r.easyrpg_fixed_enemy_facing_direction;
	}

	inline bool operator!=(const BattleCommands& l, const BattleCommands& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const BattleCommands& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(BattleCommands& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		for (int i = 0; i < static_cast<int>(obj.commands.size()); ++i) {
			const auto ctx6 = Context<BattleCommands, ParentCtx>{ "commands", i, &obj, parent_ctx };
			ForEachString(obj.commands[i], f, &ctx6);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
