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

#ifndef LCF_LDB_CHUNKS_H
#define LCF_LDB_CHUNKS_H

namespace lcf {

/**
 * LDB Reader namespace.
 */
namespace LDB_Reader {
	struct ChunkLearning {
		enum Index {
			/** Integer */
			level = 0x01,
			/** Integer */
			skill_id = 0x02
		};
	};
	struct ChunkActor {
		enum Index {
			/** String */
			name = 0x01,
			/** String */
			title = 0x02,
			/** String */
			character_name = 0x03,
			/** Integer */
			character_index = 0x04,
			/** Flag */
			transparent = 0x05,
			/** Integer */
			initial_level = 0x07,
			/** Integer */
			final_level = 0x08,
			/** Flag */
			critical_hit = 0x09,
			/** Integer */
			critical_hit_chance = 0x0A,
			/** String */
			face_name = 0x0F,
			/** Integer */
			face_index = 0x10,
			/** Flag */
			two_weapon = 0x15,
			/** Flag */
			lock_equipment = 0x16,
			/** Flag */
			auto_battle = 0x17,
			/** Flag */
			super_guard = 0x18,
			/** Array x 6 - Short */
			parameters = 0x1F,
			/** Integer */
			exp_base = 0x29,
			/** Integer */
			exp_inflation = 0x2A,
			/** Integer */
			exp_correction = 0x2B,
			/** Integer x 5 */
			initial_equipment = 0x33,
			/** Integer */
			unarmed_animation = 0x38,
			/** Integer - RPG2003 */
			class_id = 0x39,
			/** Integer - RPG2003 */
			battle_x = 0x3B,
			/** Integer - RPG2003 */
			battle_y = 0x3C,
			/** Integer - RPG2003 */
			battler_animation = 0x3E,
			/** Array - rpg::Learning */
			skills = 0x3F,
			/** Flag */
			rename_skill = 0x42,
			/** String */
			skill_name = 0x43,
			/** Integer */
			state_ranks_size = 0x47,
			/** Array - Short */
			state_ranks = 0x48,
			/** Integer */
			attribute_ranks_size = 0x49,
			/** Array - Short */
			attribute_ranks = 0x4A,
			/** Array - rpg::BattleCommand - RPG2003 */
			battle_commands = 0x50,
			/** Default autobattle AI behavior of the actor */
			easyrpg_actorai = 0xC9,
			/** If the actor is protected against critical hits */
			easyrpg_prevent_critical = 0xCA,
			/** If the actor has an increased evasion rate */
			easyrpg_raise_evasion = 0xCB,
			/** If the actor is immune to attribute downshifts */
			easyrpg_immune_to_attribute_downshifts = 0xCC,
			/** If the actors unarmed physical attacks ignore evasion */
			easyrpg_ignore_evasion = 0xCD,
			/** Hit rate of the actor on unarmed attacks */
			easyrpg_unarmed_hit = 0xCE,
			/** States inflicted by unarmed normal attacks of the actor */
			easyrpg_unarmed_state_set_size = 0xCF,
			/** States inflicted by unarmed normal attacks of the actor */
			easyrpg_unarmed_state_set = 0xD0,
			/** State infliction probability by unarmed normal attacks of the actor */
			easyrpg_unarmed_state_chance = 0xD1,
			/** Attributes used by unarmed normal attacks of the actor */
			easyrpg_unarmed_attribute_set_size = 0xD2,
			/** Attributes used by unarmed normal attacks of the actor */
			easyrpg_unarmed_attribute_set = 0xD3,
			/** If the actors unarmed physical attacks hits twice */
			easyrpg_dual_attack = 0xD4,
			/** If the actors unarmed physical targets the entire enemy party */
			easyrpg_attack_all = 0xD5
		};
	};
	struct ChunkSound {
		enum Index {
			/** String */
			name = 0x01,
			/** Integer */
			volume = 0x03,
			/** Integer */
			tempo = 0x04,
			/** Integer */
			balance = 0x05
		};
	};
	struct ChunkAnimationTiming {
		enum Index {
			/** Integer */
			frame = 0x01,
			/** rpg::Sound */
			se = 0x02,
			/** Integer */
			flash_scope = 0x03,
			/** Integer */
			flash_red = 0x04,
			/** Integer */
			flash_green = 0x05,
			/** Integer */
			flash_blue = 0x06,
			/** Integer */
			flash_power = 0x07,
			/** Integer - This field RPG2003 only but commonly found in some 2k LDB's. We disable the is2k3 field on purpose because if its defaulted in 2k it won't be written anyway. */
			screen_shake = 0x08
		};
	};
	struct ChunkAnimationCellData {
		enum Index {
			/** Bool */
			valid = 0x01,
			/** Integer */
			cell_id = 0x02,
			/** Integer */
			x = 0x03,
			/** Integer */
			y = 0x04,
			/** Integer */
			zoom = 0x05,
			/** Integer */
			tone_red = 0x06,
			/** Integer */
			tone_green = 0x07,
			/** Integer */
			tone_blue = 0x08,
			/** Integer */
			tone_gray = 0x09,
			/** Integer */
			transparency = 0x0A
		};
	};
	struct ChunkAnimationFrame {
		enum Index {
			/** Array - rpg::AnimationCellData */
			cells = 0x01
		};
	};
	struct ChunkAnimation {
		enum Index {
			/** String */
			name = 0x01,
			/** String */
			animation_name = 0x02,
			/** Battle2 animation when true */
			large = 0x03,
			/** Array - rpg::AnimationTiming */
			timings = 0x06,
			/** Integer */
			scope = 0x09,
			/** Integer */
			position = 0x0A,
			/** Array - rpg::AnimationFrames */
			frames = 0x0C
		};
	};
	struct ChunkAttribute {
		enum Index {
			/** String */
			name = 0x01,
			/** Integer */
			type = 0x02,
			/** Integer */
			a_rate = 0x0B,
			/** Integer */
			b_rate = 0x0C,
			/** Integer */
			c_rate = 0x0D,
			/** Integer */
			d_rate = 0x0E,
			/** Integer */
			e_rate = 0x0F
		};
	};
	struct ChunkBattleCommand {
		enum Index {
			/** String */
			name = 0x01,
			/** Integer */
			type = 0x02
		};
	};
	struct ChunkBattleCommands {
		enum Index {
			/** Integer */
			placement = 0x02,
			/** Set by the RM2k3 Editor when you enable death handler; but has no effect in RPG_RT. */
			death_handler_unused = 0x04,
			/** Integer */
			row = 0x06,
			/** Integer */
			battle_type = 0x07,
			/** Unused hidden checkbox Display normal parameters in RPG2003's battle settings tab */
			unused_display_normal_parameters = 0x09,
			/** Array - rpg::BattleCommand */
			commands = 0x0A,
			/** True if a 2k3 random encounter death handler is active */
			death_handler = 0x0F,
			/** Integer */
			death_event = 0x10,
			/** Integer */
			window_size = 0x14,
			/** Integer */
			transparency = 0x18,
			/** Integer */
			death_teleport = 0x19,
			/** Integer */
			death_teleport_id = 0x1A,
			/** Integer */
			death_teleport_x = 0x1B,
			/** Integer */
			death_teleport_y = 0x1C,
			/** Integer */
			death_teleport_face = 0x1D,
			/** Default ATB mode of RPG 2003 battle system */
			easyrpg_default_atb_mode = 0xC8,
			/** If the row command should be enabled in RPG Maker 2003 battles */
			easyrpg_enable_battle_row_command = 0xC9,
			/** If alternative and gauge style battles should behave like traditional style battles */
			easyrpg_sequential_order = 0xCA,
			/** If the row feature should be disabled in RPG Maker 2003 games */
			easyrpg_disable_row_feature = 0xCB,
			/** If actor facing direction shouldn't ever change during battle. Applies only to Normal and Initiative battle types (0=Disabled, 1=Always flipped, 2=Never flipped) */
			easyrpg_fixed_actor_facing_direction = 0xCC,
			/** If enemy facing direction shouldn't ever change during battle. Applies only to Normal and Initiative battle types (0=Disabled, 1=Always flipped, 2=Never flipped) */
			easyrpg_fixed_enemy_facing_direction = 0xCD
		};
	};
	struct ChunkBattlerAnimation {
		enum Index {
			/** String */
			name = 0x01,
			/** Integer */
			speed = 0x02,
			/** Array - rpg::BattlerAnimationPose */
			poses = 0x0A,
			/** Array - rpg::BattlerAnimationWeapon */
			weapons = 0x0B
		};
	};
	struct ChunkBattlerAnimationItemSkill {
		enum Index {
			/**  */
			unknown02 = 0x02,
			/**  */
			type = 0x03,
			/**  */
			weapon_animation_id = 0x04,
			/**  */
			movement = 0x05,
			/**  */
			after_image = 0x06,
			/**  */
			attacks = 0x07,
			/**  */
			ranged = 0x08,
			/**  */
			ranged_animation_id = 0x09,
			/**  */
			ranged_speed = 0x0C,
			/**  */
			battle_animation_id = 0x0D,
			/**  */
			pose = 0x0E
		};
	};
	struct ChunkBattlerAnimationPose {
		enum Index {
			/** String */
			name = 0x01,
			/** String */
			battler_name = 0x02,
			/** Integer */
			battler_index = 0x03,
			/** Integer */
			animation_type = 0x04,
			/** Integer */
			battle_animation_id = 0x05
		};
	};
	struct ChunkBattlerAnimationWeapon {
		enum Index {
			/** String */
			name = 0x01,
			/** String */
			weapon_name = 0x02,
			/** Integer */
			weapon_index = 0x03
		};
	};
	struct ChunkChipset {
		enum Index {
			/** String */
			name = 0x01,
			/** String */
			chipset_name = 0x02,
			/** Array - Short x 162 */
			terrain_data = 0x03,
			/** Array - Bitflag x 162 */
			passable_data_lower = 0x04,
			/** Array - Bitflag x 144 */
			passable_data_upper = 0x05,
			/** Integer */
			animation_type = 0x0B,
			/** Integer */
			animation_speed = 0x0C
		};
	};
	struct ChunkClass {
		enum Index {
			/** String */
			name = 0x01,
			/** Flag */
			two_weapon = 0x15,
			/** Flag */
			lock_equipment = 0x16,
			/** Flag */
			auto_battle = 0x17,
			/** Flag */
			super_guard = 0x18,
			/** Array x 6 - Short */
			parameters = 0x1F,
			/** Integer */
			exp_base = 0x29,
			/** Integer */
			exp_inflation = 0x2A,
			/** Integer */
			exp_correction = 0x2B,
			/** Integer */
			battler_animation = 0x3E,
			/** Array - rpg::Learning */
			skills = 0x3F,
			/** Integer */
			state_ranks_size = 0x47,
			/** Array - Short */
			state_ranks = 0x48,
			/** Integer */
			attribute_ranks_size = 0x49,
			/** Array - Short */
			attribute_ranks = 0x4A,
			/** Array - Uint32 */
			battle_commands = 0x50
		};
	};
	struct ChunkCommonEvent {
		enum Index {
			/** String */
			name = 0x01,
			/** Integer */
			trigger = 0x0B,
			/** Flag */
			switch_flag = 0x0C,
			/** Integer */
			switch_id = 0x0D,
			/** Integer */
			event_commands_size = 0x15,
			/** Array - rpg::EventCommand */
			event_commands = 0x16
		};
	};
	struct ChunkSkill {
		enum Index {
			/** String */
			name = 0x01,
			/** String */
			description = 0x02,
			/** String - RPG2000 */
			using_message1 = 0x03,
			/** String - RPG2000 */
			using_message2 = 0x04,
			/** Integer - RPG2000 */
			failure_message = 0x07,
			/** Integer */
			type = 0x08,
			/** Integer - RPG2003 */
			sp_type = 0x09,
			/** Integer - RPG2003 */
			sp_percent = 0x0A,
			/** Integer */
			sp_cost = 0x0B,
			/** Integer */
			scope = 0x0C,
			/** Integer */
			switch_id = 0x0D,
			/** Integer */
			animation_id = 0x0E,
			/** rpg::Sound */
			sound_effect = 0x10,
			/** Flag */
			occasion_field = 0x12,
			/** Flag */
			occasion_battle = 0x13,
			/** Flag - RPG2003 */
			reverse_state_effect = 0x14,
			/** Integer */
			physical_rate = 0x15,
			/** Integer */
			magical_rate = 0x16,
			/** Integer */
			variance = 0x17,
			/** Integer */
			power = 0x18,
			/** Integer */
			hit = 0x19,
			/** Flag */
			affect_hp = 0x1F,
			/** Flag */
			affect_sp = 0x20,
			/** Flag */
			affect_attack = 0x21,
			/** Flag */
			affect_defense = 0x22,
			/** Flag */
			affect_spirit = 0x23,
			/** Flag */
			affect_agility = 0x24,
			/** Flag */
			absorb_damage = 0x25,
			/** Flag */
			ignore_defense = 0x26,
			/** Integer */
			state_effects_size = 0x29,
			/** Array - Flag */
			state_effects = 0x2A,
			/** Integer */
			attribute_effects_size = 0x2B,
			/** Array - Flag */
			attribute_effects = 0x2C,
			/** Flag */
			affect_attr_defence = 0x2D,
			/** Integer - RPG2003 */
			battler_animation = 0x31,
			/** ? - RPG2003 */
			battler_animation_data = 0x32,
			/** RPG Maker 2003 battle skill start notification */
			easyrpg_battle2k3_message = 0xC9,
			/** If the skill bypasses reflect states. Useful for physical skills and skills which are meant to remove positive states from enemies */
			easyrpg_ignore_reflect = 0xCA,
			/** Separate accuracy value for state inflictions */
			easyrpg_state_hit = 0xCB,
			/** Separate accuracy value for attribute shifts */
			easyrpg_attribute_hit = 0xCC,
			/** If the skill should bypass skill restrictions by states */
			easyrpg_ignore_restrict_skill = 0xCD,
			/** If the skill should bypass magic restrictions by states */
			easyrpg_ignore_restrict_magic = 0xCE,
			/** If the skill can absorb attack/defense/spirit/agility */
			easyrpg_enable_stat_absorbing = 0xCF,
			/** If the skill should be affected by avoid attack states */
			easyrpg_affected_by_evade_all_physical_attacks = 0xD0,
			/** Critical hit rate percentage for the skill. If negative then the battler critical hit rate is used */
			easyrpg_critical_hit_chance = 0xD1,
			/** If the skill is affected by row modifiers */
			easyrpg_affected_by_row_modifiers = 0xD2,
			/** If the HP usage of the skill is a fixed number or in percent of the maximum HP */
			easyrpg_hp_type = 0xD3,
			/** How much percent of the maximum HP does the usage of the skill cost */
			easyrpg_hp_percent = 0xD4,
			/** How much HP does the usage of the skill cost */
			easyrpg_hp_cost = 0xD5
		};
	};
	struct ChunkItem {
		enum Index {
			/** String */
			name = 0x01,
			/** String */
			description = 0x02,
			/** Integer */
			type = 0x03,
			/** Integer */
			price = 0x05,
			/** Integer */
			uses = 0x06,
			/** Integer */
			atk_points1 = 0x0B,
			/** Integer */
			def_points1 = 0x0C,
			/** Integer */
			spi_points1 = 0x0D,
			/** Integer */
			agi_points1 = 0x0E,
			/** Flag */
			two_handed = 0x0F,
			/** Integer */
			sp_cost = 0x10,
			/** Integer */
			hit = 0x11,
			/** Integer */
			critical_hit = 0x12,
			/** Integer */
			animation_id = 0x14,
			/** Flag */
			preemptive = 0x15,
			/** Flag */
			dual_attack = 0x16,
			/** Flag */
			attack_all = 0x17,
			/** Flag */
			ignore_evasion = 0x18,
			/** Flag */
			prevent_critical = 0x19,
			/** Flag */
			raise_evasion = 0x1A,
			/** Flag */
			half_sp_cost = 0x1B,
			/** Flag */
			no_terrain_damage = 0x1C,
			/** Flag - RPG2003 */
			cursed = 0x1D,
			/** Flag */
			entire_party = 0x1F,
			/** Integer */
			recover_hp_rate = 0x20,
			/** Integer */
			recover_hp = 0x21,
			/** Integer */
			recover_sp_rate = 0x22,
			/** Integer */
			recover_sp = 0x23,
			/** Flag */
			occasion_field1 = 0x25,
			/** Flag */
			ko_only = 0x26,
			/** Integer */
			max_hp_points = 0x29,
			/** Integer */
			max_sp_points = 0x2A,
			/** Integer */
			atk_points2 = 0x2B,
			/** Integer */
			def_points2 = 0x2C,
			/** Integer */
			spi_points2 = 0x2D,
			/** Integer */
			agi_points2 = 0x2E,
			/** Integer */
			using_message = 0x33,
			/** Integer */
			skill_id = 0x35,
			/** Integer */
			switch_id = 0x37,
			/** Flag */
			occasion_field2 = 0x39,
			/** Flag */
			occasion_battle = 0x3A,
			/** Integer */
			actor_set_size = 0x3D,
			/** Array - Flag */
			actor_set = 0x3E,
			/** Integer */
			state_set_size = 0x3F,
			/** Array - Flag */
			state_set = 0x40,
			/** Integer */
			attribute_set_size = 0x41,
			/** Array - Flag */
			attribute_set = 0x42,
			/** Integer */
			state_chance = 0x43,
			/** Flag */
			reverse_state_effect = 0x44,
			/** Integer - RPG2003 */
			weapon_animation = 0x45,
			/** Array - RPG2003 */
			animation_data = 0x46,
			/** Flag - RPG2003 */
			use_skill = 0x47,
			/** Integer - RPG2003 */
			class_set_size = 0x48,
			/** Array - Flag - RPG2003 */
			class_set = 0x49,
			/** Integer */
			ranged_trajectory = 0x4B,
			/** Integer */
			ranged_target = 0x4C,
			/** Item usage message in battle */
			easyrpg_using_message = 0xC9,
			/** How many the player can carry of this item */
			easyrpg_max_count = 0xCA
		};
	};
	struct ChunkEnemyAction {
		enum Index {
			/** Integer */
			kind = 0x01,
			/** Integer */
			basic = 0x02,
			/** Integer */
			skill_id = 0x03,
			/** Integer */
			enemy_id = 0x04,
			/** Integer */
			condition_type = 0x05,
			/** Integer */
			condition_param1 = 0x06,
			/** Integer */
			condition_param2 = 0x07,
			/** Integer */
			switch_id = 0x08,
			/** Flag */
			switch_on = 0x09,
			/** Integer */
			switch_on_id = 0x0A,
			/** Flag */
			switch_off = 0x0B,
			/** Integer */
			switch_off_id = 0x0C,
			/** Integer */
			rating = 0x0D
		};
	};
	struct ChunkEnemy {
		enum Index {
			/** String */
			name = 0x01,
			/** String */
			battler_name = 0x02,
			/** Integer */
			battler_hue = 0x03,
			/** Integer */
			max_hp = 0x04,
			/** Integer */
			max_sp = 0x05,
			/** Integer */
			attack = 0x06,
			/** Integer */
			defense = 0x07,
			/** Integer */
			spirit = 0x08,
			/** Integer */
			agility = 0x09,
			/** Flag */
			transparent = 0x0A,
			/** Integer */
			exp = 0x0B,
			/** Integer */
			gold = 0x0C,
			/** Integer */
			drop_id = 0x0D,
			/** Integer */
			drop_prob = 0x0E,
			/** Flag */
			critical_hit = 0x15,
			/** Integer */
			critical_hit_chance = 0x16,
			/** Flag */
			miss = 0x1A,
			/** Flag */
			levitate = 0x1C,
			/** Integer */
			state_ranks_size = 0x1F,
			/** Array - Short */
			state_ranks = 0x20,
			/** Integer */
			attribute_ranks_size = 0x21,
			/** Array - Short */
			attribute_ranks = 0x22,
			/** Array - rpg::EnemyAction */
			actions = 0x2A,
			/** Animation for normal enemy attacks (Maniac Patch only) */
			maniac_unarmed_animation = 0x0F,
			/** Default AI behavior of the enemy */
			easyrpg_enemyai = 0xC9,
			/** If the enemy is protected against critical hits */
			easyrpg_prevent_critical = 0xCA,
			/** If the enemy has an increased evasion rate */
			easyrpg_raise_evasion = 0xCB,
			/** If the enemy is immune to attribute downshifts */
			easyrpg_immune_to_attribute_downshifts = 0xCC,
			/** If the enemies unarmed physical attacks ignore evasion */
			easyrpg_ignore_evasion = 0xCD,
			/** Hit rate of the enemy on normal attacks */
			easyrpg_hit = 0xCE,
			/** States inflicted by normal attacks of the enemy */
			easyrpg_state_set_size = 0xCF,
			/** States inflicted by normal attacks of the enemy */
			easyrpg_state_set = 0xD0,
			/** State infliction probability by normal attacks of the enemy */
			easyrpg_state_chance = 0xD1,
			/** Attributes used by normal attacks of the enemy */
			easyrpg_attribute_set_size = 0xD2,
			/** Attributes used by normal attacks of the enemy */
			easyrpg_attribute_set = 0xD3,
			/** If the enemies defend action quarters instead of halves HP damage */
			easyrpg_super_guard = 0xD4,
			/** If the enemies unarmed physical targets the entire player party */
			easyrpg_attack_all = 0xD5
		};
	};
	struct ChunkTroopMember {
		enum Index {
			/** Integer */
			enemy_id = 0x01,
			/** Integer */
			x = 0x02,
			/** Integer */
			y = 0x03,
			/** Flag */
			invisible = 0x04
		};
	};
	struct ChunkTroopPageCondition {
		enum Index {
			/** Bitflag - x 2 if RPG2003 */
			flags = 0x01,
			/** Integer */
			switch_a_id = 0x02,
			/** Integer */
			switch_b_id = 0x03,
			/** Integer */
			variable_id = 0x04,
			/** Integer */
			variable_value = 0x05,
			/** Integer */
			turn_a = 0x06,
			/** Integer */
			turn_b = 0x07,
			/** Integer */
			fatigue_min = 0x08,
			/** Integer */
			fatigue_max = 0x09,
			/** Integer */
			enemy_id = 0x0A,
			/** Integer */
			enemy_hp_min = 0x0B,
			/** Integer */
			enemy_hp_max = 0x0C,
			/** Integer */
			actor_id = 0x0D,
			/** Integer */
			actor_hp_min = 0x0E,
			/** Integer */
			actor_hp_max = 0x0F,
			/** Integer - RPG2003 */
			turn_enemy_id = 0x10,
			/** Integer - RPG2003 */
			turn_enemy_a = 0x11,
			/** Integer - RPG2003 */
			turn_enemy_b = 0x12,
			/** Integer - RPG2003 */
			turn_actor_id = 0x13,
			/** Integer - RPG2003 */
			turn_actor_a = 0x14,
			/** Integer - RPG2003 */
			turn_actor_b = 0x15,
			/** Integer - RPG2003 */
			command_actor_id = 0x16,
			/** Integer - RPG2003 */
			command_id = 0x17
		};
	};
	struct ChunkTroopPage {
		enum Index {
			/** rpg::TroopPageCondition */
			condition = 0x02,
			/** Integer */
			event_commands_size = 0x0B,
			/** Array - rpg::EventCommand */
			event_commands = 0x0C
		};
	};
	struct ChunkTroop {
		enum Index {
			/** String */
			name = 0x01,
			/** Array - rpg::TroopMember */
			members = 0x02,
			/** Flag */
			auto_alignment = 0x03,
			/** Integer */
			terrain_set_size = 0x04,
			/** Array - Flag */
			terrain_set = 0x05,
			/** Flag */
			appear_randomly = 0x06,
			/** Array - rpg::TroopPage */
			pages = 0x0B
		};
	};
	struct ChunkTerrain {
		enum Index {
			/** String */
			name = 0x01,
			/** Integer */
			damage = 0x02,
			/** Integer */
			encounter_rate = 0x03,
			/** String */
			background_name = 0x04,
			/** Flag */
			boat_pass = 0x05,
			/** Flag */
			ship_pass = 0x06,
			/** Flag */
			airship_pass = 0x07,
			/** Flag */
			airship_land = 0x09,
			/** Integer */
			bush_depth = 0x0B,
			/** rpg::Sound - RPG2003 */
			footstep = 0x0F,
			/** Flag - RPG2003 */
			on_damage_se = 0x10,
			/** Integer - RPG2003 */
			background_type = 0x11,
			/** String - RPG2003 */
			background_a_name = 0x15,
			/** Flag - RPG2003 */
			background_a_scrollh = 0x16,
			/** Flag - RPG2003 */
			background_a_scrollv = 0x17,
			/** Integer - RPG2003 */
			background_a_scrollh_speed = 0x18,
			/** Integer - RPG2003 */
			background_a_scrollv_speed = 0x19,
			/** Flag - RPG2003 */
			background_b = 0x1E,
			/** String - RPG2003 */
			background_b_name = 0x1F,
			/** Flag - RPG2003 */
			background_b_scrollh = 0x20,
			/** Flag - RPG2003 */
			background_b_scrollv = 0x21,
			/** Integer - RPG2003 */
			background_b_scrollh_speed = 0x22,
			/** Integer - RPG2003 */
			background_b_scrollv_speed = 0x23,
			/** Bitflag - RPG2003 */
			special_flags = 0x28,
			/** Integer - RPG2003 */
			special_back_party = 0x29,
			/** Integer - RPG2003 */
			special_back_enemies = 0x2A,
			/** Integer - RPG2003 */
			special_lateral_party = 0x2B,
			/** Integer - RPG2003 */
			special_lateral_enemies = 0x2C,
			/** Integer - RPG2003 */
			grid_location = 0x2D,
			/** Integer - RPG2003 */
			grid_top_y = 0x2E,
			/** Integer - RPG2003 */
			grid_elongation = 0x2F,
			/** Integer - RPG2003 */
			grid_inclination = 0x30,
			/** If the terrain damage is a percentage */
			easyrpg_damage_in_percent = 0xC8,
			/** If the terrain damage can kill the actors */
			easyrpg_damage_can_kill = 0xC9
		};
	};
	struct ChunkState {
		enum Index {
			/** String */
			name = 0x01,
			/** Integer */
			type = 0x02,
			/** Integer */
			color = 0x03,
			/** Integer */
			priority = 0x04,
			/** Integer */
			restriction = 0x05,
			/** Integer */
			a_rate = 0x0B,
			/** Integer */
			b_rate = 0x0C,
			/** Integer */
			c_rate = 0x0D,
			/** Integer */
			d_rate = 0x0E,
			/** Integer */
			e_rate = 0x0F,
			/** Integer */
			hold_turn = 0x15,
			/** Integer */
			auto_release_prob = 0x16,
			/** Integer */
			release_by_damage = 0x17,
			/** Integer - RPG2003 */
			affect_type = 0x1E,
			/** Flag */
			affect_attack = 0x1F,
			/** Flag */
			affect_defense = 0x20,
			/** Flag */
			affect_spirit = 0x21,
			/** Flag */
			affect_agility = 0x22,
			/** Integer */
			reduce_hit_ratio = 0x23,
			/** Flag - RPG2003 */
			avoid_attacks = 0x24,
			/** Flag - RPG2003 */
			reflect_magic = 0x25,
			/** Flag - RPG2003 */
			cursed = 0x26,
			/** Integer - RPG2003 */
			battler_animation_id = 0x27,
			/** Flag */
			restrict_skill = 0x29,
			/** Integer */
			restrict_skill_level = 0x2A,
			/** Flag */
			restrict_magic = 0x2B,
			/** Integer */
			restrict_magic_level = 0x2C,
			/** Integer */
			hp_change_type = 0x2D,
			/** Integer */
			sp_change_type = 0x2E,
			/** String */
			message_actor = 0x33,
			/** String */
			message_enemy = 0x34,
			/** String */
			message_already = 0x35,
			/** String */
			message_affected = 0x36,
			/** String */
			message_recovery = 0x37,
			/** Integer */
			hp_change_max = 0x3D,
			/** Integer */
			hp_change_val = 0x3E,
			/** Integer */
			hp_change_map_steps = 0x3F,
			/** Integer */
			hp_change_map_val = 0x40,
			/** Integer */
			sp_change_max = 0x41,
			/** Integer */
			sp_change_val = 0x42,
			/** Integer */
			sp_change_map_steps = 0x43,
			/** Integer */
			sp_change_map_val = 0x44,
			/** States cleared on infliction by this state */
			easyrpg_immune_states_size = 0xC8,
			/** States cleared on infliction by this state */
			easyrpg_immune_states = 0xC9
		};
	};
	struct ChunkTerms {
		enum Index {
			/** String */
			encounter = 0x01,
			/** String */
			special_combat = 0x02,
			/** String */
			escape_success = 0x03,
			/** String */
			escape_failure = 0x04,
			/** String */
			victory = 0x05,
			/** String */
			defeat = 0x06,
			/** String */
			exp_received = 0x07,
			/** String */
			gold_recieved_a = 0x08,
			/** String */
			gold_recieved_b = 0x09,
			/** String */
			item_recieved = 0x0A,
			/** String */
			attacking = 0x0B,
			/** String */
			enemy_critical = 0x0C,
			/** String */
			actor_critical = 0x0D,
			/** String */
			defending = 0x0E,
			/** String */
			observing = 0x0F,
			/** String */
			focus = 0x10,
			/** String */
			autodestruction = 0x11,
			/** String */
			enemy_escape = 0x12,
			/** String */
			enemy_transform = 0x13,
			/** String */
			enemy_damaged = 0x14,
			/** String */
			enemy_undamaged = 0x15,
			/** String */
			actor_damaged = 0x16,
			/** String */
			actor_undamaged = 0x17,
			/** String */
			skill_failure_a = 0x18,
			/** String */
			skill_failure_b = 0x19,
			/** String */
			skill_failure_c = 0x1A,
			/** String */
			dodge = 0x1B,
			/** String */
			use_item = 0x1C,
			/** String */
			hp_recovery = 0x1D,
			/** String */
			parameter_increase = 0x1E,
			/** String */
			parameter_decrease = 0x1F,
			/** String */
			enemy_hp_absorbed = 0x20,
			/** String */
			actor_hp_absorbed = 0x21,
			/** String */
			resistance_increase = 0x22,
			/** String */
			resistance_decrease = 0x23,
			/** String */
			level_up = 0x24,
			/** String */
			skill_learned = 0x25,
			/** String */
			battle_start = 0x26,
			/** String */
			miss = 0x27,
			/** String */
			shop_greeting1 = 0x29,
			/** String */
			shop_regreeting1 = 0x2A,
			/** String */
			shop_buy1 = 0x2B,
			/** String */
			shop_sell1 = 0x2C,
			/** String */
			shop_leave1 = 0x2D,
			/** String */
			shop_buy_select1 = 0x2E,
			/** String */
			shop_buy_number1 = 0x2F,
			/** String */
			shop_purchased1 = 0x30,
			/** String */
			shop_sell_select1 = 0x31,
			/** String */
			shop_sell_number1 = 0x32,
			/** String */
			shop_sold1 = 0x33,
			/** String */
			shop_greeting2 = 0x36,
			/** String */
			shop_regreeting2 = 0x37,
			/** String */
			shop_buy2 = 0x38,
			/** String */
			shop_sell2 = 0x39,
			/** String */
			shop_leave2 = 0x3A,
			/** String */
			shop_buy_select2 = 0x3B,
			/** String */
			shop_buy_number2 = 0x3C,
			/** String */
			shop_purchased2 = 0x3D,
			/** String */
			shop_sell_select2 = 0x3E,
			/** String */
			shop_sell_number2 = 0x3F,
			/** String */
			shop_sold2 = 0x40,
			/** String */
			shop_greeting3 = 0x43,
			/** String */
			shop_regreeting3 = 0x44,
			/** String */
			shop_buy3 = 0x45,
			/** String */
			shop_sell3 = 0x46,
			/** String */
			shop_leave3 = 0x47,
			/** String */
			shop_buy_select3 = 0x48,
			/** String */
			shop_buy_number3 = 0x49,
			/** String */
			shop_purchased3 = 0x4A,
			/** String */
			shop_sell_select3 = 0x4B,
			/** String */
			shop_sell_number3 = 0x4C,
			/** String */
			shop_sold3 = 0x4D,
			/** String */
			inn_a_greeting_1 = 0x50,
			/** String */
			inn_a_greeting_2 = 0x51,
			/** String */
			inn_a_greeting_3 = 0x52,
			/** String */
			inn_a_accept = 0x53,
			/** String */
			inn_a_cancel = 0x54,
			/** String */
			inn_b_greeting_1 = 0x55,
			/** String */
			inn_b_greeting_2 = 0x56,
			/** String */
			inn_b_greeting_3 = 0x57,
			/** String */
			inn_b_accept = 0x58,
			/** String */
			inn_b_cancel = 0x59,
			/** String */
			possessed_items = 0x5C,
			/** String */
			equipped_items = 0x5D,
			/** String */
			gold = 0x5F,
			/** String */
			battle_fight = 0x65,
			/** String */
			battle_auto = 0x66,
			/** String */
			battle_escape = 0x67,
			/** String */
			command_attack = 0x68,
			/** String */
			command_defend = 0x69,
			/** String */
			command_item = 0x6A,
			/** String */
			command_skill = 0x6B,
			/** String */
			menu_equipment = 0x6C,
			/** String */
			menu_save = 0x6E,
			/** String */
			menu_quit = 0x70,
			/** String */
			new_game = 0x72,
			/** String */
			load_game = 0x73,
			/** String */
			exit_game = 0x75,
			/** String */
			status = 0x76,
			/** String */
			row = 0x77,
			/** String */
			order = 0x78,
			/** String */
			wait_on = 0x79,
			/** String */
			wait_off = 0x7A,
			/** String */
			level = 0x7B,
			/** String */
			health_points = 0x7C,
			/** String */
			spirit_points = 0x7D,
			/** String */
			normal_status = 0x7E,
			/** String - char x 2? */
			exp_short = 0x7F,
			/** String - char x 2? */
			lvl_short = 0x80,
			/** String - char x 2? */
			hp_short = 0x81,
			/** String - char x 2? */
			sp_short = 0x82,
			/** String */
			sp_cost = 0x83,
			/** String */
			attack = 0x84,
			/** String */
			defense = 0x85,
			/** String */
			spirit = 0x86,
			/** String */
			agility = 0x87,
			/** String */
			weapon = 0x88,
			/** String */
			shield = 0x89,
			/** String */
			armor = 0x8A,
			/** String */
			helmet = 0x8B,
			/** String */
			accessory = 0x8C,
			/** String */
			save_game_message = 0x92,
			/** String */
			load_game_message = 0x93,
			/** String */
			file = 0x94,
			/** String */
			exit_game_message = 0x97,
			/** String */
			yes = 0x98,
			/** String */
			no = 0x99,
			/** Item number separator */
			easyrpg_item_number_separator = 0xC8,
			/** Skill cost separator */
			easyrpg_skill_cost_separator = 0xC9,
			/** Equipment window arrow */
			easyrpg_equipment_arrow = 0xCA,
			/** Status scene Name */
			easyrpg_status_scene_name = 0xCB,
			/** Status scene Class */
			easyrpg_status_scene_class = 0xCC,
			/** Status scene Title */
			easyrpg_status_scene_title = 0xCD,
			/** Status scene Condition */
			easyrpg_status_scene_condition = 0xCE,
			/** Status scene Front */
			easyrpg_status_scene_front = 0xCF,
			/** Status scene Back */
			easyrpg_status_scene_back = 0xD0,
			/** Order scene Confirm */
			easyrpg_order_scene_confirm = 0xD1,
			/** Order scene Redo */
			easyrpg_order_scene_redo = 0xD2,
			/** RPG Maker 2003 battle monster Double attack notification */
			easyrpg_battle2k3_double_attack = 0xD3,
			/** RPG Maker 2003 battle monster Defend notification */
			easyrpg_battle2k3_defend = 0xD4,
			/** RPG Maker 2003 battle monster Observe notification */
			easyrpg_battle2k3_observe = 0xD5,
			/** RPG Maker 2003 battle monster Charge notification */
			easyrpg_battle2k3_charge = 0xD6,
			/** RPG Maker 2003 battle monster Self-Destruct notification */
			easyrpg_battle2k3_selfdestruct = 0xD7,
			/** RPG Maker 2003 battle monster Escape notification */
			easyrpg_battle2k3_escape = 0xD8,
			/** Message for back and pincer attack */
			easyrpg_battle2k3_special_combat_back = 0xD9,
			/** RPG Maker 2003 battle general skill start notification */
			easyrpg_battle2k3_skill = 0xDA,
			/** RPG Maker 2003 battle general item start notification */
			easyrpg_battle2k3_item = 0xDB
		};
	};
	struct ChunkMusic {
		enum Index {
			/** String */
			name = 0x01,
			/** Integer */
			fadein = 0x02,
			/** Integer */
			volume = 0x03,
			/** Integer */
			tempo = 0x04,
			/** Integer */
			balance = 0x05
		};
	};
	struct ChunkTestBattler {
		enum Index {
			/** Integer */
			actor_id = 0x01,
			/** Integer */
			level = 0x02,
			/** Integer */
			weapon_id = 0x0B,
			/** Integer */
			shield_id = 0x0C,
			/** Integer */
			armor_id = 0x0D,
			/** Integer */
			helmet_id = 0x0E,
			/** Integer */
			accessory_id = 0x0F
		};
	};
	struct ChunkSystem {
		enum Index {
			/** Integer - RPG2003 */
			ldb_id = 0x0A,
			/** String */
			boat_name = 0x0B,
			/** String */
			ship_name = 0x0C,
			/** String */
			airship_name = 0x0D,
			/** Integer */
			boat_index = 0x0E,
			/** Integer */
			ship_index = 0x0F,
			/** Integer */
			airship_index = 0x10,
			/** String */
			title_name = 0x11,
			/** String */
			gameover_name = 0x12,
			/** String */
			system_name = 0x13,
			/** String - RPG2003 */
			system2_name = 0x14,
			/** Integer */
			party_size = 0x15,
			/** Array - Short */
			party = 0x16,
			/** Integer - RPG2003 */
			menu_commands_size = 0x1A,
			/** Array - Short - RPG2003 */
			menu_commands = 0x1B,
			/** rpg::Music */
			title_music = 0x1F,
			/** rpg::Music */
			battle_music = 0x20,
			/** rpg::Music */
			battle_end_music = 0x21,
			/** rpg::Music */
			inn_music = 0x22,
			/** rpg::Music */
			boat_music = 0x23,
			/** rpg::Music */
			ship_music = 0x24,
			/** rpg::Music */
			airship_music = 0x25,
			/** rpg::Music */
			gameover_music = 0x26,
			/** rpg::Sound */
			cursor_se = 0x29,
			/** rpg::Sound */
			decision_se = 0x2A,
			/** rpg::Sound */
			cancel_se = 0x2B,
			/** rpg::Sound */
			buzzer_se = 0x2C,
			/** rpg::Sound */
			battle_se = 0x2D,
			/** rpg::Sound */
			escape_se = 0x2E,
			/** rpg::Sound */
			enemy_attack_se = 0x2F,
			/** rpg::Sound */
			enemy_damaged_se = 0x30,
			/** rpg::Sound */
			actor_damaged_se = 0x31,
			/** rpg::Sound */
			dodge_se = 0x32,
			/** rpg::Sound */
			enemy_death_se = 0x33,
			/** rpg::Sound */
			item_se = 0x34,
			/** Integer */
			transition_out = 0x3D,
			/** Integer */
			transition_in = 0x3E,
			/** Integer */
			battle_start_fadeout = 0x3F,
			/** Integer */
			battle_start_fadein = 0x40,
			/** Integer */
			battle_end_fadeout = 0x41,
			/** Integer */
			battle_end_fadein = 0x42,
			/** Integer */
			message_stretch = 0x47,
			/** Integer */
			font_id = 0x48,
			/** Integer */
			selected_condition = 0x51,
			/** Integer */
			selected_hero = 0x52,
			/** String */
			battletest_background = 0x54,
			/** Array - rpg::TestBattler */
			battletest_data = 0x55,
			/** Integer */
			save_count = 0x5B,
			/** Integer */
			battletest_terrain = 0x5E,
			/** Integer */
			battletest_formation = 0x5F,
			/** Integer */
			battletest_condition = 0x60,
			/** Integer RPG2003 - Whether equipment usage is by Actor or by Class. This is a global setting in RM2k3! */
			equipment_setting = 0x61,
			/** Integer RPG2003 (EDITOR ONLY) - Double click on Terrain in Troops changes this setting and 0x54. Affects only the RM2k3 editor. */
			battletest_alt_terrain = 0x62,
			/** Flag - RPG2003 */
			show_frame = 0x63,
			/** String - RPG2003 */
			frame_name = 0x64,
			/** Flag - RPG2003 */
			invert_animations = 0x65,
			/** When false the title is skipped and the game starts directly. In TestPlay mode skips directly to the Load scene. Added in RPG Maker 2003 v1.11 */
			show_title = 0x6F,
			/** Which EXP formula to use: 0=Default 1=2k 2=2k3 */
			easyrpg_alternative_exp = 0xC8,
			/** Available battle options where 0=Battle 1=AutoBattle 2=Escape */
			easyrpg_battle_options = 0xC9,
			/** Absolute maximum HP value actors can have */
			easyrpg_max_actor_hp = 0xCA,
			/** Absolute maximum HP value enemies can have */
			easyrpg_max_enemy_hp = 0xCB,
			/** How much damage can an attack inflict at most */
			easyrpg_max_damage = 0xCC,
			/** Absolute maximum EXP value actors can have */
			easyrpg_max_exp = 0xCD,
			/** Absolute maximum level actors can reach */
			easyrpg_max_level = 0xCE,
			/** How many savegame slots should be shown in the savegame scene */
			easyrpg_max_savefiles = 0xCF,
			/** How many items of a kind the player can carry. Can be overridden per item with the easyrpg_max_count setting of an item */
			easyrpg_max_item_count = 0xD0,
			/** Minimum value a variable can have */
			easyrpg_variable_min_value = 0xD1,
			/** Maximum value a variable can have */
			easyrpg_variable_max_value = 0xD2,
			/** Absolute maximum SP value actors can have */
			easyrpg_max_actor_sp = 0xD3,
			/** Absolute maximum SP value enemies can have */
			easyrpg_max_enemy_sp = 0xD4,
			/** Absolute maximum value a base stat can have */
			easyrpg_max_stat_base_value = 0xD5,
			/** Absolute maximum value a battle stat can have */
			easyrpg_max_stat_battle_value = 0xD6,
			/** If the RPG Maker 2000 battle system should be used in RPG Maker 2003 games */
			easyrpg_use_rpg2k_battle_system = 0xD7,
			/** If RPG Maker 2000 battles in RPG Maker 2003 games should use the RPG2kE strings */
			easyrpg_battle_use_rpg2ke_strings = 0xD8,
			/** If the RPG Maker 2000 battle commands should be used in RPG Maker 2003 games */
			easyrpg_use_rpg2k_battle_commands = 0xD9,
			/** System default actor AI */
			easyrpg_default_actorai = 0xDA,
			/** System default enemy AI */
			easyrpg_default_enemyai = 0xDB
		};
	};
	struct ChunkSwitch {
		enum Index {
			/** String */
			name = 0x01
		};
	};
	struct ChunkVariable {
		enum Index {
			/** String */
			name = 0x01
		};
	};
	struct ChunkDatabase {
		enum Index {
			/** rpg::Actor */
			actors = 0x0B,
			/** rpg::Skill */
			skills = 0x0C,
			/** rpg::Item */
			items = 0x0D,
			/** rpg::Enemy */
			enemies = 0x0E,
			/** rpg::Troop */
			troops = 0x0F,
			/** rpg::Terrain */
			terrains = 0x10,
			/** rpg::Attribute */
			attributes = 0x11,
			/** rpg::State */
			states = 0x12,
			/** rpg::Animation */
			animations = 0x13,
			/** rpg::Chipset */
			chipsets = 0x14,
			/** rpg::Terms */
			terms = 0x15,
			/** rpg::System */
			system = 0x16,
			/** rpg::Switches */
			switches = 0x17,
			/** rpg::Variables */
			variables = 0x18,
			/** rpg::CommonEvent */
			commonevents = 0x19,
			/** Indicates version of database. When 1 the database was converted to RPG Maker 2000 v1.61 */
			version = 0x1A,
			/** Duplicated? - Not used - RPG2003 */
			commoneventD2 = 0x1B,
			/** Duplicated? - Not used - RPG2003 */
			commoneventD3 = 0x1C,
			/** rpg::BattleCommand - RPG2003 */
			battlecommands = 0x1D,
			/** rpg::Class - RPG2003 */
			classes = 0x1E,
			/** Duplicated? - Not used - RPG2003 */
			classD1 = 0x1F,
			/** rpg::BattlerAnimation - RPG2003 */
			battleranimations = 0x20
		};
	};
}

} //namespace lcf

#endif
