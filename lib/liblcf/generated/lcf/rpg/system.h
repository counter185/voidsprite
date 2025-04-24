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

#ifndef LCF_RPG_SYSTEM_H
#define LCF_RPG_SYSTEM_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/dbstring.h"
#include "lcf/enum_tags.h"
#include "lcf/rpg/music.h"
#include "lcf/rpg/sound.h"
#include "lcf/rpg/testbattler.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::System class.
 */
namespace lcf {
namespace rpg {
	class System {
	public:
		enum FadeOut {
			FadeOut_default = 0,
			FadeOut_fade_out = 1,
			FadeOut_remove_blocks = 2,
			FadeOut_wipe_downward = 3,
			FadeOut_wipe_upward = 4,
			FadeOut_venetian_blinds = 5,
			FadeOut_vertical_blinds = 6,
			FadeOut_horizontal_blinds = 7,
			FadeOut_receding_square = 8,
			FadeOut_expanding_square = 9,
			FadeOut_screen_moves_up = 10,
			FadeOut_screen_moves_down = 11,
			FadeOut_screen_moves_left = 12,
			FadeOut_screen_moves_right = 13,
			FadeOut_vertical_div = 14,
			FadeOut_horizontal_div = 15,
			FadeOut_quadrasection = 16,
			FadeOut_zoom_in = 17,
			FadeOut_mosaic = 18,
			FadeOut_waver_screen = 19,
			FadeOut_instantaneous = 20,
			FadeOut_none = 21
		};
		static constexpr auto kFadeOutTags = lcf::makeEnumTags<FadeOut>(
			"default",
			"fade_out",
			"remove_blocks",
			"wipe_downward",
			"wipe_upward",
			"venetian_blinds",
			"vertical_blinds",
			"horizontal_blinds",
			"receding_square",
			"expanding_square",
			"screen_moves_up",
			"screen_moves_down",
			"screen_moves_left",
			"screen_moves_right",
			"vertical_div",
			"horizontal_div",
			"quadrasection",
			"zoom_in",
			"mosaic",
			"waver_screen",
			"instantaneous",
			"none"
		);
		enum FadeIn {
			FadeIn_default = 0,
			FadeIn_fade_in = 1,
			FadeIn_reconstitute_blocks = 2,
			FadeIn_unwipe_downward = 3,
			FadeIn_unwipe_upward = 4,
			FadeIn_venetian_blinds = 5,
			FadeIn_vertical_blinds = 6,
			FadeIn_horizontal_blinds = 7,
			FadeIn_receding_square = 8,
			FadeIn_expanding_square = 9,
			FadeIn_screen_moves_down = 10,
			FadeIn_screen_moves_up = 11,
			FadeIn_screen_moves_right = 12,
			FadeIn_screen_moves_left = 13,
			FadeIn_vertical_unify = 14,
			FadeIn_horizontal_unify = 15,
			FadeIn_unify_quadrants = 16,
			FadeIn_zoom_out = 17,
			FadeIn_mosaic = 18,
			FadeIn_waver_screen = 19,
			FadeIn_instantaneous = 20,
			FadeIn_none = 21
		};
		static constexpr auto kFadeInTags = lcf::makeEnumTags<FadeIn>(
			"default",
			"fade_in",
			"reconstitute_blocks",
			"unwipe_downward",
			"unwipe_upward",
			"venetian_blinds",
			"vertical_blinds",
			"horizontal_blinds",
			"receding_square",
			"expanding_square",
			"screen_moves_down",
			"screen_moves_up",
			"screen_moves_right",
			"screen_moves_left",
			"vertical_unify",
			"horizontal_unify",
			"unify_quadrants",
			"zoom_out",
			"mosaic",
			"waver_screen",
			"instantaneous",
			"none"
		);
		enum Stretch {
			Stretch_stretch = 0,
			Stretch_tiled = 1,
			Stretch_easyrpg_none = 2
		};
		static constexpr auto kStretchTags = lcf::makeEnumTags<Stretch>(
			"stretch",
			"tiled",
			"easyrpg_none"
		);
		enum Font {
			Font_gothic = 0,
			Font_mincho = 1
		};
		static constexpr auto kFontTags = lcf::makeEnumTags<Font>(
			"gothic",
			"mincho"
		);
		enum BattleFormation {
			BattleFormation_terrain = 0,
			BattleFormation_loose = 1,
			BattleFormation_tight = 2
		};
		static constexpr auto kBattleFormationTags = lcf::makeEnumTags<BattleFormation>(
			"terrain",
			"loose",
			"tight"
		);
		enum BattleCondition {
			BattleCondition_none = 0,
			BattleCondition_initiative = 1,
			BattleCondition_back = 2,
			BattleCondition_surround = 3,
			BattleCondition_pincers = 4
		};
		static constexpr auto kBattleConditionTags = lcf::makeEnumTags<BattleCondition>(
			"none",
			"initiative",
			"back",
			"surround",
			"pincers"
		);
		enum EquipmentSetting {
			EquipmentSetting_actor = 0,
			EquipmentSetting_class = 1
		};
		static constexpr auto kEquipmentSettingTags = lcf::makeEnumTags<EquipmentSetting>(
			"actor",
			"class"
		);

