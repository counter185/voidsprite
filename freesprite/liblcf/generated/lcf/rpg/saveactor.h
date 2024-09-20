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

#ifndef LCF_RPG_SAVEACTOR_H
#define LCF_RPG_SAVEACTOR_H

// Headers
#include <stdint.h>
#include <string>
#include <vector>
#include "lcf/enum_tags.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::SaveActor class.
 */
namespace lcf {
namespace rpg {
	class SaveActor {
	public:
		// Sentinel name used to denote that the default LDB name should be used.
		static constexpr const char* kEmptyName = "\x1";

		enum RowType {
			RowType_front = 0,
			RowType_back = 1
		};
		static constexpr auto kRowTypeTags = lcf::makeEnumTags<RowType>(
			"front",
			"back"
		);

		int ID = 0;
		std::string name = kEmptyName;
		std::string title = kEmptyName;
		std::string sprite_name;
		int32_t sprite_id = 0;
		int32_t transparency = 0;
		std::string face_name;
		int32_t face_id = 0;
		int32_t level = -1;
		int32_t exp = -1;
		int32_t hp_mod = -1;
		int32_t sp_mod = -1;
		int32_t attack_mod = 0;
		int32_t defense_mod = 0;
		int32_t spirit_mod = 0;
		int32_t agility_mod = 0;
		std::vector<int16_t> skills;
		std::vector<int16_t> equipped = {0, 0, 0, 0, 0};
		int32_t current_hp = -1;
		int32_t current_sp = -1;
		std::vector<int32_t> battle_commands = {-1, -1, -1, -1, -1, -1, -1};
		std::vector<int16_t> status;
		bool changed_battle_commands = false;
		int32_t class_id = -1;
		int32_t row = 0;
		bool two_weapon = false;
		bool lock_equipment = false;
		bool auto_battle = false;
		bool super_guard = false;
		int32_t battler_animation = 0;
	};
	inline std::ostream& operator<<(std::ostream& os, SaveActor::RowType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const SaveActor& l, const SaveActor& r) {
		return l.name == r.name
		&& l.title == r.title
		&& l.sprite_name == r.sprite_name
		&& l.sprite_id == r.sprite_id
		&& l.transparency == r.transparency
		&& l.face_name == r.face_name
		&& l.face_id == r.face_id
		&& l.level == r.level
		&& l.exp == r.exp
		&& l.hp_mod == r.hp_mod
		&& l.sp_mod == r.sp_mod
		&& l.attack_mod == r.attack_mod
		&& l.defense_mod == r.defense_mod
		&& l.spirit_mod == r.spirit_mod
		&& l.agility_mod == r.agility_mod
		&& l.skills == r.skills
		&& l.equipped == r.equipped
		&& l.current_hp == r.current_hp
		&& l.current_sp == r.current_sp
		&& l.battle_commands == r.battle_commands
		&& l.status == r.status
		&& l.changed_battle_commands == r.changed_battle_commands
		&& l.class_id == r.class_id
		&& l.row == r.row
		&& l.two_weapon == r.two_weapon
		&& l.lock_equipment == r.lock_equipment
		&& l.auto_battle == r.auto_battle
		&& l.super_guard == r.super_guard
		&& l.battler_animation == r.battler_animation;
	}

	inline bool operator!=(const SaveActor& l, const SaveActor& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const SaveActor& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(SaveActor& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
