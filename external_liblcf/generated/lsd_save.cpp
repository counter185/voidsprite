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
#include "lcf/lsd/reader.h"
#include "lcf/lsd/chunks.h"
#include "reader_struct_impl.h"

namespace lcf {

// Read Save.

template <>
char const* const Struct<rpg::Save>::name = "Save";
static TypedField<rpg::Save, rpg::SaveTitle> static_title(
	&rpg::Save::title,
	LSD_Reader::ChunkSave::title,
	"title",
	0,
	0
);
static TypedField<rpg::Save, rpg::SaveSystem> static_system(
	&rpg::Save::system,
	LSD_Reader::ChunkSave::system,
	"system",
	1,
	0
);
static TypedField<rpg::Save, rpg::SaveScreen> static_screen(
	&rpg::Save::screen,
	LSD_Reader::ChunkSave::screen,
	"screen",
	1,
	0
);
static TypedField<rpg::Save, std::vector<rpg::SavePicture>> static_pictures(
	&rpg::Save::pictures,
	LSD_Reader::ChunkSave::pictures,
	"pictures",
	1,
	0
);
static TypedField<rpg::Save, rpg::SavePartyLocation> static_party_location(
	&rpg::Save::party_location,
	LSD_Reader::ChunkSave::party_location,
	"party_location",
	1,
	0
);
static TypedField<rpg::Save, rpg::SaveVehicleLocation> static_boat_location(
	&rpg::Save::boat_location,
	LSD_Reader::ChunkSave::boat_location,
	"boat_location",
	1,
	0
);
static TypedField<rpg::Save, rpg::SaveVehicleLocation> static_ship_location(
	&rpg::Save::ship_location,
	LSD_Reader::ChunkSave::ship_location,
	"ship_location",
	1,
	0
);
static TypedField<rpg::Save, rpg::SaveVehicleLocation> static_airship_location(
	&rpg::Save::airship_location,
	LSD_Reader::ChunkSave::airship_location,
	"airship_location",
	1,
	0
);
static TypedField<rpg::Save, std::vector<rpg::SaveActor>> static_actors(
	&rpg::Save::actors,
	LSD_Reader::ChunkSave::actors,
	"actors",
	1,
	0
);
static TypedField<rpg::Save, rpg::SaveInventory> static_inventory(
	&rpg::Save::inventory,
	LSD_Reader::ChunkSave::inventory,
	"inventory",
	1,
	0
);
static TypedField<rpg::Save, std::vector<rpg::SaveTarget>> static_targets(
	&rpg::Save::targets,
	LSD_Reader::ChunkSave::targets,
	"targets",
	1,
	0
);
static TypedField<rpg::Save, rpg::SaveMapInfo> static_map_info(
	&rpg::Save::map_info,
	LSD_Reader::ChunkSave::map_info,
	"map_info",
	1,
	0
);
static TypedField<rpg::Save, rpg::SavePanorama> static_panorama(
	&rpg::Save::panorama,
	LSD_Reader::ChunkSave::panorama,
	"panorama",
	1,
	0
);
static TypedField<rpg::Save, rpg::SaveEventExecState> static_foreground_event_execstate(
	&rpg::Save::foreground_event_execstate,
	LSD_Reader::ChunkSave::foreground_event_execstate,
	"foreground_event_execstate",
	1,
	0
);
static TypedField<rpg::Save, std::vector<rpg::SaveCommonEvent>> static_common_events(
	&rpg::Save::common_events,
	LSD_Reader::ChunkSave::common_events,
	"common_events",
	1,
	0
);
static TypedField<rpg::Save, rpg::SaveEasyRpgData> static_easyrpg_data(
	&rpg::Save::easyrpg_data,
	LSD_Reader::ChunkSave::easyrpg_data,
	"easyrpg_data",
	0,
	0
);


template <>
Field<rpg::Save> const* Struct<rpg::Save>::fields[] = {
	&static_title,
	&static_system,
	&static_screen,
	&static_pictures,
	&static_party_location,
	&static_boat_location,
	&static_ship_location,
	&static_airship_location,
	&static_actors,
	&static_inventory,
	&static_targets,
	&static_map_info,
	&static_panorama,
	&static_foreground_event_execstate,
	&static_common_events,
	&static_easyrpg_data,
	NULL
};

template class Struct<rpg::Save>;

} //namespace lcf