		int32_t ldb_id = 0;
		DBString boat_name;
		DBString ship_name;
		DBString airship_name;
		int32_t boat_index = 0;
		int32_t ship_index = 0;
		int32_t airship_index = 0;
		DBString title_name;
		DBString gameover_name;
		DBString system_name;
		DBString system2_name;
		std::vector<int16_t> party = {1};
		std::vector<int16_t> menu_commands = {1};
		Music title_music;
		Music battle_music;
		Music battle_end_music;
		Music inn_music;
		Music boat_music;
		Music ship_music;
		Music airship_music;
		Music gameover_music;
		Sound cursor_se;
		Sound decision_se;
		Sound cancel_se;
		Sound buzzer_se;
		Sound battle_se;
		Sound escape_se;
		Sound enemy_attack_se;
		Sound enemy_damaged_se;
		Sound actor_damaged_se;
		Sound dodge_se;
		Sound enemy_death_se;
		Sound item_se;
		int32_t transition_out = 0;
		int32_t transition_in = 0;
		int32_t battle_start_fadeout = 0;
		int32_t battle_start_fadein = 0;
		int32_t battle_end_fadeout = 0;
		int32_t battle_end_fadein = 0;
		int32_t message_stretch = 0;
		int32_t font_id = 0;
		int32_t selected_condition = 0;
		int32_t selected_hero = 0;
		DBString battletest_background;
		std::vector<TestBattler> battletest_data;
		int32_t save_count = 0;
		int32_t battletest_terrain = 0;
		int32_t battletest_formation = 0;
		int32_t battletest_condition = 0;
		int32_t equipment_setting = 0;
		int32_t battletest_alt_terrain = -1;
		bool show_frame = false;
		DBString frame_name;
		bool invert_animations = false;
		bool show_title = true;
		int32_t easyrpg_alternative_exp = 0;
		std::vector<int16_t> easyrpg_battle_options = {0, 1, 2};
		int32_t easyrpg_max_actor_hp = -1;
		int32_t easyrpg_max_enemy_hp = -1;
		int32_t easyrpg_max_damage = -1;
		int32_t easyrpg_max_exp = -1;
		int32_t easyrpg_max_level = -1;
		int32_t easyrpg_max_savefiles = 15;
		int32_t easyrpg_max_item_count = -1;
		int32_t easyrpg_variable_min_value = 0;
		int32_t easyrpg_variable_max_value = 0;
		int32_t easyrpg_max_actor_sp = -1;
		int32_t easyrpg_max_enemy_sp = -1;
		int32_t easyrpg_max_stat_base_value = -1;
		int32_t easyrpg_max_stat_battle_value = -1;
		bool easyrpg_use_rpg2k_battle_system = false;
		bool easyrpg_battle_use_rpg2ke_strings = false;
		bool easyrpg_use_rpg2k_battle_commands = false;
		int32_t easyrpg_default_actorai = -1;
		int32_t easyrpg_default_enemyai = -1;
	};
	inline std::ostream& operator<<(std::ostream& os, System::FadeOut code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, System::FadeIn code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, System::Stretch code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, System::Font code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, System::BattleFormation code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, System::BattleCondition code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, System::EquipmentSetting code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const System& l, const System& r) {
		return l.ldb_id == r.ldb_id
		&& l.boat_name == r.boat_name
		&& l.ship_name == r.ship_name
		&& l.airship_name == r.airship_name
		&& l.boat_index == r.boat_index
		&& l.ship_index == r.ship_index
		&& l.airship_index == r.airship_index
		&& l.title_name == r.title_name
		&& l.gameover_name == r.gameover_name
		&& l.system_name == r.system_name
		&& l.system2_name == r.system2_name
		&& l.party == r.party
		&& l.menu_commands == r.menu_commands
		&& l.title_music == r.title_music
		&& l.battle_music == r.battle_music
		&& l.battle_end_music == r.battle_end_music
		&& l.inn_music == r.inn_music
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
		&& l.message_stretch == r.message_stretch
		&& l.font_id == r.font_id
		&& l.selected_condition == r.selected_condition
		&& l.selected_hero == r.selected_hero
		&& l.battletest_background == r.battletest_background
		&& l.battletest_data == r.battletest_data
		&& l.save_count == r.save_count
		&& l.battletest_terrain == r.battletest_terrain
		&& l.battletest_formation == r.battletest_formation
		&& l.battletest_condition == r.battletest_condition
		&& l.equipment_setting == r.equipment_setting
		&& l.battletest_alt_terrain == r.battletest_alt_terrain
		&& l.show_frame == r.show_frame
		&& l.frame_name == r.frame_name
		&& l.invert_animations == r.invert_animations
		&& l.show_title == r.show_title
		&& l.easyrpg_alternative_exp == r.easyrpg_alternative_exp
		&& l.easyrpg_battle_options == r.easyrpg_battle_options
		&& l.easyrpg_max_actor_hp == r.easyrpg_max_actor_hp
		&& l.easyrpg_max_enemy_hp == r.easyrpg_max_enemy_hp
		&& l.easyrpg_max_damage == r.easyrpg_max_damage
		&& l.easyrpg_max_exp == r.easyrpg_max_exp
		&& l.easyrpg_max_level == r.easyrpg_max_level
		&& l.easyrpg_max_savefiles == r.easyrpg_max_savefiles
		&& l.easyrpg_max_item_count == r.easyrpg_max_item_count
		&& l.easyrpg_variable_min_value == r.easyrpg_variable_min_value
		&& l.easyrpg_variable_max_value == r.easyrpg_variable_max_value
		&& l.easyrpg_max_actor_sp == r.easyrpg_max_actor_sp
		&& l.easyrpg_max_enemy_sp == r.easyrpg_max_enemy_sp
		&& l.easyrpg_max_stat_base_value == r.easyrpg_max_stat_base_value
		&& l.easyrpg_max_stat_battle_value == r.easyrpg_max_stat_battle_value
		&& l.easyrpg_use_rpg2k_battle_system == r.easyrpg_use_rpg2k_battle_system
		&& l.easyrpg_battle_use_rpg2ke_strings == r.easyrpg_battle_use_rpg2ke_strings
		&& l.easyrpg_use_rpg2k_battle_commands == r.easyrpg_use_rpg2k_battle_commands
		&& l.easyrpg_default_actorai == r.easyrpg_default_actorai
		&& l.easyrpg_default_enemyai == r.easyrpg_default_enemyai;
	}

