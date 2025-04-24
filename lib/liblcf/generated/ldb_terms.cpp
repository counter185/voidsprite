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

// Headers
#include "lcf/ldb/reader.h"
#include "lcf/ldb/chunks.h"
#include "reader_struct_impl.h"

namespace lcf {

// Read Terms.

template <>
char const* const Struct<rpg::Terms>::name = "Terms";
static TypedField<rpg::Terms, DBString> static_encounter(
	&rpg::Terms::encounter,
	LDB_Reader::ChunkTerms::encounter,
	"encounter",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_special_combat(
	&rpg::Terms::special_combat,
	LDB_Reader::ChunkTerms::special_combat,
	"special_combat",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_escape_success(
	&rpg::Terms::escape_success,
	LDB_Reader::ChunkTerms::escape_success,
	"escape_success",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_escape_failure(
	&rpg::Terms::escape_failure,
	LDB_Reader::ChunkTerms::escape_failure,
	"escape_failure",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_victory(
	&rpg::Terms::victory,
	LDB_Reader::ChunkTerms::victory,
	"victory",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_defeat(
	&rpg::Terms::defeat,
	LDB_Reader::ChunkTerms::defeat,
	"defeat",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_exp_received(
	&rpg::Terms::exp_received,
	LDB_Reader::ChunkTerms::exp_received,
	"exp_received",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_gold_recieved_a(
	&rpg::Terms::gold_recieved_a,
	LDB_Reader::ChunkTerms::gold_recieved_a,
	"gold_recieved_a",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_gold_recieved_b(
	&rpg::Terms::gold_recieved_b,
	LDB_Reader::ChunkTerms::gold_recieved_b,
	"gold_recieved_b",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_item_recieved(
	&rpg::Terms::item_recieved,
	LDB_Reader::ChunkTerms::item_recieved,
	"item_recieved",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_attacking(
	&rpg::Terms::attacking,
	LDB_Reader::ChunkTerms::attacking,
	"attacking",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_enemy_critical(
	&rpg::Terms::enemy_critical,
	LDB_Reader::ChunkTerms::enemy_critical,
	"enemy_critical",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_actor_critical(
	&rpg::Terms::actor_critical,
	LDB_Reader::ChunkTerms::actor_critical,
	"actor_critical",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_defending(
	&rpg::Terms::defending,
	LDB_Reader::ChunkTerms::defending,
	"defending",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_observing(
	&rpg::Terms::observing,
	LDB_Reader::ChunkTerms::observing,
	"observing",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_focus(
	&rpg::Terms::focus,
	LDB_Reader::ChunkTerms::focus,
	"focus",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_autodestruction(
	&rpg::Terms::autodestruction,
	LDB_Reader::ChunkTerms::autodestruction,
	"autodestruction",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_enemy_escape(
	&rpg::Terms::enemy_escape,
	LDB_Reader::ChunkTerms::enemy_escape,
	"enemy_escape",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_enemy_transform(
	&rpg::Terms::enemy_transform,
	LDB_Reader::ChunkTerms::enemy_transform,
	"enemy_transform",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_enemy_damaged(
	&rpg::Terms::enemy_damaged,
	LDB_Reader::ChunkTerms::enemy_damaged,
	"enemy_damaged",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_enemy_undamaged(
	&rpg::Terms::enemy_undamaged,
	LDB_Reader::ChunkTerms::enemy_undamaged,
	"enemy_undamaged",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_actor_damaged(
	&rpg::Terms::actor_damaged,
	LDB_Reader::ChunkTerms::actor_damaged,
	"actor_damaged",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_actor_undamaged(
	&rpg::Terms::actor_undamaged,
	LDB_Reader::ChunkTerms::actor_undamaged,
	"actor_undamaged",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_skill_failure_a(
	&rpg::Terms::skill_failure_a,
	LDB_Reader::ChunkTerms::skill_failure_a,
	"skill_failure_a",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_skill_failure_b(
	&rpg::Terms::skill_failure_b,
	LDB_Reader::ChunkTerms::skill_failure_b,
	"skill_failure_b",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_skill_failure_c(
	&rpg::Terms::skill_failure_c,
	LDB_Reader::ChunkTerms::skill_failure_c,
	"skill_failure_c",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_dodge(
	&rpg::Terms::dodge,
	LDB_Reader::ChunkTerms::dodge,
	"dodge",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_use_item(
	&rpg::Terms::use_item,
	LDB_Reader::ChunkTerms::use_item,
	"use_item",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_hp_recovery(
	&rpg::Terms::hp_recovery,
	LDB_Reader::ChunkTerms::hp_recovery,
	"hp_recovery",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_parameter_increase(
	&rpg::Terms::parameter_increase,
	LDB_Reader::ChunkTerms::parameter_increase,
	"parameter_increase",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_parameter_decrease(
	&rpg::Terms::parameter_decrease,
	LDB_Reader::ChunkTerms::parameter_decrease,
	"parameter_decrease",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_enemy_hp_absorbed(
	&rpg::Terms::enemy_hp_absorbed,
	LDB_Reader::ChunkTerms::enemy_hp_absorbed,
	"enemy_hp_absorbed",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_actor_hp_absorbed(
	&rpg::Terms::actor_hp_absorbed,
	LDB_Reader::ChunkTerms::actor_hp_absorbed,
	"actor_hp_absorbed",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_resistance_increase(
	&rpg::Terms::resistance_increase,
	LDB_Reader::ChunkTerms::resistance_increase,
	"resistance_increase",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_resistance_decrease(
	&rpg::Terms::resistance_decrease,
	LDB_Reader::ChunkTerms::resistance_decrease,
	"resistance_decrease",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_level_up(
	&rpg::Terms::level_up,
	LDB_Reader::ChunkTerms::level_up,
	"level_up",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_skill_learned(
	&rpg::Terms::skill_learned,
	LDB_Reader::ChunkTerms::skill_learned,
	"skill_learned",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_battle_start(
	&rpg::Terms::battle_start,
	LDB_Reader::ChunkTerms::battle_start,
	"battle_start",
	1,
	1
);
static TypedField<rpg::Terms, DBString> static_miss(
	&rpg::Terms::miss,
	LDB_Reader::ChunkTerms::miss,
	"miss",
	1,
	1
);
static TypedField<rpg::Terms, DBString> static_shop_greeting1(
	&rpg::Terms::shop_greeting1,
	LDB_Reader::ChunkTerms::shop_greeting1,
	"shop_greeting1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_regreeting1(
	&rpg::Terms::shop_regreeting1,
	LDB_Reader::ChunkTerms::shop_regreeting1,
	"shop_regreeting1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_buy1(
	&rpg::Terms::shop_buy1,
	LDB_Reader::ChunkTerms::shop_buy1,
	"shop_buy1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sell1(
	&rpg::Terms::shop_sell1,
	LDB_Reader::ChunkTerms::shop_sell1,
	"shop_sell1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_leave1(
	&rpg::Terms::shop_leave1,
	LDB_Reader::ChunkTerms::shop_leave1,
	"shop_leave1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_buy_select1(
	&rpg::Terms::shop_buy_select1,
	LDB_Reader::ChunkTerms::shop_buy_select1,
	"shop_buy_select1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_buy_number1(
	&rpg::Terms::shop_buy_number1,
	LDB_Reader::ChunkTerms::shop_buy_number1,
	"shop_buy_number1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_purchased1(
	&rpg::Terms::shop_purchased1,
	LDB_Reader::ChunkTerms::shop_purchased1,
	"shop_purchased1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sell_select1(
	&rpg::Terms::shop_sell_select1,
	LDB_Reader::ChunkTerms::shop_sell_select1,
	"shop_sell_select1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sell_number1(
	&rpg::Terms::shop_sell_number1,
	LDB_Reader::ChunkTerms::shop_sell_number1,
	"shop_sell_number1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sold1(
	&rpg::Terms::shop_sold1,
	LDB_Reader::ChunkTerms::shop_sold1,
	"shop_sold1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_greeting2(
	&rpg::Terms::shop_greeting2,
	LDB_Reader::ChunkTerms::shop_greeting2,
	"shop_greeting2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_regreeting2(
	&rpg::Terms::shop_regreeting2,
	LDB_Reader::ChunkTerms::shop_regreeting2,
	"shop_regreeting2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_buy2(
	&rpg::Terms::shop_buy2,
	LDB_Reader::ChunkTerms::shop_buy2,
	"shop_buy2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sell2(
	&rpg::Terms::shop_sell2,
	LDB_Reader::ChunkTerms::shop_sell2,
	"shop_sell2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_leave2(
	&rpg::Terms::shop_leave2,
	LDB_Reader::ChunkTerms::shop_leave2,
	"shop_leave2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_buy_select2(
	&rpg::Terms::shop_buy_select2,
	LDB_Reader::ChunkTerms::shop_buy_select2,
	"shop_buy_select2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_buy_number2(
	&rpg::Terms::shop_buy_number2,
	LDB_Reader::ChunkTerms::shop_buy_number2,
	"shop_buy_number2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_purchased2(
	&rpg::Terms::shop_purchased2,
	LDB_Reader::ChunkTerms::shop_purchased2,
	"shop_purchased2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sell_select2(
	&rpg::Terms::shop_sell_select2,
	LDB_Reader::ChunkTerms::shop_sell_select2,
	"shop_sell_select2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sell_number2(
	&rpg::Terms::shop_sell_number2,
	LDB_Reader::ChunkTerms::shop_sell_number2,
	"shop_sell_number2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sold2(
	&rpg::Terms::shop_sold2,
	LDB_Reader::ChunkTerms::shop_sold2,
	"shop_sold2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_greeting3(
	&rpg::Terms::shop_greeting3,
	LDB_Reader::ChunkTerms::shop_greeting3,
	"shop_greeting3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_regreeting3(
	&rpg::Terms::shop_regreeting3,
	LDB_Reader::ChunkTerms::shop_regreeting3,
	"shop_regreeting3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_buy3(
	&rpg::Terms::shop_buy3,
	LDB_Reader::ChunkTerms::shop_buy3,
	"shop_buy3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sell3(
	&rpg::Terms::shop_sell3,
	LDB_Reader::ChunkTerms::shop_sell3,
	"shop_sell3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_leave3(
	&rpg::Terms::shop_leave3,
	LDB_Reader::ChunkTerms::shop_leave3,
	"shop_leave3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_buy_select3(
	&rpg::Terms::shop_buy_select3,
	LDB_Reader::ChunkTerms::shop_buy_select3,
	"shop_buy_select3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_buy_number3(
	&rpg::Terms::shop_buy_number3,
	LDB_Reader::ChunkTerms::shop_buy_number3,
	"shop_buy_number3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_purchased3(
	&rpg::Terms::shop_purchased3,
	LDB_Reader::ChunkTerms::shop_purchased3,
	"shop_purchased3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sell_select3(
	&rpg::Terms::shop_sell_select3,
	LDB_Reader::ChunkTerms::shop_sell_select3,
	"shop_sell_select3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sell_number3(
	&rpg::Terms::shop_sell_number3,
	LDB_Reader::ChunkTerms::shop_sell_number3,
	"shop_sell_number3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shop_sold3(
	&rpg::Terms::shop_sold3,
	LDB_Reader::ChunkTerms::shop_sold3,
	"shop_sold3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_inn_a_greeting_1(
	&rpg::Terms::inn_a_greeting_1,
	LDB_Reader::ChunkTerms::inn_a_greeting_1,
	"inn_a_greeting_1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_inn_a_greeting_2(
	&rpg::Terms::inn_a_greeting_2,
	LDB_Reader::ChunkTerms::inn_a_greeting_2,
	"inn_a_greeting_2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_inn_a_greeting_3(
	&rpg::Terms::inn_a_greeting_3,
	LDB_Reader::ChunkTerms::inn_a_greeting_3,
	"inn_a_greeting_3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_inn_a_accept(
	&rpg::Terms::inn_a_accept,
	LDB_Reader::ChunkTerms::inn_a_accept,
	"inn_a_accept",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_inn_a_cancel(
	&rpg::Terms::inn_a_cancel,
	LDB_Reader::ChunkTerms::inn_a_cancel,
	"inn_a_cancel",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_inn_b_greeting_1(
	&rpg::Terms::inn_b_greeting_1,
	LDB_Reader::ChunkTerms::inn_b_greeting_1,
	"inn_b_greeting_1",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_inn_b_greeting_2(
	&rpg::Terms::inn_b_greeting_2,
	LDB_Reader::ChunkTerms::inn_b_greeting_2,
	"inn_b_greeting_2",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_inn_b_greeting_3(
	&rpg::Terms::inn_b_greeting_3,
	LDB_Reader::ChunkTerms::inn_b_greeting_3,
	"inn_b_greeting_3",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_inn_b_accept(
	&rpg::Terms::inn_b_accept,
	LDB_Reader::ChunkTerms::inn_b_accept,
	"inn_b_accept",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_inn_b_cancel(
	&rpg::Terms::inn_b_cancel,
	LDB_Reader::ChunkTerms::inn_b_cancel,
	"inn_b_cancel",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_possessed_items(
	&rpg::Terms::possessed_items,
	LDB_Reader::ChunkTerms::possessed_items,
	"possessed_items",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_equipped_items(
	&rpg::Terms::equipped_items,
	LDB_Reader::ChunkTerms::equipped_items,
	"equipped_items",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_gold(
	&rpg::Terms::gold,
	LDB_Reader::ChunkTerms::gold,
	"gold",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_battle_fight(
	&rpg::Terms::battle_fight,
	LDB_Reader::ChunkTerms::battle_fight,
	"battle_fight",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_battle_auto(
	&rpg::Terms::battle_auto,
	LDB_Reader::ChunkTerms::battle_auto,
	"battle_auto",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_battle_escape(
	&rpg::Terms::battle_escape,
	LDB_Reader::ChunkTerms::battle_escape,
	"battle_escape",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_command_attack(
	&rpg::Terms::command_attack,
	LDB_Reader::ChunkTerms::command_attack,
	"command_attack",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_command_defend(
	&rpg::Terms::command_defend,
	LDB_Reader::ChunkTerms::command_defend,
	"command_defend",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_command_item(
	&rpg::Terms::command_item,
	LDB_Reader::ChunkTerms::command_item,
	"command_item",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_command_skill(
	&rpg::Terms::command_skill,
	LDB_Reader::ChunkTerms::command_skill,
	"command_skill",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_menu_equipment(
	&rpg::Terms::menu_equipment,
	LDB_Reader::ChunkTerms::menu_equipment,
	"menu_equipment",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_menu_save(
	&rpg::Terms::menu_save,
	LDB_Reader::ChunkTerms::menu_save,
	"menu_save",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_menu_quit(
	&rpg::Terms::menu_quit,
	LDB_Reader::ChunkTerms::menu_quit,
	"menu_quit",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_new_game(
	&rpg::Terms::new_game,
	LDB_Reader::ChunkTerms::new_game,
	"new_game",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_load_game(
	&rpg::Terms::load_game,
	LDB_Reader::ChunkTerms::load_game,
	"load_game",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_exit_game(
	&rpg::Terms::exit_game,
	LDB_Reader::ChunkTerms::exit_game,
	"exit_game",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_status(
	&rpg::Terms::status,
	LDB_Reader::ChunkTerms::status,
	"status",
	1,
	1
);
static TypedField<rpg::Terms, DBString> static_row(
	&rpg::Terms::row,
	LDB_Reader::ChunkTerms::row,
	"row",
	1,
	1
);
static TypedField<rpg::Terms, DBString> static_order(
	&rpg::Terms::order,
	LDB_Reader::ChunkTerms::order,
	"order",
	1,
	1
);
static TypedField<rpg::Terms, DBString> static_wait_on(
	&rpg::Terms::wait_on,
	LDB_Reader::ChunkTerms::wait_on,
	"wait_on",
	1,
	1
);
static TypedField<rpg::Terms, DBString> static_wait_off(
	&rpg::Terms::wait_off,
	LDB_Reader::ChunkTerms::wait_off,
	"wait_off",
	1,
	1
);
static TypedField<rpg::Terms, DBString> static_level(
	&rpg::Terms::level,
	LDB_Reader::ChunkTerms::level,
	"level",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_health_points(
	&rpg::Terms::health_points,
	LDB_Reader::ChunkTerms::health_points,
	"health_points",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_spirit_points(
	&rpg::Terms::spirit_points,
	LDB_Reader::ChunkTerms::spirit_points,
	"spirit_points",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_normal_status(
	&rpg::Terms::normal_status,
	LDB_Reader::ChunkTerms::normal_status,
	"normal_status",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_exp_short(
	&rpg::Terms::exp_short,
	LDB_Reader::ChunkTerms::exp_short,
	"exp_short",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_lvl_short(
	&rpg::Terms::lvl_short,
	LDB_Reader::ChunkTerms::lvl_short,
	"lvl_short",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_hp_short(
	&rpg::Terms::hp_short,
	LDB_Reader::ChunkTerms::hp_short,
	"hp_short",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_sp_short(
	&rpg::Terms::sp_short,
	LDB_Reader::ChunkTerms::sp_short,
	"sp_short",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_sp_cost(
	&rpg::Terms::sp_cost,
	LDB_Reader::ChunkTerms::sp_cost,
	"sp_cost",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_attack(
	&rpg::Terms::attack,
	LDB_Reader::ChunkTerms::attack,
	"attack",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_defense(
	&rpg::Terms::defense,
	LDB_Reader::ChunkTerms::defense,
	"defense",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_spirit(
	&rpg::Terms::spirit,
	LDB_Reader::ChunkTerms::spirit,
	"spirit",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_agility(
	&rpg::Terms::agility,
	LDB_Reader::ChunkTerms::agility,
	"agility",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_weapon(
	&rpg::Terms::weapon,
	LDB_Reader::ChunkTerms::weapon,
	"weapon",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_shield(
	&rpg::Terms::shield,
	LDB_Reader::ChunkTerms::shield,
	"shield",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_armor(
	&rpg::Terms::armor,
	LDB_Reader::ChunkTerms::armor,
	"armor",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_helmet(
	&rpg::Terms::helmet,
	LDB_Reader::ChunkTerms::helmet,
	"helmet",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_accessory(
	&rpg::Terms::accessory,
	LDB_Reader::ChunkTerms::accessory,
	"accessory",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_save_game_message(
	&rpg::Terms::save_game_message,
	LDB_Reader::ChunkTerms::save_game_message,
	"save_game_message",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_load_game_message(
	&rpg::Terms::load_game_message,
	LDB_Reader::ChunkTerms::load_game_message,
	"load_game_message",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_file(
	&rpg::Terms::file,
	LDB_Reader::ChunkTerms::file,
	"file",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_exit_game_message(
	&rpg::Terms::exit_game_message,
	LDB_Reader::ChunkTerms::exit_game_message,
	"exit_game_message",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_yes(
	&rpg::Terms::yes,
	LDB_Reader::ChunkTerms::yes,
	"yes",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_no(
	&rpg::Terms::no,
	LDB_Reader::ChunkTerms::no,
	"no",
	1,
	0
);
static TypedField<rpg::Terms, DBString> static_easyrpg_item_number_separator(
	&rpg::Terms::easyrpg_item_number_separator,
	LDB_Reader::ChunkTerms::easyrpg_item_number_separator,
	"easyrpg_item_number_separator",
	0,
	0
);
static TypedField<rpg::Terms, DBString> static_easyrpg_skill_cost_separator(
	&rpg::Terms::easyrpg_skill_cost_separator,
	LDB_Reader::ChunkTerms::easyrpg_skill_cost_separator,
	"easyrpg_skill_cost_separator",
	0,
	0
);
static TypedField<rpg::Terms, DBString> static_easyrpg_equipment_arrow(
	&rpg::Terms::easyrpg_equipment_arrow,
	LDB_Reader::ChunkTerms::easyrpg_equipment_arrow,
	"easyrpg_equipment_arrow",
	0,
	0
);
static TypedField<rpg::Terms, DBString> static_easyrpg_status_scene_name(
	&rpg::Terms::easyrpg_status_scene_name,
	LDB_Reader::ChunkTerms::easyrpg_status_scene_name,
	"easyrpg_status_scene_name",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_status_scene_class(
	&rpg::Terms::easyrpg_status_scene_class,
	LDB_Reader::ChunkTerms::easyrpg_status_scene_class,
	"easyrpg_status_scene_class",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_status_scene_title(
	&rpg::Terms::easyrpg_status_scene_title,
	LDB_Reader::ChunkTerms::easyrpg_status_scene_title,
	"easyrpg_status_scene_title",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_status_scene_condition(
	&rpg::Terms::easyrpg_status_scene_condition,
	LDB_Reader::ChunkTerms::easyrpg_status_scene_condition,
	"easyrpg_status_scene_condition",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_status_scene_front(
	&rpg::Terms::easyrpg_status_scene_front,
	LDB_Reader::ChunkTerms::easyrpg_status_scene_front,
	"easyrpg_status_scene_front",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_status_scene_back(
	&rpg::Terms::easyrpg_status_scene_back,
	LDB_Reader::ChunkTerms::easyrpg_status_scene_back,
	"easyrpg_status_scene_back",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_order_scene_confirm(
	&rpg::Terms::easyrpg_order_scene_confirm,
	LDB_Reader::ChunkTerms::easyrpg_order_scene_confirm,
	"easyrpg_order_scene_confirm",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_order_scene_redo(
	&rpg::Terms::easyrpg_order_scene_redo,
	LDB_Reader::ChunkTerms::easyrpg_order_scene_redo,
	"easyrpg_order_scene_redo",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_battle2k3_double_attack(
	&rpg::Terms::easyrpg_battle2k3_double_attack,
	LDB_Reader::ChunkTerms::easyrpg_battle2k3_double_attack,
	"easyrpg_battle2k3_double_attack",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_battle2k3_defend(
	&rpg::Terms::easyrpg_battle2k3_defend,
	LDB_Reader::ChunkTerms::easyrpg_battle2k3_defend,
	"easyrpg_battle2k3_defend",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_battle2k3_observe(
	&rpg::Terms::easyrpg_battle2k3_observe,
	LDB_Reader::ChunkTerms::easyrpg_battle2k3_observe,
	"easyrpg_battle2k3_observe",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_battle2k3_charge(
	&rpg::Terms::easyrpg_battle2k3_charge,
	LDB_Reader::ChunkTerms::easyrpg_battle2k3_charge,
	"easyrpg_battle2k3_charge",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_battle2k3_selfdestruct(
	&rpg::Terms::easyrpg_battle2k3_selfdestruct,
	LDB_Reader::ChunkTerms::easyrpg_battle2k3_selfdestruct,
	"easyrpg_battle2k3_selfdestruct",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_battle2k3_escape(
	&rpg::Terms::easyrpg_battle2k3_escape,
	LDB_Reader::ChunkTerms::easyrpg_battle2k3_escape,
	"easyrpg_battle2k3_escape",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_battle2k3_special_combat_back(
	&rpg::Terms::easyrpg_battle2k3_special_combat_back,
	LDB_Reader::ChunkTerms::easyrpg_battle2k3_special_combat_back,
	"easyrpg_battle2k3_special_combat_back",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_battle2k3_skill(
	&rpg::Terms::easyrpg_battle2k3_skill,
	LDB_Reader::ChunkTerms::easyrpg_battle2k3_skill,
	"easyrpg_battle2k3_skill",
	0,
	1
);
static TypedField<rpg::Terms, DBString> static_easyrpg_battle2k3_item(
	&rpg::Terms::easyrpg_battle2k3_item,
	LDB_Reader::ChunkTerms::easyrpg_battle2k3_item,
	"easyrpg_battle2k3_item",
	0,
	1
);


template <>
Field<rpg::Terms> const* Struct<rpg::Terms>::fields[] = {
	&static_encounter,
	&static_special_combat,
	&static_escape_success,
	&static_escape_failure,
	&static_victory,
	&static_defeat,
	&static_exp_received,
	&static_gold_recieved_a,
	&static_gold_recieved_b,
	&static_item_recieved,
	&static_attacking,
	&static_enemy_critical,
	&static_actor_critical,
	&static_defending,
	&static_observing,
	&static_focus,
	&static_autodestruction,
	&static_enemy_escape,
	&static_enemy_transform,
	&static_enemy_damaged,
	&static_enemy_undamaged,
	&static_actor_damaged,
	&static_actor_undamaged,
	&static_skill_failure_a,
	&static_skill_failure_b,
	&static_skill_failure_c,
	&static_dodge,
	&static_use_item,
	&static_hp_recovery,
	&static_parameter_increase,
	&static_parameter_decrease,
	&static_enemy_hp_absorbed,
	&static_actor_hp_absorbed,
	&static_resistance_increase,
	&static_resistance_decrease,
	&static_level_up,
	&static_skill_learned,
	&static_battle_start,
	&static_miss,
	&static_shop_greeting1,
	&static_shop_regreeting1,
	&static_shop_buy1,
	&static_shop_sell1,
	&static_shop_leave1,
	&static_shop_buy_select1,
	&static_shop_buy_number1,
	&static_shop_purchased1,
	&static_shop_sell_select1,
	&static_shop_sell_number1,
	&static_shop_sold1,
	&static_shop_greeting2,
	&static_shop_regreeting2,
	&static_shop_buy2,
	&static_shop_sell2,
	&static_shop_leave2,
	&static_shop_buy_select2,
	&static_shop_buy_number2,
	&static_shop_purchased2,
	&static_shop_sell_select2,
	&static_shop_sell_number2,
	&static_shop_sold2,
	&static_shop_greeting3,
	&static_shop_regreeting3,
	&static_shop_buy3,
	&static_shop_sell3,
	&static_shop_leave3,
	&static_shop_buy_select3,
	&static_shop_buy_number3,
	&static_shop_purchased3,
	&static_shop_sell_select3,
	&static_shop_sell_number3,
	&static_shop_sold3,
	&static_inn_a_greeting_1,
	&static_inn_a_greeting_2,
	&static_inn_a_greeting_3,
	&static_inn_a_accept,
	&static_inn_a_cancel,
	&static_inn_b_greeting_1,
	&static_inn_b_greeting_2,
	&static_inn_b_greeting_3,
	&static_inn_b_accept,
	&static_inn_b_cancel,
	&static_possessed_items,
	&static_equipped_items,
	&static_gold,
	&static_battle_fight,
	&static_battle_auto,
	&static_battle_escape,
	&static_command_attack,
	&static_command_defend,
	&static_command_item,
	&static_command_skill,
	&static_menu_equipment,
	&static_menu_save,
	&static_menu_quit,
	&static_new_game,
	&static_load_game,
	&static_exit_game,
	&static_status,
	&static_row,
	&static_order,
	&static_wait_on,
	&static_wait_off,
	&static_level,
	&static_health_points,
	&static_spirit_points,
	&static_normal_status,
	&static_exp_short,
	&static_lvl_short,
	&static_hp_short,
	&static_sp_short,
	&static_sp_cost,
	&static_attack,
	&static_defense,
	&static_spirit,
	&static_agility,
	&static_weapon,
	&static_shield,
	&static_armor,
	&static_helmet,
	&static_accessory,
	&static_save_game_message,
	&static_load_game_message,
	&static_file,
	&static_exit_game_message,
	&static_yes,
	&static_no,
	&static_easyrpg_item_number_separator,
	&static_easyrpg_skill_cost_separator,
	&static_easyrpg_equipment_arrow,
	&static_easyrpg_status_scene_name,
	&static_easyrpg_status_scene_class,
	&static_easyrpg_status_scene_title,
	&static_easyrpg_status_scene_condition,
	&static_easyrpg_status_scene_front,
	&static_easyrpg_status_scene_back,
	&static_easyrpg_order_scene_confirm,
	&static_easyrpg_order_scene_redo,
	&static_easyrpg_battle2k3_double_attack,
	&static_easyrpg_battle2k3_defend,
	&static_easyrpg_battle2k3_observe,
	&static_easyrpg_battle2k3_charge,
	&static_easyrpg_battle2k3_selfdestruct,
	&static_easyrpg_battle2k3_escape,
	&static_easyrpg_battle2k3_special_combat_back,
	&static_easyrpg_battle2k3_skill,
	&static_easyrpg_battle2k3_item,
	NULL
};

template class Struct<rpg::Terms>;

} //namespace lcf
