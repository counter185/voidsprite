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

// Read TroopPageCondition.

template <>
char const* const Struct<rpg::TroopPageCondition>::name = "TroopPageCondition";
static TypedField<rpg::TroopPageCondition, rpg::TroopPageCondition::Flags> static_flags(
	&rpg::TroopPageCondition::flags,
	LDB_Reader::ChunkTroopPageCondition::flags,
	"flags",
	1,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_switch_a_id(
	&rpg::TroopPageCondition::switch_a_id,
	LDB_Reader::ChunkTroopPageCondition::switch_a_id,
	"switch_a_id",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_switch_b_id(
	&rpg::TroopPageCondition::switch_b_id,
	LDB_Reader::ChunkTroopPageCondition::switch_b_id,
	"switch_b_id",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_variable_id(
	&rpg::TroopPageCondition::variable_id,
	LDB_Reader::ChunkTroopPageCondition::variable_id,
	"variable_id",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_variable_value(
	&rpg::TroopPageCondition::variable_value,
	LDB_Reader::ChunkTroopPageCondition::variable_value,
	"variable_value",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_turn_a(
	&rpg::TroopPageCondition::turn_a,
	LDB_Reader::ChunkTroopPageCondition::turn_a,
	"turn_a",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_turn_b(
	&rpg::TroopPageCondition::turn_b,
	LDB_Reader::ChunkTroopPageCondition::turn_b,
	"turn_b",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_fatigue_min(
	&rpg::TroopPageCondition::fatigue_min,
	LDB_Reader::ChunkTroopPageCondition::fatigue_min,
	"fatigue_min",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_fatigue_max(
	&rpg::TroopPageCondition::fatigue_max,
	LDB_Reader::ChunkTroopPageCondition::fatigue_max,
	"fatigue_max",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_enemy_id(
	&rpg::TroopPageCondition::enemy_id,
	LDB_Reader::ChunkTroopPageCondition::enemy_id,
	"enemy_id",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_enemy_hp_min(
	&rpg::TroopPageCondition::enemy_hp_min,
	LDB_Reader::ChunkTroopPageCondition::enemy_hp_min,
	"enemy_hp_min",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_enemy_hp_max(
	&rpg::TroopPageCondition::enemy_hp_max,
	LDB_Reader::ChunkTroopPageCondition::enemy_hp_max,
	"enemy_hp_max",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_actor_id(
	&rpg::TroopPageCondition::actor_id,
	LDB_Reader::ChunkTroopPageCondition::actor_id,
	"actor_id",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_actor_hp_min(
	&rpg::TroopPageCondition::actor_hp_min,
	LDB_Reader::ChunkTroopPageCondition::actor_hp_min,
	"actor_hp_min",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_actor_hp_max(
	&rpg::TroopPageCondition::actor_hp_max,
	LDB_Reader::ChunkTroopPageCondition::actor_hp_max,
	"actor_hp_max",
	0,
	0
);
static TypedField<rpg::TroopPageCondition, int32_t> static_turn_enemy_id(
	&rpg::TroopPageCondition::turn_enemy_id,
	LDB_Reader::ChunkTroopPageCondition::turn_enemy_id,
	"turn_enemy_id",
	0,
	1
);
static TypedField<rpg::TroopPageCondition, int32_t> static_turn_enemy_a(
	&rpg::TroopPageCondition::turn_enemy_a,
	LDB_Reader::ChunkTroopPageCondition::turn_enemy_a,
	"turn_enemy_a",
	0,
	1
);
static TypedField<rpg::TroopPageCondition, int32_t> static_turn_enemy_b(
	&rpg::TroopPageCondition::turn_enemy_b,
	LDB_Reader::ChunkTroopPageCondition::turn_enemy_b,
	"turn_enemy_b",
	0,
	1
);
static TypedField<rpg::TroopPageCondition, int32_t> static_turn_actor_id(
	&rpg::TroopPageCondition::turn_actor_id,
	LDB_Reader::ChunkTroopPageCondition::turn_actor_id,
	"turn_actor_id",
	0,
	1
);
static TypedField<rpg::TroopPageCondition, int32_t> static_turn_actor_a(
	&rpg::TroopPageCondition::turn_actor_a,
	LDB_Reader::ChunkTroopPageCondition::turn_actor_a,
	"turn_actor_a",
	0,
	1
);
static TypedField<rpg::TroopPageCondition, int32_t> static_turn_actor_b(
	&rpg::TroopPageCondition::turn_actor_b,
	LDB_Reader::ChunkTroopPageCondition::turn_actor_b,
	"turn_actor_b",
	0,
	1
);
static TypedField<rpg::TroopPageCondition, int32_t> static_command_actor_id(
	&rpg::TroopPageCondition::command_actor_id,
	LDB_Reader::ChunkTroopPageCondition::command_actor_id,
	"command_actor_id",
	0,
	1
);
static TypedField<rpg::TroopPageCondition, int32_t> static_command_id(
	&rpg::TroopPageCondition::command_id,
	LDB_Reader::ChunkTroopPageCondition::command_id,
	"command_id",
	0,
	1
);


template <>
Field<rpg::TroopPageCondition> const* Struct<rpg::TroopPageCondition>::fields[] = {
	&static_flags,
	&static_switch_a_id,
	&static_switch_b_id,
	&static_variable_id,
	&static_variable_value,
	&static_turn_a,
	&static_turn_b,
	&static_fatigue_min,
	&static_fatigue_max,
	&static_enemy_id,
	&static_enemy_hp_min,
	&static_enemy_hp_max,
	&static_actor_id,
	&static_actor_hp_min,
	&static_actor_hp_max,
	&static_turn_enemy_id,
	&static_turn_enemy_a,
	&static_turn_enemy_b,
	&static_turn_actor_id,
	&static_turn_actor_a,
	&static_turn_actor_b,
	&static_command_actor_id,
	&static_command_id,
	NULL
};

template class Struct<rpg::TroopPageCondition>;

} //namespace lcf
