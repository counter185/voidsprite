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

#ifndef LCF_RPG_SAVESYSTEM_H
#define LCF_RPG_SAVESYSTEM_H

// Headers
#include <stdint.h>
#include <string>
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/rpg/music.h"
#include "lcf/rpg/sound.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveSystem class.
 */
namespace lcf {
namespace rpg {
	class SaveSystem {
	public:
		enum Scene {
			Scene_map = 0,
			Scene_menu = 1,
			Scene_battle = 2,
			Scene_shop = 3,
			Scene_name = 4,
			Scene_file = 5,
			Scene_title = 6,
			Scene_game_over = 7,
			Scene_debug = 8
		};
		static constexpr auto kSceneTags = lcf::makeEnumTags<Scene>(
			"map",
			"menu",
			"battle",
			"shop",
			"name",
			"file",
			"title",
			"game_over",
			"debug"
		);
		enum AtbMode {
			AtbMode_atb_active = 0,
			AtbMode_atb_wait = 1
		};
		static constexpr auto kAtbModeTags = lcf::makeEnumTags<AtbMode>(
			"atb_active",
			"atb_wait"
		);

		int32_t scene = 0;
		int32_t frame_count = 0;
		std::string graphics_name;
		int32_t message_stretch = 0;
		int32_t font_id = 0;
		std::vector<bool> switches;
		std::vector<int32_t> variables;
		int32_t message_transparent = 0;
		int32_t message_position = 2;
		int32_t message_prevent_overlap = 1;
		int32_t message_continue_events = 0;
		std::string face_name;
		int32_t face_id = 0;
		bool face_right = false;
		bool face_flip = false;
		bool event_message_active = false;
		bool music_stopping = false;
		Music title_music = Music{ "" };
		Music battle_music = Music{ "" };
		Music battle_end_music = Music{ "" };
		Music inn_music = Music{ "" };
		Music current_music;
		Music before_vehicle_music;
		Music before_battle_music;
		Music stored_music;
		Music boat_music = Music{ "" };
		Music ship_music = Music{ "" };
		Music airship_music = Music{ "" };
		Music gameover_music = Music{ "" };
		Sound cursor_se = Sound{ "" };
		Sound decision_se = Sound{ "" };
		Sound cancel_se = Sound{ "" };
		Sound buzzer_se = Sound{ "" };
		Sound battle_se = Sound{ "" };
		Sound escape_se = Sound{ "" };
		Sound enemy_attack_se = Sound{ "" };
		Sound enemy_damaged_se = Sound{ "" };
		Sound actor_damaged_se = Sound{ "" };
		Sound dodge_se = Sound{ "" };
		Sound enemy_death_se = Sound{ "" };
		Sound item_se = Sound{ "" };
		int8_t transition_out = -1;
		int8_t transition_in = -1;
		int8_t battle_start_fadeout = -1;
		int8_t battle_start_fadein = -1;
		int8_t battle_end_fadeout = -1;
		int8_t battle_end_fadein = -1;
		bool teleport_allowed = true;
		bool escape_allowed = true;
		bool save_allowed = true;
		bool menu_allowed = true;
		std::string background;
		int32_t save_count = 0;
		int32_t save_slot = 1;
		int32_t atb_mode = 0;
		std::vector<DBString> maniac_strings;
		int32_t maniac_frameskip = 0;
		int32_t maniac_picture_limit = 0;
		std::vector<uint8_t> maniac_options;
		std::vector<uint8_t> maniac_joypad_bindings;
	};
	inline std::ostream& operator<<(std::ostream& os, SaveSystem::Scene code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, SaveSystem::AtbMode code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const SaveSystem& l, const SaveSystem& r) {
		return l.scene == r.scene
		&& l.frame_count == r.frame_count
		&& l.graphics_name == r.graphics_name
		&& l.message_stretch == r.message_stretch
		&& l.font_id == r.font_id
		&& l.switches == r.switches
		&& l.variables == r.variables
		&& l.message_transparent == r.message_transparent
		&& l.message_position == r.message_position
		&& l.message_prevent_overlap == r.message_prevent_overlap
		&& l.message_continue_events == r.message_continue_events
		&& l.face_name == r.face_name
		&& l.face_id == r.face_id
		&& l.face_right == r.face_right
		&& l.face_flip == r.face_flip
		&& l.event_message_active == r.event_message_active
		&& l.music_stopping == r.music_stopping
		&& l.title_music == r.title_music
		&& l.battle_music == r.battle_music
		&& l.battle_end_music == r.battle_end_music
		&& l.inn_music == r.inn_music
		&& l.current_music == r.current_music
		&& l.before_vehicle_music == r.before_vehicle_music
		&& l.before_battle_music == r.before_battle_music
		&& l.stored_music == r.stored_music
		&& l.boat_music == r.boat_music
		&& l.ship_music == r.ship_music
		&& l.airship_music == r.airship_music
		&& l.gameover_music == r.gameover_music
		&& l.cursor_se == r.cursor_se
		&& l.decision_se == r.decision_se
		&& l.cancel_se == r.cancel_se
		&& l.buzzer_se == r.buzzer_se
		&& l.battle_se == r.battle_se
		&& l.escape_se == r.escape_se
		&& l.enemy_attack_se == r.enemy_attack_se
		&& l.enemy_damaged_se == r.enemy_damaged_se
		&& l.actor_damaged_se == r.actor_damaged_se
		&& l.dodge_se == r.dodge_se
		&& l.enemy_death_se == r.enemy_death_se
		&& l.item_se == r.item_se
		&& l.transition_out == r.transition_out
		&& l.transition_in == r.transition_in
		&& l.battle_start_fadeout == r.battle_start_fadeout
		&& l.battle_start_fadein == r.battle_start_fadein
		&& l.battle_end_fadeout == r.battle_end_fadeout
		&& l.battle_end_fadein == r.battle_end_fadein
		&& l.teleport_allowed == r.teleport_allowed
		&& l.escape_allowed == r.escape_allowed
		&& l.save_allowed == r.save_allowed
		&& l.menu_allowed == r.menu_allowed
		&& l.background == r.background
		&& l.save_count == r.save_count
		&& l.save_slot == r.save_slot
		&& l.atb_mode == r.atb_mode
		&& l.maniac_strings == r.maniac_strings
		&& l.maniac_frameskip == r.maniac_frameskip
		&& l.maniac_picture_limit == r.maniac_picture_limit
		&& l.maniac_options == r.maniac_options
		&& l.maniac_joypad_bindings == r.maniac_joypad_bindings;
	}

