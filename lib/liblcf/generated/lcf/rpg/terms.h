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

#ifndef LCF_RPG_TERMS_H
#define LCF_RPG_TERMS_H

// Headers
#include "lcf/dbstring.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Terms class.
 */
namespace lcf {
namespace rpg {
	class Terms {
	public:
		// Sentinel name used to denote that the default hardcoded term should be used.
		static constexpr const char* kDefaultTerm = "default_term";

		static std::string TermOrDefault(const DBString& db_term, StringView default_term);
		DBString encounter;
		DBString special_combat;
		DBString escape_success;
		DBString escape_failure;
		DBString victory;
		DBString defeat;
		DBString exp_received;
		DBString gold_recieved_a;
		DBString gold_recieved_b;
		DBString item_recieved;
		DBString attacking;
		DBString enemy_critical;
		DBString actor_critical;
		DBString defending;
		DBString observing;
		DBString focus;
		DBString autodestruction;
		DBString enemy_escape;
		DBString enemy_transform;
		DBString enemy_damaged;
		DBString enemy_undamaged;
		DBString actor_damaged;
		DBString actor_undamaged;
		DBString skill_failure_a;
		DBString skill_failure_b;
		DBString skill_failure_c;
		DBString dodge;
		DBString use_item;
		DBString hp_recovery;
		DBString parameter_increase;
		DBString parameter_decrease;
		DBString enemy_hp_absorbed;
		DBString actor_hp_absorbed;
		DBString resistance_increase;
		DBString resistance_decrease;
		DBString level_up;
		DBString skill_learned;
		DBString battle_start;
		DBString miss;
		DBString shop_greeting1;
		DBString shop_regreeting1;
		DBString shop_buy1;
		DBString shop_sell1;
		DBString shop_leave1;
		DBString shop_buy_select1;
		DBString shop_buy_number1;
		DBString shop_purchased1;
		DBString shop_sell_select1;
		DBString shop_sell_number1;
		DBString shop_sold1;
		DBString shop_greeting2;
		DBString shop_regreeting2;
		DBString shop_buy2;
		DBString shop_sell2;
		DBString shop_leave2;
		DBString shop_buy_select2;
		DBString shop_buy_number2;
		DBString shop_purchased2;
		DBString shop_sell_select2;
		DBString shop_sell_number2;
		DBString shop_sold2;
		DBString shop_greeting3;
		DBString shop_regreeting3;
		DBString shop_buy3;
		DBString shop_sell3;
		DBString shop_leave3;
		DBString shop_buy_select3;
		DBString shop_buy_number3;
		DBString shop_purchased3;
		DBString shop_sell_select3;
		DBString shop_sell_number3;
		DBString shop_sold3;
		DBString inn_a_greeting_1;
		DBString inn_a_greeting_2;
		DBString inn_a_greeting_3;
		DBString inn_a_accept;
		DBString inn_a_cancel;
		DBString inn_b_greeting_1;
		DBString inn_b_greeting_2;
		DBString inn_b_greeting_3;
		DBString inn_b_accept;
		DBString inn_b_cancel;
		DBString possessed_items;
		DBString equipped_items;
		DBString gold;
		DBString battle_fight;
		DBString battle_auto;
		DBString battle_escape;
		DBString command_attack;
		DBString command_defend;
		DBString command_item;
		DBString command_skill;
		DBString menu_equipment;
		DBString menu_save;
		DBString menu_quit;
		DBString new_game;
		DBString load_game;
		DBString exit_game;
		DBString status;
		DBString row;
		DBString order;
		DBString wait_on;
		DBString wait_off;
		DBString level;
		DBString health_points;
		DBString spirit_points;
		DBString normal_status;
		DBString exp_short;
		DBString lvl_short;
		DBString hp_short;
		DBString sp_short;
		DBString sp_cost;
		DBString attack;
		DBString defense;
		DBString spirit;
		DBString agility;
		DBString weapon;
		DBString shield;
		DBString armor;
		DBString helmet;
		DBString accessory;
		DBString save_game_message;
		DBString load_game_message;
		DBString file;
		DBString exit_game_message;
		DBString yes;
		DBString no;
		DBString easyrpg_item_number_separator = DBString(kDefaultTerm);
		DBString easyrpg_skill_cost_separator = DBString(kDefaultTerm);
		DBString easyrpg_equipment_arrow = DBString(kDefaultTerm);
		DBString easyrpg_status_scene_name = DBString(kDefaultTerm);
		DBString easyrpg_status_scene_class = DBString(kDefaultTerm);
		DBString easyrpg_status_scene_title = DBString(kDefaultTerm);
		DBString easyrpg_status_scene_condition = DBString(kDefaultTerm);
		DBString easyrpg_status_scene_front = DBString(kDefaultTerm);
		DBString easyrpg_status_scene_back = DBString(kDefaultTerm);
		DBString easyrpg_order_scene_confirm = DBString(kDefaultTerm);
		DBString easyrpg_order_scene_redo = DBString(kDefaultTerm);
		DBString easyrpg_battle2k3_double_attack = DBString(kDefaultTerm);
		DBString easyrpg_battle2k3_defend = DBString(kDefaultTerm);
		DBString easyrpg_battle2k3_observe = DBString(kDefaultTerm);
		DBString easyrpg_battle2k3_charge = DBString(kDefaultTerm);
		DBString easyrpg_battle2k3_selfdestruct = DBString(kDefaultTerm);
		DBString easyrpg_battle2k3_escape = DBString(kDefaultTerm);
		DBString easyrpg_battle2k3_special_combat_back;
		DBString easyrpg_battle2k3_skill = DBString(kDefaultTerm);
		DBString easyrpg_battle2k3_item = DBString(kDefaultTerm);
	};

