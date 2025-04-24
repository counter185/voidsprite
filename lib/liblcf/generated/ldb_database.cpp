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

// Read Database.

template <>
char const* const Struct<rpg::Database>::name = "Database";
static TypedField<rpg::Database, std::vector<rpg::Actor>> static_actors(
	&rpg::Database::actors,
	LDB_Reader::ChunkDatabase::actors,
	"actors",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::Skill>> static_skills(
	&rpg::Database::skills,
	LDB_Reader::ChunkDatabase::skills,
	"skills",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::Item>> static_items(
	&rpg::Database::items,
	LDB_Reader::ChunkDatabase::items,
	"items",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::Enemy>> static_enemies(
	&rpg::Database::enemies,
	LDB_Reader::ChunkDatabase::enemies,
	"enemies",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::Troop>> static_troops(
	&rpg::Database::troops,
	LDB_Reader::ChunkDatabase::troops,
	"troops",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::Terrain>> static_terrains(
	&rpg::Database::terrains,
	LDB_Reader::ChunkDatabase::terrains,
	"terrains",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::Attribute>> static_attributes(
	&rpg::Database::attributes,
	LDB_Reader::ChunkDatabase::attributes,
	"attributes",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::State>> static_states(
	&rpg::Database::states,
	LDB_Reader::ChunkDatabase::states,
	"states",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::Animation>> static_animations(
	&rpg::Database::animations,
	LDB_Reader::ChunkDatabase::animations,
	"animations",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::Chipset>> static_chipsets(
	&rpg::Database::chipsets,
	LDB_Reader::ChunkDatabase::chipsets,
	"chipsets",
	1,
	0
);
static TypedField<rpg::Database, rpg::Terms> static_terms(
	&rpg::Database::terms,
	LDB_Reader::ChunkDatabase::terms,
	"terms",
	1,
	0
);
static TypedField<rpg::Database, rpg::System> static_system(
	&rpg::Database::system,
	LDB_Reader::ChunkDatabase::system,
	"system",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::Switch>> static_switches(
	&rpg::Database::switches,
	LDB_Reader::ChunkDatabase::switches,
	"switches",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::Variable>> static_variables(
	&rpg::Database::variables,
	LDB_Reader::ChunkDatabase::variables,
	"variables",
	1,
	0
);
static TypedField<rpg::Database, std::vector<rpg::CommonEvent>> static_commonevents(
	&rpg::Database::commonevents,
	LDB_Reader::ChunkDatabase::commonevents,
	"commonevents",
	1,
	0
);
static DatabaseVersionField<rpg::Database, int32_t> static_version(
	&rpg::Database::version,
	LDB_Reader::ChunkDatabase::version,
	"version",
	0,
	0
);
static EmptyField<rpg::Database> static_commoneventD2(
	LDB_Reader::ChunkDatabase::commoneventD2,
	"commoneventD2",
	1,
	1
);
static EmptyField<rpg::Database> static_commoneventD3(
	LDB_Reader::ChunkDatabase::commoneventD3,
	"commoneventD3",
	1,
	1
);
static TypedField<rpg::Database, rpg::BattleCommands> static_battlecommands(
	&rpg::Database::battlecommands,
	LDB_Reader::ChunkDatabase::battlecommands,
	"battlecommands",
	1,
	1
);
static TypedField<rpg::Database, std::vector<rpg::Class>> static_classes(
	&rpg::Database::classes,
	LDB_Reader::ChunkDatabase::classes,
	"classes",
	1,
	1
);
static EmptyField<rpg::Database> static_classD1(
	LDB_Reader::ChunkDatabase::classD1,
	"classD1",
	1,
	1
);
static TypedField<rpg::Database, std::vector<rpg::BattlerAnimation>> static_battleranimations(
	&rpg::Database::battleranimations,
	LDB_Reader::ChunkDatabase::battleranimations,
	"battleranimations",
	1,
	1
);


template <>
Field<rpg::Database> const* Struct<rpg::Database>::fields[] = {
	&static_actors,
	&static_skills,
	&static_items,
	&static_enemies,
	&static_troops,
	&static_terrains,
	&static_attributes,
	&static_states,
	&static_animations,
	&static_chipsets,
	&static_terms,
	&static_system,
	&static_switches,
	&static_variables,
	&static_commonevents,
	&static_version,
	&static_commoneventD2,
	&static_commoneventD3,
	&static_battlecommands,
	&static_classes,
	&static_classD1,
	&static_battleranimations,
	NULL
};

template class Struct<rpg::Database>;

} //namespace lcf