	inline bool operator!=(const SaveSystem& l, const SaveSystem& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveSystem& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveSystem& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx18 = Context<SaveSystem, ParentCtx>{ "title_music", -1, &obj, parent_ctx };
		ForEachString(obj.title_music, f, &ctx18);
		const auto ctx19 = Context<SaveSystem, ParentCtx>{ "battle_music", -1, &obj, parent_ctx };
		ForEachString(obj.battle_music, f, &ctx19);
		const auto ctx20 = Context<SaveSystem, ParentCtx>{ "battle_end_music", -1, &obj, parent_ctx };
		ForEachString(obj.battle_end_music, f, &ctx20);
		const auto ctx21 = Context<SaveSystem, ParentCtx>{ "inn_music", -1, &obj, parent_ctx };
		ForEachString(obj.inn_music, f, &ctx21);
		const auto ctx22 = Context<SaveSystem, ParentCtx>{ "current_music", -1, &obj, parent_ctx };
		ForEachString(obj.current_music, f, &ctx22);
		const auto ctx23 = Context<SaveSystem, ParentCtx>{ "before_vehicle_music", -1, &obj, parent_ctx };
		ForEachString(obj.before_vehicle_music, f, &ctx23);
		const auto ctx24 = Context<SaveSystem, ParentCtx>{ "before_battle_music", -1, &obj, parent_ctx };
		ForEachString(obj.before_battle_music, f, &ctx24);
		const auto ctx25 = Context<SaveSystem, ParentCtx>{ "stored_music", -1, &obj, parent_ctx };
		ForEachString(obj.stored_music, f, &ctx25);
		const auto ctx26 = Context<SaveSystem, ParentCtx>{ "boat_music", -1, &obj, parent_ctx };
		ForEachString(obj.boat_music, f, &ctx26);
		const auto ctx27 = Context<SaveSystem, ParentCtx>{ "ship_music", -1, &obj, parent_ctx };
		ForEachString(obj.ship_music, f, &ctx27);
		const auto ctx28 = Context<SaveSystem, ParentCtx>{ "airship_music", -1, &obj, parent_ctx };
		ForEachString(obj.airship_music, f, &ctx28);
		const auto ctx29 = Context<SaveSystem, ParentCtx>{ "gameover_music", -1, &obj, parent_ctx };
		ForEachString(obj.gameover_music, f, &ctx29);
		const auto ctx30 = Context<SaveSystem, ParentCtx>{ "cursor_se", -1, &obj, parent_ctx };
		ForEachString(obj.cursor_se, f, &ctx30);
		const auto ctx31 = Context<SaveSystem, ParentCtx>{ "decision_se", -1, &obj, parent_ctx };
		ForEachString(obj.decision_se, f, &ctx31);
		const auto ctx32 = Context<SaveSystem, ParentCtx>{ "cancel_se", -1, &obj, parent_ctx };
		ForEachString(obj.cancel_se, f, &ctx32);
		const auto ctx33 = Context<SaveSystem, ParentCtx>{ "buzzer_se", -1, &obj, parent_ctx };
		ForEachString(obj.buzzer_se, f, &ctx33);
		const auto ctx34 = Context<SaveSystem, ParentCtx>{ "battle_se", -1, &obj, parent_ctx };
		ForEachString(obj.battle_se, f, &ctx34);
		const auto ctx35 = Context<SaveSystem, ParentCtx>{ "escape_se", -1, &obj, parent_ctx };
		ForEachString(obj.escape_se, f, &ctx35);
		const auto ctx36 = Context<SaveSystem, ParentCtx>{ "enemy_attack_se", -1, &obj, parent_ctx };
		ForEachString(obj.enemy_attack_se, f, &ctx36);
		const auto ctx37 = Context<SaveSystem, ParentCtx>{ "enemy_damaged_se", -1, &obj, parent_ctx };
		ForEachString(obj.enemy_damaged_se, f, &ctx37);
		const auto ctx38 = Context<SaveSystem, ParentCtx>{ "actor_damaged_se", -1, &obj, parent_ctx };
		ForEachString(obj.actor_damaged_se, f, &ctx38);
		const auto ctx39 = Context<SaveSystem, ParentCtx>{ "dodge_se", -1, &obj, parent_ctx };
		ForEachString(obj.dodge_se, f, &ctx39);
		const auto ctx40 = Context<SaveSystem, ParentCtx>{ "enemy_death_se", -1, &obj, parent_ctx };
		ForEachString(obj.enemy_death_se, f, &ctx40);
		const auto ctx41 = Context<SaveSystem, ParentCtx>{ "item_se", -1, &obj, parent_ctx };
		ForEachString(obj.item_se, f, &ctx41);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