	inline bool operator==(const Terms& l, const Terms& r) {
		return l.encounter == r.encounter
		&& l.special_combat == r.special_combat
		&& l.escape_success == r.escape_success
		&& l.escape_failure == r.escape_failure
		&& l.victory == r.victory
		&& l.defeat == r.defeat
		&& l.exp_received == r.exp_received
		&& l.gold_recieved_a == r.gold_recieved_a
		&& l.gold_recieved_b == r.gold_recieved_b
		&& l.item_recieved == r.item_recieved
		&& l.attacking == r.attacking
		&& l.enemy_critical == r.enemy_critical
		&& l.actor_critical == r.actor_critical
		&& l.defending == r.defending
		&& l.observing == r.observing
		&& l.focus == r.focus
		&& l.autodestruction == r.autodestruction
		&& l.enemy_escape == r.enemy_escape
		&& l.enemy_transform == r.enemy_transform
		&& l.enemy_damaged == r.enemy_damaged
		&& l.enemy_undamaged == r.enemy_undamaged
		&& l.actor_damaged == r.actor_damaged
		&& l.actor_undamaged == r.actor_undamaged
		&& l.skill_failure_a == r.skill_failure_a
		&& l.skill_failure_b == r.skill_failure_b
		&& l.skill_failure_c == r.skill_failure_c
		&& l.dodge == r.dodge
		&& l.use_item == r.use_item
		&& l.hp_recovery == r.hp_recovery
		&& l.parameter_increase == r.parameter_increase
		&& l.parameter_decrease == r.parameter_decrease
		&& l.enemy_hp_absorbed == r.enemy_hp_absorbed
		&& l.actor_hp_absorbed == r.actor_hp_absorbed
		&& l.resistance_increase == r.resistance_increase
		&& l.resistance_decrease == r.resistance_decrease
		&& l.level_up == r.level_up
		&& l.skill_learned == r.skill_learned
		&& l.battle_start == r.battle_start
		&& l.miss == r.miss
		&& l.shop_greeting1 == r.shop_greeting1
		&& l.shop_regreeting1 == r.shop_regreeting1
		&& l.shop_buy1 == r.shop_buy1
		&& l.shop_sell1 == r.shop_sell1
		&& l.shop_leave1 == r.shop_leave1
		&& l.shop_buy_select1 == r.shop_buy_select1
		&& l.shop_buy_number1 == r.shop_buy_number1
		&& l.shop_purchased1 == r.shop_purchased1
		&& l.shop_sell_select1 == r.shop_sell_select1
		&& l.shop_sell_number1 == r.shop_sell_number1
		&& l.shop_sold1 == r.shop_sold1
		&& l.shop_greeting2 == r.shop_greeting2
		&& l.shop_regreeting2 == r.shop_regreeting2
		&& l.shop_buy2 == r.shop_buy2
		&& l.shop_sell2 == r.shop_sell2
		&& l.shop_leave2 == r.shop_leave2
		&& l.shop_buy_select2 == r.shop_buy_select2
		&& l.shop_buy_number2 == r.shop_buy_number2
		&& l.shop_purchased2 == r.shop_purchased2
		&& l.shop_sell_select2 == r.shop_sell_select2
		&& l.shop_sell_number2 == r.shop_sell_number2
		&& l.shop_sold2 == r.shop_sold2
		&& l.shop_greeting3 == r.shop_greeting3
		&& l.shop_regreeting3 == r.shop_regreeting3
		&& l.shop_buy3 == r.shop_buy3
		&& l.shop_sell3 == r.shop_sell3
		&& l.shop_leave3 == r.shop_leave3
		&& l.shop_buy_select3 == r.shop_buy_select3
		&& l.shop_buy_number3 == r.shop_buy_number3
		&& l.shop_purchased3 == r.shop_purchased3
		&& l.shop_sell_select3 == r.shop_sell_select3
		&& l.shop_sell_number3 == r.shop_sell_number3
		&& l.shop_sold3 == r.shop_sold3
		&& l.inn_a_greeting_1 == r.inn_a_greeting_1
		&& l.inn_a_greeting_2 == r.inn_a_greeting_2
		&& l.inn_a_greeting_3 == r.inn_a_greeting_3
		&& l.inn_a_accept == r.inn_a_accept
		&& l.inn_a_cancel == r.inn_a_cancel
		&& l.inn_b_greeting_1 == r.inn_b_greeting_1
		&& l.inn_b_greeting_2 == r.inn_b_greeting_2
		&& l.inn_b_greeting_3 == r.inn_b_greeting_3
		&& l.inn_b_accept == r.inn_b_accept
		&& l.inn_b_cancel == r.inn_b_cancel
		&& l.possessed_items == r.possessed_items
		&& l.equipped_items == r.equipped_items
		&& l.gold == r.gold
		&& l.battle_fight == r.battle_fight
		&& l.battle_auto == r.battle_auto
		&& l.battle_escape == r.battle_escape
		&& l.command_attack == r.command_attack
		&& l.command_defend == r.command_defend
		&& l.command_item == r.command_item
		&& l.command_skill == r.command_skill
		&& l.menu_equipment == r.menu_equipment
		&& l.menu_save == r.menu_save
		&& l.menu_quit == r.menu_quit
		&& l.new_game == r.new_game
		&& l.load_game == r.load_game
		&& l.exit_game == r.exit_game
		&& l.status == r.status
		&& l.row == r.row
		&& l.order == r.order
		&& l.wait_on == r.wait_on
		&& l.wait_off == r.wait_off
		&& l.level == r.level
		&& l.health_points == r.health_points
		&& l.spirit_points == r.spirit_points
		&& l.normal_status == r.normal_status
		&& l.exp_short == r.exp_short
		&& l.lvl_short == r.lvl_short
		&& l.hp_short == r.hp_short
		&& l.sp_short == r.sp_short
		&& l.sp_cost == r.sp_cost
		&& l.attack == r.attack
		&& l.defense == r.defense
		&& l.spirit == r.spirit
		&& l.agility == r.agility
		&& l.weapon == r.weapon
		&& l.shield == r.shield
		&& l.armor == r.armor
		&& l.helmet == r.helmet
		&& l.accessory == r.accessory
		&& l.save_game_message == r.save_game_message
		&& l.load_game_message == r.load_game_message
		&& l.file == r.file
		&& l.exit_game_message == r.exit_game_message
		&& l.yes == r.yes
		&& l.no == r.no
		&& l.easyrpg_item_number_separator == r.easyrpg_item_number_separator
		&& l.easyrpg_skill_cost_separator == r.easyrpg_skill_cost_separator
		&& l.easyrpg_equipment_arrow == r.easyrpg_equipment_arrow
		&& l.easyrpg_status_scene_name == r.easyrpg_status_scene_name
		&& l.easyrpg_status_scene_class == r.easyrpg_status_scene_class
		&& l.easyrpg_status_scene_title == r.easyrpg_status_scene_title
		&& l.easyrpg_status_scene_condition == r.easyrpg_status_scene_condition
		&& l.easyrpg_status_scene_front == r.easyrpg_status_scene_front
		&& l.easyrpg_status_scene_back == r.easyrpg_status_scene_back
		&& l.easyrpg_order_scene_confirm == r.easyrpg_order_scene_confirm
		&& l.easyrpg_order_scene_redo == r.easyrpg_order_scene_redo
		&& l.easyrpg_battle2k3_double_attack == r.easyrpg_battle2k3_double_attack
		&& l.easyrpg_battle2k3_defend == r.easyrpg_battle2k3_defend
		&& l.easyrpg_battle2k3_observe == r.easyrpg_battle2k3_observe
		&& l.easyrpg_battle2k3_charge == r.easyrpg_battle2k3_charge
		&& l.easyrpg_battle2k3_selfdestruct == r.easyrpg_battle2k3_selfdestruct
		&& l.easyrpg_battle2k3_escape == r.easyrpg_battle2k3_escape
		&& l.easyrpg_battle2k3_special_combat_back == r.easyrpg_battle2k3_special_combat_back
		&& l.easyrpg_battle2k3_skill == r.easyrpg_battle2k3_skill
		&& l.easyrpg_battle2k3_item == r.easyrpg_battle2k3_item;
	}

