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
#include "lcf/rpg/terms.h"

constexpr const char* lcf::rpg::Terms::kDefaultTerm;
namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Terms& obj) {
	os << "Terms{";
	os << "encounter="<< obj.encounter;
	os << ", special_combat="<< obj.special_combat;
	os << ", escape_success="<< obj.escape_success;
	os << ", escape_failure="<< obj.escape_failure;
	os << ", victory="<< obj.victory;
	os << ", defeat="<< obj.defeat;
	os << ", exp_received="<< obj.exp_received;
	os << ", gold_recieved_a="<< obj.gold_recieved_a;
	os << ", gold_recieved_b="<< obj.gold_recieved_b;
	os << ", item_recieved="<< obj.item_recieved;
	os << ", attacking="<< obj.attacking;
	os << ", enemy_critical="<< obj.enemy_critical;
	os << ", actor_critical="<< obj.actor_critical;
	os << ", defending="<< obj.defending;
	os << ", observing="<< obj.observing;
	os << ", focus="<< obj.focus;
	os << ", autodestruction="<< obj.autodestruction;
	os << ", enemy_escape="<< obj.enemy_escape;
	os << ", enemy_transform="<< obj.enemy_transform;
	os << ", enemy_damaged="<< obj.enemy_damaged;
	os << ", enemy_undamaged="<< obj.enemy_undamaged;
	os << ", actor_damaged="<< obj.actor_damaged;
	os << ", actor_undamaged="<< obj.actor_undamaged;
	os << ", skill_failure_a="<< obj.skill_failure_a;
	os << ", skill_failure_b="<< obj.skill_failure_b;
	os << ", skill_failure_c="<< obj.skill_failure_c;
	os << ", dodge="<< obj.dodge;
	os << ", use_item="<< obj.use_item;
	os << ", hp_recovery="<< obj.hp_recovery;
	os << ", parameter_increase="<< obj.parameter_increase;
	os << ", parameter_decrease="<< obj.parameter_decrease;
	os << ", enemy_hp_absorbed="<< obj.enemy_hp_absorbed;
	os << ", actor_hp_absorbed="<< obj.actor_hp_absorbed;
	os << ", resistance_increase="<< obj.resistance_increase;
	os << ", resistance_decrease="<< obj.resistance_decrease;
	os << ", level_up="<< obj.level_up;
	os << ", skill_learned="<< obj.skill_learned;
	os << ", battle_start="<< obj.battle_start;
	os << ", miss="<< obj.miss;
	os << ", shop_greeting1="<< obj.shop_greeting1;
	os << ", shop_regreeting1="<< obj.shop_regreeting1;
	os << ", shop_buy1="<< obj.shop_buy1;
	os << ", shop_sell1="<< obj.shop_sell1;
	os << ", shop_leave1="<< obj.shop_leave1;
	os << ", shop_buy_select1="<< obj.shop_buy_select1;
	os << ", shop_buy_number1="<< obj.shop_buy_number1;
	os << ", shop_purchased1="<< obj.shop_purchased1;
	os << ", shop_sell_select1="<< obj.shop_sell_select1;
	os << ", shop_sell_number1="<< obj.shop_sell_number1;
	os << ", shop_sold1="<< obj.shop_sold1;
	os << ", shop_greeting2="<< obj.shop_greeting2;
	os << ", shop_regreeting2="<< obj.shop_regreeting2;
	os << ", shop_buy2="<< obj.shop_buy2;
	os << ", shop_sell2="<< obj.shop_sell2;
	os << ", shop_leave2="<< obj.shop_leave2;
	os << ", shop_buy_select2="<< obj.shop_buy_select2;
	os << ", shop_buy_number2="<< obj.shop_buy_number2;
	os << ", shop_purchased2="<< obj.shop_purchased2;
	os << ", shop_sell_select2="<< obj.shop_sell_select2;
	os << ", shop_sell_number2="<< obj.shop_sell_number2;
	os << ", shop_sold2="<< obj.shop_sold2;
	os << ", shop_greeting3="<< obj.shop_greeting3;
	os << ", shop_regreeting3="<< obj.shop_regreeting3;
	os << ", shop_buy3="<< obj.shop_buy3;
	os << ", shop_sell3="<< obj.shop_sell3;
	os << ", shop_leave3="<< obj.shop_leave3;
	os << ", shop_buy_select3="<< obj.shop_buy_select3;
	os << ", shop_buy_number3="<< obj.shop_buy_number3;
	os << ", shop_purchased3="<< obj.shop_purchased3;
	os << ", shop_sell_select3="<< obj.shop_sell_select3;
	os << ", shop_sell_number3="<< obj.shop_sell_number3;
	os << ", shop_sold3="<< obj.shop_sold3;
	os << ", inn_a_greeting_1="<< obj.inn_a_greeting_1;
	os << ", inn_a_greeting_2="<< obj.inn_a_greeting_2;
	os << ", inn_a_greeting_3="<< obj.inn_a_greeting_3;
	os << ", inn_a_accept="<< obj.inn_a_accept;
	os << ", inn_a_cancel="<< obj.inn_a_cancel;
	os << ", inn_b_greeting_1="<< obj.inn_b_greeting_1;
	os << ", inn_b_greeting_2="<< obj.inn_b_greeting_2;
	os << ", inn_b_greeting_3="<< obj.inn_b_greeting_3;
	os << ", inn_b_accept="<< obj.inn_b_accept;
	os << ", inn_b_cancel="<< obj.inn_b_cancel;
	os << ", possessed_items="<< obj.possessed_items;
	os << ", equipped_items="<< obj.equipped_items;
	os << ", gold="<< obj.gold;
	os << ", battle_fight="<< obj.battle_fight;
	os << ", battle_auto="<< obj.battle_auto;
	os << ", battle_escape="<< obj.battle_escape;
	os << ", command_attack="<< obj.command_attack;
	os << ", command_defend="<< obj.command_defend;
	os << ", command_item="<< obj.command_item;
	os << ", command_skill="<< obj.command_skill;
	os << ", menu_equipment="<< obj.menu_equipment;
	os << ", menu_save="<< obj.menu_save;
	os << ", menu_quit="<< obj.menu_quit;
	os << ", new_game="<< obj.new_game;
	os << ", load_game="<< obj.load_game;
	os << ", exit_game="<< obj.exit_game;
	os << ", status="<< obj.status;
	os << ", row="<< obj.row;
	os << ", order="<< obj.order;
	os << ", wait_on="<< obj.wait_on;
	os << ", wait_off="<< obj.wait_off;
	os << ", level="<< obj.level;
	os << ", health_points="<< obj.health_points;
	os << ", spirit_points="<< obj.spirit_points;
	os << ", normal_status="<< obj.normal_status;
	os << ", exp_short="<< obj.exp_short;
	os << ", lvl_short="<< obj.lvl_short;
	os << ", hp_short="<< obj.hp_short;
	os << ", sp_short="<< obj.sp_short;
	os << ", sp_cost="<< obj.sp_cost;
	os << ", attack="<< obj.attack;
	os << ", defense="<< obj.defense;
	os << ", spirit="<< obj.spirit;
	os << ", agility="<< obj.agility;
	os << ", weapon="<< obj.weapon;
	os << ", shield="<< obj.shield;
	os << ", armor="<< obj.armor;
	os << ", helmet="<< obj.helmet;
	os << ", accessory="<< obj.accessory;
	os << ", save_game_message="<< obj.save_game_message;
	os << ", load_game_message="<< obj.load_game_message;
	os << ", file="<< obj.file;
	os << ", exit_game_message="<< obj.exit_game_message;
	os << ", yes="<< obj.yes;
	os << ", no="<< obj.no;
	os << ", easyrpg_item_number_separator="<< obj.easyrpg_item_number_separator;
	os << ", easyrpg_skill_cost_separator="<< obj.easyrpg_skill_cost_separator;
	os << ", easyrpg_equipment_arrow="<< obj.easyrpg_equipment_arrow;
	os << ", easyrpg_status_scene_name="<< obj.easyrpg_status_scene_name;
	os << ", easyrpg_status_scene_class="<< obj.easyrpg_status_scene_class;
	os << ", easyrpg_status_scene_title="<< obj.easyrpg_status_scene_title;
	os << ", easyrpg_status_scene_condition="<< obj.easyrpg_status_scene_condition;
	os << ", easyrpg_status_scene_front="<< obj.easyrpg_status_scene_front;
	os << ", easyrpg_status_scene_back="<< obj.easyrpg_status_scene_back;
	os << ", easyrpg_order_scene_confirm="<< obj.easyrpg_order_scene_confirm;
	os << ", easyrpg_order_scene_redo="<< obj.easyrpg_order_scene_redo;
	os << ", easyrpg_battle2k3_double_attack="<< obj.easyrpg_battle2k3_double_attack;
	os << ", easyrpg_battle2k3_defend="<< obj.easyrpg_battle2k3_defend;
	os << ", easyrpg_battle2k3_observe="<< obj.easyrpg_battle2k3_observe;
	os << ", easyrpg_battle2k3_charge="<< obj.easyrpg_battle2k3_charge;
	os << ", easyrpg_battle2k3_selfdestruct="<< obj.easyrpg_battle2k3_selfdestruct;
	os << ", easyrpg_battle2k3_escape="<< obj.easyrpg_battle2k3_escape;
	os << ", easyrpg_battle2k3_special_combat_back="<< obj.easyrpg_battle2k3_special_combat_back;
	os << ", easyrpg_battle2k3_skill="<< obj.easyrpg_battle2k3_skill;
	os << ", easyrpg_battle2k3_item="<< obj.easyrpg_battle2k3_item;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
