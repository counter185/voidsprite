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

#ifndef LCF_RPG_EVENTPAGE_H
#define LCF_RPG_EVENTPAGE_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/rpg/eventcommand.h"
#include "lcf/rpg/eventpagecondition.h"
#include "lcf/rpg/moveroute.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::EventPage class.
 */
namespace lcf {
namespace rpg {
	class EventPage {
	public:
		enum Direction {
			Direction_up = 0,
			Direction_right = 1,
			Direction_down = 2,
			Direction_left = 3
		};
		static constexpr auto kDirectionTags = lcf::makeEnumTags<Direction>(
			"up",
			"right",
			"down",
			"left"
		);
		enum Frame {
			Frame_left = 0,
			Frame_middle = 1,
			Frame_right = 2,
			Frame_middle2 = 3
		};
		static constexpr auto kFrameTags = lcf::makeEnumTags<Frame>(
			"left",
			"middle",
			"right",
			"middle2"
		);
		enum MoveType {
			MoveType_stationary = 0,
			MoveType_random = 1,
			MoveType_vertical = 2,
			MoveType_horizontal = 3,
			MoveType_toward = 4,
			MoveType_away = 5,
			MoveType_custom = 6
		};
		static constexpr auto kMoveTypeTags = lcf::makeEnumTags<MoveType>(
			"stationary",
			"random",
			"vertical",
			"horizontal",
			"toward",
			"away",
			"custom"
		);
		enum Trigger {
			Trigger_action = 0,
			Trigger_touched = 1,
			Trigger_collision = 2,
			Trigger_auto_start = 3,
			Trigger_parallel = 4
		};
		static constexpr auto kTriggerTags = lcf::makeEnumTags<Trigger>(
			"action",
			"touched",
			"collision",
			"auto_start",
			"parallel"
		);
		enum Layers {
			Layers_below = 0,
			Layers_same = 1,
			Layers_above = 2
		};
		static constexpr auto kLayersTags = lcf::makeEnumTags<Layers>(
			"below",
			"same",
			"above"
		);
		enum AnimType {
			AnimType_non_continuous = 0,
			AnimType_continuous = 1,
			AnimType_fixed_non_continuous = 2,
			AnimType_fixed_continuous = 3,
			AnimType_fixed_graphic = 4,
			AnimType_spin = 5,
			AnimType_step_frame_fix = 6
		};
		static constexpr auto kAnimTypeTags = lcf::makeEnumTags<AnimType>(
			"non_continuous",
			"continuous",
			"fixed_non_continuous",
			"fixed_continuous",
			"fixed_graphic",
			"spin",
			"step_frame_fix"
		);
		enum MoveSpeed {
			MoveSpeed_eighth = 1,
			MoveSpeed_quarter = 2,
			MoveSpeed_half = 3,
			MoveSpeed_normal = 4,
			MoveSpeed_double = 5,
			MoveSpeed_fourfold = 6
		};
		static constexpr auto kMoveSpeedTags = lcf::EnumTags{
			MoveSpeed_eighth, "eighth",
			MoveSpeed_quarter, "quarter",
			MoveSpeed_half, "half",
			MoveSpeed_normal, "normal",
			MoveSpeed_double, "double",
			MoveSpeed_fourfold, "fourfold"
		};
		enum ManiacEventInfo {
			ManiacEventInfo_action = 0,
			ManiacEventInfo_touched = 1,
			ManiacEventInfo_collision = 2,
			ManiacEventInfo_auto_start = 3,
			ManiacEventInfo_parallel = 4,
			ManiacEventInfo_called = 5,
			ManiacEventInfo_battle_start = 6,
			ManiacEventInfo_battle_parallel = 7,
			ManiacEventInfo_map_event = 16,
			ManiacEventInfo_common_event = 32,
			ManiacEventInfo_battle_event = 64
		};
		static constexpr auto kManiacEventInfoTags = lcf::EnumTags{
			ManiacEventInfo_action, "action",
			ManiacEventInfo_touched, "touched",
			ManiacEventInfo_collision, "collision",
			ManiacEventInfo_auto_start, "auto_start",
			ManiacEventInfo_parallel, "parallel",
			ManiacEventInfo_called, "called",
			ManiacEventInfo_battle_start, "battle_start",
			ManiacEventInfo_battle_parallel, "battle_parallel",
			ManiacEventInfo_map_event, "map_event",
			ManiacEventInfo_common_event, "common_event",
			ManiacEventInfo_battle_event, "battle_event"
		};

		int ID = 0;
		EventPageCondition condition;
		DBString character_name;
		int32_t character_index = 0;
		int32_t character_direction = 2;
		int32_t character_pattern = 1;
		bool translucent = false;
		int32_t move_type = 1;
		int32_t move_frequency = 3;
		int32_t trigger = 0;
		int32_t layer = 0;
		bool overlap_forbidden = false;
		int32_t animation_type = 0;
		int32_t move_speed = 3;
		MoveRoute move_route;
		std::vector<EventCommand> event_commands;
	};
	inline std::ostream& operator<<(std::ostream& os, EventPage::Direction code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, EventPage::Frame code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, EventPage::MoveType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, EventPage::Trigger code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, EventPage::Layers code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, EventPage::AnimType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, EventPage::MoveSpeed code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, EventPage::ManiacEventInfo code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const EventPage& l, const EventPage& r) {
		return l.condition == r.condition
		&& l.character_name == r.character_name
		&& l.character_index == r.character_index
		&& l.character_direction == r.character_direction
		&& l.character_pattern == r.character_pattern
		&& l.translucent == r.translucent
		&& l.move_type == r.move_type
		&& l.move_frequency == r.move_frequency
		&& l.trigger == r.trigger
		&& l.layer == r.layer
		&& l.overlap_forbidden == r.overlap_forbidden
		&& l.animation_type == r.animation_type
		&& l.move_speed == r.move_speed
		&& l.move_route == r.move_route
		&& l.event_commands == r.event_commands;
	}

	inline bool operator!=(const EventPage& l, const EventPage& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const EventPage& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(EventPage& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<EventPage, ParentCtx>{ "condition", -1, &obj, parent_ctx };
		ForEachString(obj.condition, f, &ctx1);
		const auto ctx2 = Context<EventPage, ParentCtx>{ "character_name", -1, &obj, parent_ctx };
		f(obj.character_name, ctx2);
		const auto ctx14 = Context<EventPage, ParentCtx>{ "move_route", -1, &obj, parent_ctx };
		ForEachString(obj.move_route, f, &ctx14);
		for (int i = 0; i < static_cast<int>(obj.event_commands.size()); ++i) {
			const auto ctx15 = Context<EventPage, ParentCtx>{ "event_commands", i, &obj, parent_ctx };
			ForEachString(obj.event_commands[i], f, &ctx15);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