	inline bool operator!=(const Terms& l, const Terms& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Terms& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Terms& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Terms, ParentCtx>{ "encounter", -1, &obj, parent_ctx };
		f(obj.encounter, ctx1);
		const auto ctx2 = Context<Terms, ParentCtx>{ "special_combat", -1, &obj, parent_ctx };
		f(obj.special_combat, ctx2);
		const auto ctx3 = Context<Terms, ParentCtx>{ "escape_success", -1, &obj, parent_ctx };
		f(obj.escape_success, ctx3);
		const auto ctx4 = Context<Terms, ParentCtx>{ "escape_failure", -1, &obj, parent_ctx };
		f(obj.escape_failure, ctx4);
		const auto ctx5 = Context<Terms, ParentCtx>{ "victory", -1, &obj, parent_ctx };
		f(obj.victory, ctx5);
		const auto ctx6 = Context<Terms, ParentCtx>{ "defeat", -1, &obj, parent_ctx };
		f(obj.defeat, ctx6);
		const auto ctx7 = Context<Terms, ParentCtx>{ "exp_received", -1, &obj, parent_ctx };
		f(obj.exp_received, ctx7);
		const auto ctx8 = Context<Terms, ParentCtx>{ "gold_recieved_a", -1, &obj, parent_ctx };
		f(obj.gold_recieved_a, ctx8);
		const auto ctx9 = Context<Terms, ParentCtx>{ "gold_recieved_b", -1, &obj, parent_ctx };
		f(obj.gold_recieved_b, ctx9);
		const auto ctx10 = Context<Terms, ParentCtx>{ "item_recieved", -1, &obj, parent_ctx };
		f(obj.item_recieved, ctx10);
		const auto ctx11 = Context<Terms, ParentCtx>{ "attacking", -1, &obj, parent_ctx };
		f(obj.attacking, ctx11);
		const auto ctx12 = Context<Terms, ParentCtx>{ "enemy_critical", -1, &obj, parent_ctx };
		f(obj.enemy_critical, ctx12);
		const auto ctx13 = Context<Terms, ParentCtx>{ "actor_critical", -1, &obj, parent_ctx };
		f(obj.actor_critical, ctx13);
		const auto ctx14 = Context<Terms, ParentCtx>{ "defending", -1, &obj, parent_ctx };
		f(obj.defending, ctx14);
		const auto ctx15 = Context<Terms, ParentCtx>{ "observing", -1, &obj, parent_ctx };
		f(obj.observing, ctx15);
		const auto ctx16 = Context<Terms, ParentCtx>{ "focus", -1, &obj, parent_ctx };
		f(obj.focus, ctx16);
		const auto ctx17 = Context<Terms, ParentCtx>{ "autodestruction", -1, &obj, parent_ctx };
		f(obj.autodestruction, ctx17);
		const auto ctx18 = Context<Terms, ParentCtx>{ "enemy_escape", -1, &obj, parent_ctx };
		f(obj.enemy_escape, ctx18);
		const auto ctx19 = Context<Terms, ParentCtx>{ "enemy_transform", -1, &obj, parent_ctx };
		f(obj.enemy_transform, ctx19);
		const auto ctx20 = Context<Terms, ParentCtx>{ "enemy_damaged", -1, &obj, parent_ctx };
		f(obj.enemy_damaged, ctx20);
		const auto ctx21 = Context<Terms, ParentCtx>{ "enemy_undamaged", -1, &obj, parent_ctx };
		f(obj.enemy_undamaged, ctx21);
		const auto ctx22 = Context<Terms, ParentCtx>{ "actor_damaged", -1, &obj, parent_ctx };
		f(obj.actor_damaged, ctx22);
		const auto ctx23 = Context<Terms, ParentCtx>{ "actor_undamaged", -1, &obj, parent_ctx };
		f(obj.actor_undamaged, ctx23);
		const auto ctx24 = Context<Terms, ParentCtx>{ "skill_failure_a", -1, &obj, parent_ctx };
		f(obj.skill_failure_a, ctx24);
		const auto ctx25 = Context<Terms, ParentCtx>{ "skill_failure_b", -1, &obj, parent_ctx };
		f(obj.skill_failure_b, ctx25);
		const auto ctx26 = Context<Terms, ParentCtx>{ "skill_failure_c", -1, &obj, parent_ctx };
		f(obj.skill_failure_c, ctx26);
		const auto ctx27 = Context<Terms, ParentCtx>{ "dodge", -1, &obj, parent_ctx };
		f(obj.dodge, ctx27);
		const auto ctx28 = Context<Terms, ParentCtx>{ "use_item", -1, &obj, parent_ctx };
		f(obj.use_item, ctx28);
		const auto ctx29 = Context<Terms, ParentCtx>{ "hp_recovery", -1, &obj, parent_ctx };
		f(obj.hp_recovery, ctx29);
		const auto ctx30 = Context<Terms, ParentCtx>{ "parameter_increase", -1, &obj, parent_ctx };
		f(obj.parameter_increase, ctx30);
		const auto ctx31 = Context<Terms, ParentCtx>{ "parameter_decrease", -1, &obj, parent_ctx };
		f(obj.parameter_decrease, ctx31);
		const auto ctx32 = Context<Terms, ParentCtx>{ "enemy_hp_absorbed", -1, &obj, parent_ctx };
		f(obj.enemy_hp_absorbed, ctx32);
		const auto ctx33 = Context<Terms, ParentCtx>{ "actor_hp_absorbed", -1, &obj, parent_ctx };
		f(obj.actor_hp_absorbed, ctx33);
		const auto ctx34 = Context<Terms, ParentCtx>{ "resistance_increase", -1, &obj, parent_ctx };
		f(obj.resistance_increase, ctx34);
		const auto ctx35 = Context<Terms, ParentCtx>{ "resistance_decrease", -1, &obj, parent_ctx };
		f(obj.resistance_decrease, ctx35);
		const auto ctx36 = Context<Terms, ParentCtx>{ "level_up", -1, &obj, parent_ctx };
		f(obj.level_up, ctx36);
		const auto ctx37 = Context<Terms, ParentCtx>{ "skill_learned", -1, &obj, parent_ctx };
		f(obj.skill_learned, ctx37);
		const auto ctx38 = Context<Terms, ParentCtx>{ "battle_start", -1, &obj, parent_ctx };
		f(obj.battle_start, ctx38);
		const auto ctx39 = Context<Terms, ParentCtx>{ "miss", -1, &obj, parent_ctx };
		f(obj.miss, ctx39);
		const auto ctx40 = Context<Terms, ParentCtx>{ "shop_greeting1", -1, &obj, parent_ctx };
		f(obj.shop_greeting1, ctx40);
		const auto ctx41 = Context<Terms, ParentCtx>{ "shop_regreeting1", -1, &obj, parent_ctx };
		f(obj.shop_regreeting1, ctx41);
		const auto ctx42 = Context<Terms, ParentCtx>{ "shop_buy1", -1, &obj, parent_ctx };
		f(obj.shop_buy1, ctx42);
		const auto ctx43 = Context<Terms, ParentCtx>{ "shop_sell1", -1, &obj, parent_ctx };
		f(obj.shop_sell1, ctx43);
		const auto ctx44 = Context<Terms, ParentCtx>{ "shop_leave1", -1, &obj, parent_ctx };
		f(obj.shop_leave1, ctx44);
		const auto ctx45 = Context<Terms, ParentCtx>{ "shop_buy_select1", -1, &obj, parent_ctx };
		f(obj.shop_buy_select1, ctx45);
		const auto ctx46 = Context<Terms, ParentCtx>{ "shop_buy_number1", -1, &obj, parent_ctx };
		f(obj.shop_buy_number1, ctx46);
		const auto ctx47 = Context<Terms, ParentCtx>{ "shop_purchased1", -1, &obj, parent_ctx };
		f(obj.shop_purchased1, ctx47);
		const auto ctx48 = Context<Terms, ParentCtx>{ "shop_sell_select1", -1, &obj, parent_ctx };
		f(obj.shop_sell_select1, ctx48);
		const auto ctx49 = Context<Terms, ParentCtx>{ "shop_sell_number1", -1, &obj, parent_ctx };
		f(obj.shop_sell_number1, ctx49);
		const auto ctx50 = Context<Terms, ParentCtx>{ "shop_sold1", -1, &obj, parent_ctx };
		f(obj.shop_sold1, ctx50);
		const auto ctx51 = Context<Terms, ParentCtx>{ "shop_greeting2", -1, &obj, parent_ctx };
		f(obj.shop_greeting2, ctx51);
		const auto ctx52 = Context<Terms, ParentCtx>{ "shop_regreeting2", -1, &obj, parent_ctx };
		f(obj.shop_regreeting2, ctx52);
		const auto ctx53 = Context<Terms, ParentCtx>{ "shop_buy2", -1, &obj, parent_ctx };
		f(obj.shop_buy2, ctx53);
		const auto ctx54 = Context<Terms, ParentCtx>{ "shop_sell2", -1, &obj, parent_ctx };
		f(obj.shop_sell2, ctx54);
		const auto ctx55 = Context<Terms, ParentCtx>{ "shop_leave2", -1, &obj, parent_ctx };
		f(obj.shop_leave2, ctx55);
		const auto ctx56 = Context<Terms, ParentCtx>{ "shop_buy_select2", -1, &obj, parent_ctx };
		f(obj.shop_buy_select2, ctx56);
		const auto ctx57 = Context<Terms, ParentCtx>{ "shop_buy_number2", -1, &obj, parent_ctx };
		f(obj.shop_buy_number2, ctx57);
		const auto ctx58 = Context<Terms, ParentCtx>{ "shop_purchased2", -1, &obj, parent_ctx };
		f(obj.shop_purchased2, ctx58);
		const auto ctx59 = Context<Terms, ParentCtx>{ "shop_sell_select2", -1, &obj, parent_ctx };
		f(obj.shop_sell_select2, ctx59);
		const auto ctx60 = Context<Terms, ParentCtx>{ "shop_sell_number2", -1, &obj, parent_ctx };
		f(obj.shop_sell_number2, ctx60);
		const auto ctx61 = Context<Terms, ParentCtx>{ "shop_sold2", -1, &obj, parent_ctx };
		f(obj.shop_sold2, ctx61);
		const auto ctx62 = Context<Terms, ParentCtx>{ "shop_greeting3", -1, &obj, parent_ctx };
		f(obj.shop_greeting3, ctx62);
		const auto ctx63 = Context<Terms, ParentCtx>{ "shop_regreeting3", -1, &obj, parent_ctx };
		f(obj.shop_regreeting3, ctx63);
		const auto ctx64 = Context<Terms, ParentCtx>{ "shop_buy3", -1, &obj, parent_ctx };
		f(obj.shop_buy3, ctx64);
		const auto ctx65 = Context<Terms, ParentCtx>{ "shop_sell3", -1, &obj, parent_ctx };
		f(obj.shop_sell3, ctx65);
		const auto ctx66 = Context<Terms, ParentCtx>{ "shop_leave3", -1, &obj, parent_ctx };
		f(obj.shop_leave3, ctx66);
		const auto ctx67 = Context<Terms, ParentCtx>{ "shop_buy_select3", -1, &obj, parent_ctx };
		f(obj.shop_buy_select3, ctx67);
		const auto ctx68 = Context<Terms, ParentCtx>{ "shop_buy_number3", -1, &obj, parent_ctx };
		f(obj.shop_buy_number3, ctx68);
		const auto ctx69 = Context<Terms, ParentCtx>{ "shop_purchased3", -1, &obj, parent_ctx };
		f(obj.shop_purchased3, ctx69);
		const auto ctx70 = Context<Terms, ParentCtx>{ "shop_sell_select3", -1, &obj, parent_ctx };
		f(obj.shop_sell_select3, ctx70);
		const auto ctx71 = Context<Terms, ParentCtx>{ "shop_sell_number3", -1, &obj, parent_ctx };
		f(obj.shop_sell_number3, ctx71);
		const auto ctx72 = Context<Terms, ParentCtx>{ "shop_sold3", -1, &obj, parent_ctx };
		f(obj.shop_sold3, ctx72);
		const auto ctx73 = Context<Terms, ParentCtx>{ "inn_a_greeting_1", -1, &obj, parent_ctx };
		f(obj.inn_a_greeting_1, ctx73);
		const auto ctx74 = Context<Terms, ParentCtx>{ "inn_a_greeting_2", -1, &obj, parent_ctx };
		f(obj.inn_a_greeting_2, ctx74);
		const auto ctx75 = Context<Terms, ParentCtx>{ "inn_a_greeting_3", -1, &obj, parent_ctx };
		f(obj.inn_a_greeting_3, ctx75);
		const auto ctx76 = Context<Terms, ParentCtx>{ "inn_a_accept", -1, &obj, parent_ctx };
		f(obj.inn_a_accept, ctx76);
		const auto ctx77 = Context<Terms, ParentCtx>{ "inn_a_cancel", -1, &obj, parent_ctx };
		f(obj.inn_a_cancel, ctx77);
		const auto ctx78 = Context<Terms, ParentCtx>{ "inn_b_greeting_1", -1, &obj, parent_ctx };
		f(obj.inn_b_greeting_1, ctx78);
		const auto ctx79 = Context<Terms, ParentCtx>{ "inn_b_greeting_2", -1, &obj, parent_ctx };
		f(obj.inn_b_greeting_2, ctx79);
		const auto ctx80 = Context<Terms, ParentCtx>{ "inn_b_greeting_3", -1, &obj, parent_ctx };
		f(obj.inn_b_greeting_3, ctx80);
		const auto ctx81 = Context<Terms, ParentCtx>{ "inn_b_accept", -1, &obj, parent_ctx };
		f(obj.inn_b_accept, ctx81);
		const auto ctx82 = Context<Terms, ParentCtx>{ "inn_b_cancel", -1, &obj, parent_ctx };
		f(obj.inn_b_cancel, ctx82);
		const auto ctx83 = Context<Terms, ParentCtx>{ "possessed_items", -1, &obj, parent_ctx };
		f(obj.possessed_items, ctx83);
		const auto ctx84 = Context<Terms, ParentCtx>{ "equipped_items", -1, &obj, parent_ctx };
		f(obj.equipped_items, ctx84);
		const auto ctx85 = Context<Terms, ParentCtx>{ "gold", -1, &obj, parent_ctx };
		f(obj.gold, ctx85);
		const auto ctx86 = Context<Terms, ParentCtx>{ "battle_fight", -1, &obj, parent_ctx };
		f(obj.battle_fight, ctx86);
		const auto ctx87 = Context<Terms, ParentCtx>{ "battle_auto", -1, &obj, parent_ctx };
		f(obj.battle_auto, ctx87);
		const auto ctx88 = Context<Terms, ParentCtx>{ "battle_escape", -1, &obj, parent_ctx };
		f(obj.battle_escape, ctx88);
		const auto ctx89 = Context<Terms, ParentCtx>{ "command_attack", -1, &obj, parent_ctx };
		f(obj.command_attack, ctx89);
		const auto ctx90 = Context<Terms, ParentCtx>{ "command_defend", -1, &obj, parent_ctx };
		f(obj.command_defend, ctx90);
		const auto ctx91 = Context<Terms, ParentCtx>{ "command_item", -1, &obj, parent_ctx };
		f(obj.command_item, ctx91);
		const auto ctx92 = Context<Terms, ParentCtx>{ "command_skill", -1, &obj, parent_ctx };
		f(obj.command_skill, ctx92);
		const auto ctx93 = Context<Terms, ParentCtx>{ "menu_equipment", -1, &obj, parent_ctx };
		f(obj.menu_equipment, ctx93);
		const auto ctx94 = Context<Terms, ParentCtx>{ "menu_save", -1, &obj, parent_ctx };
		f(obj.menu_save, ctx94);
		const auto ctx95 = Context<Terms, ParentCtx>{ "menu_quit", -1, &obj, parent_ctx };
		f(obj.menu_quit, ctx95);
		const auto ctx96 = Context<Terms, ParentCtx>{ "new_game", -1, &obj, parent_ctx };
		f(obj.new_game, ctx96);
		const auto ctx97 = Context<Terms, ParentCtx>{ "load_game", -1, &obj, parent_ctx };
		f(obj.load_game, ctx97);
		const auto ctx98 = Context<Terms, ParentCtx>{ "exit_game", -1, &obj, parent_ctx };
		f(obj.exit_game, ctx98);
		const auto ctx99 = Context<Terms, ParentCtx>{ "status", -1, &obj, parent_ctx };
		f(obj.status, ctx99);
		const auto ctx100 = Context<Terms, ParentCtx>{ "row", -1, &obj, parent_ctx };
		f(obj.row, ctx100);
		const auto ctx101 = Context<Terms, ParentCtx>{ "order", -1, &obj, parent_ctx };
		f(obj.order, ctx101);
		const auto ctx102 = Context<Terms, ParentCtx>{ "wait_on", -1, &obj, parent_ctx };
		f(obj.wait_on, ctx102);
		const auto ctx103 = Context<Terms, ParentCtx>{ "wait_off", -1, &obj, parent_ctx };
		f(obj.wait_off, ctx103);
		const auto ctx104 = Context<Terms, ParentCtx>{ "level", -1, &obj, parent_ctx };
		f(obj.level, ctx104);
		const auto ctx105 = Context<Terms, ParentCtx>{ "health_points", -1, &obj, parent_ctx };
		f(obj.health_points, ctx105);
		const auto ctx106 = Context<Terms, ParentCtx>{ "spirit_points", -1, &obj, parent_ctx };
		f(obj.spirit_points, ctx106);
		const auto ctx107 = Context<Terms, ParentCtx>{ "normal_status", -1, &obj, parent_ctx };
		f(obj.normal_status, ctx107);
		const auto ctx108 = Context<Terms, ParentCtx>{ "exp_short", -1, &obj, parent_ctx };
		f(obj.exp_short, ctx108);
		const auto ctx109 = Context<Terms, ParentCtx>{ "lvl_short", -1, &obj, parent_ctx };
		f(obj.lvl_short, ctx109);
		const auto ctx110 = Context<Terms, ParentCtx>{ "hp_short", -1, &obj, parent_ctx };
		f(obj.hp_short, ctx110);
		const auto ctx111 = Context<Terms, ParentCtx>{ "sp_short", -1, &obj, parent_ctx };
		f(obj.sp_short, ctx111);
		const auto ctx112 = Context<Terms, ParentCtx>{ "sp_cost", -1, &obj, parent_ctx };
		f(obj.sp_cost, ctx112);
		const auto ctx113 = Context<Terms, ParentCtx>{ "attack", -1, &obj, parent_ctx };
		f(obj.attack, ctx113);
		const auto ctx114 = Context<Terms, ParentCtx>{ "defense", -1, &obj, parent_ctx };
		f(obj.defense, ctx114);
		const auto ctx115 = Context<Terms, ParentCtx>{ "spirit", -1, &obj, parent_ctx };
		f(obj.spirit, ctx115);
		const auto ctx116 = Context<Terms, ParentCtx>{ "agility", -1, &obj, parent_ctx };
		f(obj.agility, ctx116);
		const auto ctx117 = Context<Terms, ParentCtx>{ "weapon", -1, &obj, parent_ctx };
		f(obj.weapon, ctx117);
		const auto ctx118 = Context<Terms, ParentCtx>{ "shield", -1, &obj, parent_ctx };
		f(obj.shield, ctx118);
		const auto ctx119 = Context<Terms, ParentCtx>{ "armor", -1, &obj, parent_ctx };
		f(obj.armor, ctx119);
		const auto ctx120 = Context<Terms, ParentCtx>{ "helmet", -1, &obj, parent_ctx };
		f(obj.helmet, ctx120);
		const auto ctx121 = Context<Terms, ParentCtx>{ "accessory", -1, &obj, parent_ctx };
		f(obj.accessory, ctx121);
		const auto ctx122 = Context<Terms, ParentCtx>{ "save_game_message", -1, &obj, parent_ctx };
		f(obj.save_game_message, ctx122);
		const auto ctx123 = Context<Terms, ParentCtx>{ "load_game_message", -1, &obj, parent_ctx };
		f(obj.load_game_message, ctx123);
		const auto ctx124 = Context<Terms, ParentCtx>{ "file", -1, &obj, parent_ctx };
		f(obj.file, ctx124);
		const auto ctx125 = Context<Terms, ParentCtx>{ "exit_game_message", -1, &obj, parent_ctx };
		f(obj.exit_game_message, ctx125);
		const auto ctx126 = Context<Terms, ParentCtx>{ "yes", -1, &obj, parent_ctx };
		f(obj.yes, ctx126);
		const auto ctx127 = Context<Terms, ParentCtx>{ "no", -1, &obj, parent_ctx };
		f(obj.no, ctx127);
		const auto ctx128 = Context<Terms, ParentCtx>{ "easyrpg_item_number_separator", -1, &obj, parent_ctx };
		f(obj.easyrpg_item_number_separator, ctx128);
		const auto ctx129 = Context<Terms, ParentCtx>{ "easyrpg_skill_cost_separator", -1, &obj, parent_ctx };
		f(obj.easyrpg_skill_cost_separator, ctx129);
		const auto ctx130 = Context<Terms, ParentCtx>{ "easyrpg_equipment_arrow", -1, &obj, parent_ctx };
		f(obj.easyrpg_equipment_arrow, ctx130);
		const auto ctx131 = Context<Terms, ParentCtx>{ "easyrpg_status_scene_name", -1, &obj, parent_ctx };
		f(obj.easyrpg_status_scene_name, ctx131);
		const auto ctx132 = Context<Terms, ParentCtx>{ "easyrpg_status_scene_class", -1, &obj, parent_ctx };
		f(obj.easyrpg_status_scene_class, ctx132);
		const auto ctx133 = Context<Terms, ParentCtx>{ "easyrpg_status_scene_title", -1, &obj, parent_ctx };
		f(obj.easyrpg_status_scene_title, ctx133);
		const auto ctx134 = Context<Terms, ParentCtx>{ "easyrpg_status_scene_condition", -1, &obj, parent_ctx };
		f(obj.easyrpg_status_scene_condition, ctx134);
		const auto ctx135 = Context<Terms, ParentCtx>{ "easyrpg_status_scene_front", -1, &obj, parent_ctx };
		f(obj.easyrpg_status_scene_front, ctx135);
		const auto ctx136 = Context<Terms, ParentCtx>{ "easyrpg_status_scene_back", -1, &obj, parent_ctx };
		f(obj.easyrpg_status_scene_back, ctx136);
		const auto ctx137 = Context<Terms, ParentCtx>{ "easyrpg_order_scene_confirm", -1, &obj, parent_ctx };
		f(obj.easyrpg_order_scene_confirm, ctx137);
		const auto ctx138 = Context<Terms, ParentCtx>{ "easyrpg_order_scene_redo", -1, &obj, parent_ctx };
		f(obj.easyrpg_order_scene_redo, ctx138);
		const auto ctx139 = Context<Terms, ParentCtx>{ "easyrpg_battle2k3_double_attack", -1, &obj, parent_ctx };
		f(obj.easyrpg_battle2k3_double_attack, ctx139);
		const auto ctx140 = Context<Terms, ParentCtx>{ "easyrpg_battle2k3_defend", -1, &obj, parent_ctx };
		f(obj.easyrpg_battle2k3_defend, ctx140);
		const auto ctx141 = Context<Terms, ParentCtx>{ "easyrpg_battle2k3_observe", -1, &obj, parent_ctx };
		f(obj.easyrpg_battle2k3_observe, ctx141);
		const auto ctx142 = Context<Terms, ParentCtx>{ "easyrpg_battle2k3_charge", -1, &obj, parent_ctx };
		f(obj.easyrpg_battle2k3_charge, ctx142);
		const auto ctx143 = Context<Terms, ParentCtx>{ "easyrpg_battle2k3_selfdestruct", -1, &obj, parent_ctx };
		f(obj.easyrpg_battle2k3_selfdestruct, ctx143);
		const auto ctx144 = Context<Terms, ParentCtx>{ "easyrpg_battle2k3_escape", -1, &obj, parent_ctx };
		f(obj.easyrpg_battle2k3_escape, ctx144);
		const auto ctx145 = Context<Terms, ParentCtx>{ "easyrpg_battle2k3_special_combat_back", -1, &obj, parent_ctx };
		f(obj.easyrpg_battle2k3_special_combat_back, ctx145);
		const auto ctx146 = Context<Terms, ParentCtx>{ "easyrpg_battle2k3_skill", -1, &obj, parent_ctx };
		f(obj.easyrpg_battle2k3_skill, ctx146);
		const auto ctx147 = Context<Terms, ParentCtx>{ "easyrpg_battle2k3_item", -1, &obj, parent_ctx };
		f(obj.easyrpg_battle2k3_item, ctx147);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
