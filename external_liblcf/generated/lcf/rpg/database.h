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

#ifndef LCF_RPG_DATABASE_H
#define LCF_RPG_DATABASE_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/rpg/actor.h"
#include "lcf/rpg/animation.h"
#include "lcf/rpg/attribute.h"
#include "lcf/rpg/battlecommands.h"
#include "lcf/rpg/battleranimation.h"
#include "lcf/rpg/chipset.h"
#include "lcf/rpg/class.h"
#include "lcf/rpg/commonevent.h"
#include "lcf/rpg/enemy.h"
#include "lcf/rpg/item.h"
#include "lcf/rpg/skill.h"
#include "lcf/rpg/state.h"
#include "lcf/rpg/switch.h"
#include "lcf/rpg/system.h"
#include "lcf/rpg/terms.h"
#include "lcf/rpg/terrain.h"
#include "lcf/rpg/troop.h"
#include "lcf/rpg/variable.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Database class.
 */
namespace lcf {
namespace rpg {
	class Database {
	public:
		std::string ldb_header;
		std::vector<Actor> actors;
		std::vector<Skill> skills;
		std::vector<Item> items;
		std::vector<Enemy> enemies;
		std::vector<Troop> troops;
		std::vector<Terrain> terrains;
		std::vector<Attribute> attributes;
		std::vector<State> states;
		std::vector<Animation> animations;
		std::vector<Chipset> chipsets;
		Terms terms;
		System system;
		std::vector<Switch> switches;
		std::vector<Variable> variables;
		std::vector<CommonEvent> commonevents;
		int32_t version = 0;
		BattleCommands battlecommands;
		std::vector<Class> classes;
		std::vector<BattlerAnimation> battleranimations;
	};

	inline bool operator==(const Database& l, const Database& r) {
		return l.actors == r.actors
		&& l.skills == r.skills
		&& l.items == r.items
		&& l.enemies == r.enemies
		&& l.troops == r.troops
		&& l.terrains == r.terrains
		&& l.attributes == r.attributes
		&& l.states == r.states
		&& l.animations == r.animations
		&& l.chipsets == r.chipsets
		&& l.terms == r.terms
		&& l.system == r.system
		&& l.switches == r.switches
		&& l.variables == r.variables
		&& l.commonevents == r.commonevents
		&& l.version == r.version
		&& l.battlecommands == r.battlecommands
		&& l.classes == r.classes
		&& l.battleranimations == r.battleranimations;
	}

	inline bool operator!=(const Database& l, const Database& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Database& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Database& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		for (int i = 0; i < static_cast<int>(obj.actors.size()); ++i) {
			const auto ctx1 = Context<Database, ParentCtx>{ "actors", i, &obj, parent_ctx };
			ForEachString(obj.actors[i], f, &ctx1);
		}
		for (int i = 0; i < static_cast<int>(obj.skills.size()); ++i) {
			const auto ctx2 = Context<Database, ParentCtx>{ "skills", i, &obj, parent_ctx };
			ForEachString(obj.skills[i], f, &ctx2);
		}
		for (int i = 0; i < static_cast<int>(obj.items.size()); ++i) {
			const auto ctx3 = Context<Database, ParentCtx>{ "items", i, &obj, parent_ctx };
			ForEachString(obj.items[i], f, &ctx3);
		}
		for (int i = 0; i < static_cast<int>(obj.enemies.size()); ++i) {
			const auto ctx4 = Context<Database, ParentCtx>{ "enemies", i, &obj, parent_ctx };
			ForEachString(obj.enemies[i], f, &ctx4);
		}
		for (int i = 0; i < static_cast<int>(obj.troops.size()); ++i) {
			const auto ctx5 = Context<Database, ParentCtx>{ "troops", i, &obj, parent_ctx };
			ForEachString(obj.troops[i], f, &ctx5);
		}
		for (int i = 0; i < static_cast<int>(obj.terrains.size()); ++i) {
			const auto ctx6 = Context<Database, ParentCtx>{ "terrains", i, &obj, parent_ctx };
			ForEachString(obj.terrains[i], f, &ctx6);
		}
		for (int i = 0; i < static_cast<int>(obj.attributes.size()); ++i) {
			const auto ctx7 = Context<Database, ParentCtx>{ "attributes", i, &obj, parent_ctx };
			ForEachString(obj.attributes[i], f, &ctx7);
		}
		for (int i = 0; i < static_cast<int>(obj.states.size()); ++i) {
			const auto ctx8 = Context<Database, ParentCtx>{ "states", i, &obj, parent_ctx };
			ForEachString(obj.states[i], f, &ctx8);
		}
		for (int i = 0; i < static_cast<int>(obj.animations.size()); ++i) {
			const auto ctx9 = Context<Database, ParentCtx>{ "animations", i, &obj, parent_ctx };
			ForEachString(obj.animations[i], f, &ctx9);
		}
		for (int i = 0; i < static_cast<int>(obj.chipsets.size()); ++i) {
			const auto ctx10 = Context<Database, ParentCtx>{ "chipsets", i, &obj, parent_ctx };
			ForEachString(obj.chipsets[i], f, &ctx10);
		}
		const auto ctx11 = Context<Database, ParentCtx>{ "terms", -1, &obj, parent_ctx };
		ForEachString(obj.terms, f, &ctx11);
		const auto ctx12 = Context<Database, ParentCtx>{ "system", -1, &obj, parent_ctx };
		ForEachString(obj.system, f, &ctx12);
		for (int i = 0; i < static_cast<int>(obj.switches.size()); ++i) {
			const auto ctx13 = Context<Database, ParentCtx>{ "switches", i, &obj, parent_ctx };
			ForEachString(obj.switches[i], f, &ctx13);
		}
		for (int i = 0; i < static_cast<int>(obj.variables.size()); ++i) {
			const auto ctx14 = Context<Database, ParentCtx>{ "variables", i, &obj, parent_ctx };
			ForEachString(obj.variables[i], f, &ctx14);
		}
		for (int i = 0; i < static_cast<int>(obj.commonevents.size()); ++i) {
			const auto ctx15 = Context<Database, ParentCtx>{ "commonevents", i, &obj, parent_ctx };
			ForEachString(obj.commonevents[i], f, &ctx15);
		}
		const auto ctx17 = Context<Database, ParentCtx>{ "battlecommands", -1, &obj, parent_ctx };
		ForEachString(obj.battlecommands, f, &ctx17);
		for (int i = 0; i < static_cast<int>(obj.classes.size()); ++i) {
			const auto ctx18 = Context<Database, ParentCtx>{ "classes", i, &obj, parent_ctx };
			ForEachString(obj.classes[i], f, &ctx18);
		}
		for (int i = 0; i < static_cast<int>(obj.battleranimations.size()); ++i) {
			const auto ctx19 = Context<Database, ParentCtx>{ "battleranimations", i, &obj, parent_ctx };
			ForEachString(obj.battleranimations[i], f, &ctx19);
		}
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