	inline bool operator!=(const System& l, const System& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const System& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(System& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx2 = Context<System, ParentCtx>{ "boat_name", -1, &obj, parent_ctx };
		f(obj.boat_name, ctx2);
		const auto ctx3 = Context<System, ParentCtx>{ "ship_name", -1, &obj, parent_ctx };
		f(obj.ship_name, ctx3);
		const auto ctx4 = Context<System, ParentCtx>{ "airship_name", -1, &obj, parent_ctx };
		f(obj.airship_name, ctx4);
		const auto ctx8 = Context<System, ParentCtx>{ "title_name", -1, &obj, parent_ctx };
		f(obj.title_name, ctx8);
		const auto ctx9 = Context<System, ParentCtx>{ "gameover_name", -1, &obj, parent_ctx };
		f(obj.gameover_name, ctx9);
		const auto ctx10 = Context<System, ParentCtx>{ "system_name", -1, &obj, parent_ctx };
		f(obj.system_name, ctx10);
		const auto ctx11 = Context<System, ParentCtx>{ "system2_name", -1, &obj, parent_ctx };
		f(obj.system2_name, ctx11);
		const auto ctx14 = Context<System, ParentCtx>{ "title_music", -1, &obj, parent_ctx };
		ForEachString(obj.title_music, f, &ctx14);
		const auto ctx15 = Context<System, ParentCtx>{ "battle_music", -1, &obj, parent_ctx };
		ForEachString(obj.battle_music, f, &ctx15);
		const auto ctx16 = Context<System, ParentCtx>{ "battle_end_music", -1, &obj, parent_ctx };
		ForEachString(obj.battle_end_music, f, &ctx16);
		const auto ctx17 = Context<System, ParentCtx>{ "inn_music", -1, &obj, parent_ctx };
		ForEachString(obj.inn_music, f, &ctx17);
		const auto ctx18 = Context<System, ParentCtx>{ "boat_music", -1, &obj, parent_ctx };
		ForEachString(obj.boat_music, f, &ctx18);
		const auto ctx19 = Context<System, ParentCtx>{ "ship_music", -1, &obj, parent_ctx };
		ForEachString(obj.ship_music, f, &ctx19);
		const auto ctx20 = Context<System, ParentCtx>{ "airship_music", -1, &obj, parent_ctx };
		ForEachString(obj.airship_music, f, &ctx20);
		const auto ctx21 = Context<System, ParentCtx>{ "gameover_music", -1, &obj, parent_ctx };
		ForEachString(obj.gameover_music, f, &ctx21);
		const auto ctx22 = Context<System, ParentCtx>{ "cursor_se", -1, &obj, parent_ctx };
		ForEachString(obj.cursor_se, f, &ctx22);
		const auto ctx23 = Context<System, ParentCtx>{ "decision_se", -1, &obj, parent_ctx };
		ForEachString(obj.decision_se, f, &ctx23);
		const auto ctx24 = Context<System, ParentCtx>{ "cancel_se", -1, &obj, parent_ctx };
		ForEachString(obj.cancel_se, f, &ctx24);
		const auto ctx25 = Context<System, ParentCtx>{ "buzzer_se", -1, &obj, parent_ctx };
		ForEachString(obj.buzzer_se, f, &ctx25);
		const auto ctx26 = Context<System, ParentCtx>{ "battle_se", -1, &obj, parent_ctx };
		ForEachString(obj.battle_se, f, &ctx26);
		const auto ctx27 = Context<System, ParentCtx>{ "escape_se", -1, &obj, parent_ctx };
		ForEachString(obj.escape_se, f, &ctx27);
		const auto ctx28 = Context<System, ParentCtx>{ "enemy_attack_se", -1, &obj, parent_ctx };
		ForEachString(obj.enemy_attack_se, f, &ctx28);
		const auto ctx29 = Context<System, ParentCtx>{ "enemy_damaged_se", -1, &obj, parent_ctx };
		ForEachString(obj.enemy_damaged_se, f, &ctx29);
		const auto ctx30 = Context<System, ParentCtx>{ "actor_damaged_se", -1, &obj, parent_ctx };
		ForEachString(obj.actor_damaged_se, f, &ctx30);
		const auto ctx31 = Context<System, ParentCtx>{ "dodge_se", -1, &obj, parent_ctx };
		ForEachString(obj.dodge_se, f, &ctx31);
		const auto ctx32 = Context<System, ParentCtx>{ "enemy_death_se", -1, &obj, parent_ctx };
		ForEachString(obj.enemy_death_se, f, &ctx32);
		const auto ctx33 = Context<System, ParentCtx>{ "item_se", -1, &obj, parent_ctx };
		ForEachString(obj.item_se, f, &ctx33);
		const auto ctx44 = Context<System, ParentCtx>{ "battletest_background", -1, &obj, parent_ctx };
		f(obj.battletest_background, ctx44);
		for (int i = 0; i < static_cast<int>(obj.battletest_data.size()); ++i) {
			const auto ctx45 = Context<System, ParentCtx>{ "battletest_data", i, &obj, parent_ctx };
			ForEachString(obj.battletest_data[i], f, &ctx45);
		}
		const auto ctx53 = Context<System, ParentCtx>{ "frame_name", -1, &obj, parent_ctx };
		f(obj.frame_name, ctx53);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
