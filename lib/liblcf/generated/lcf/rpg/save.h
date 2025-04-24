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

#ifndef LCF_RPG_SAVE_H
#define LCF_RPG_SAVE_H

// Headers
#include <vector>
#include "lcf/rpg/saveactor.h"
#include "lcf/rpg/savecommonevent.h"
#include "lcf/rpg/saveeasyrpgdata.h"
#include "lcf/rpg/saveeventexecstate.h"
#include "lcf/rpg/saveinventory.h"
#include "lcf/rpg/savemapinfo.h"
#include "lcf/rpg/savepanorama.h"
#include "lcf/rpg/savepartylocation.h"
#include "lcf/rpg/savepicture.h"
#include "lcf/rpg/savescreen.h"
#include "lcf/rpg/savesystem.h"
#include "lcf/rpg/savetarget.h"
#include "lcf/rpg/savetitle.h"
#include "lcf/rpg/savevehiclelocation.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::Save class.
 */
namespace lcf {
namespace rpg {
	class Save {
	public:
		SaveTitle title;
		SaveSystem system;
		SaveScreen screen;
		std::vector<SavePicture> pictures;
		SavePartyLocation party_location;
		SaveVehicleLocation boat_location;
		SaveVehicleLocation ship_location;
		SaveVehicleLocation airship_location;
		std::vector<SaveActor> actors;
		SaveInventory inventory;
		std::vector<SaveTarget> targets;
		SaveMapInfo map_info;
		SavePanorama panorama;
		SaveEventExecState foreground_event_execstate;
		std::vector<SaveCommonEvent> common_events;
		SaveEasyRpgData easyrpg_data;
	};

	inline bool operator==(const Save& l, const Save& r) {
		return l.title == r.title
		&& l.system == r.system
		&& l.screen == r.screen
		&& l.pictures == r.pictures
		&& l.party_location == r.party_location
		&& l.boat_location == r.boat_location
		&& l.ship_location == r.ship_location
		&& l.airship_location == r.airship_location
		&& l.actors == r.actors
		&& l.inventory == r.inventory
		&& l.targets == r.targets
		&& l.map_info == r.map_info
		&& l.panorama == r.panorama
		&& l.foreground_event_execstate == r.foreground_event_execstate
		&& l.common_events == r.common_events
		&& l.easyrpg_data == r.easyrpg_data;
	}

	inline bool operator!=(const Save& l, const Save& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const Save& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(Save& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		const auto ctx1 = Context<Save, ParentCtx>{ "title", -1, &obj, parent_ctx };
		ForEachString(obj.title, f, &ctx1);
		const auto ctx2 = Context<Save, ParentCtx>{ "system", -1, &obj, parent_ctx };
		ForEachString(obj.system, f, &ctx2);
		const auto ctx3 = Context<Save, ParentCtx>{ "screen", -1, &obj, parent_ctx };
		ForEachString(obj.screen, f, &ctx3);
		for (int i = 0; i < static_cast<int>(obj.pictures.size()); ++i) {
			const auto ctx4 = Context<Save, ParentCtx>{ "pictures", i, &obj, parent_ctx };
			ForEachString(obj.pictures[i], f, &ctx4);
		}
		const auto ctx5 = Context<Save, ParentCtx>{ "party_location", -1, &obj, parent_ctx };
		ForEachString(obj.party_location, f, &ctx5);
		const auto ctx6 = Context<Save, ParentCtx>{ "boat_location", -1, &obj, parent_ctx };
		ForEachString(obj.boat_location, f, &ctx6);
		const auto ctx7 = Context<Save, ParentCtx>{ "ship_location", -1, &obj, parent_ctx };
		ForEachString(obj.ship_location, f, &ctx7);
		const auto ctx8 = Context<Save, ParentCtx>{ "airship_location", -1, &obj, parent_ctx };
		ForEachString(obj.airship_location, f, &ctx8);
		for (int i = 0; i < static_cast<int>(obj.actors.size()); ++i) {
			const auto ctx9 = Context<Save, ParentCtx>{ "actors", i, &obj, parent_ctx };
			ForEachString(obj.actors[i], f, &ctx9);
		}
		const auto ctx10 = Context<Save, ParentCtx>{ "inventory", -1, &obj, parent_ctx };
		ForEachString(obj.inventory, f, &ctx10);
		for (int i = 0; i < static_cast<int>(obj.targets.size()); ++i) {
			const auto ctx11 = Context<Save, ParentCtx>{ "targets", i, &obj, parent_ctx };
			ForEachString(obj.targets[i], f, &ctx11);
		}
		const auto ctx12 = Context<Save, ParentCtx>{ "map_info", -1, &obj, parent_ctx };
		ForEachString(obj.map_info, f, &ctx12);
		const auto ctx13 = Context<Save, ParentCtx>{ "panorama", -1, &obj, parent_ctx };
		ForEachString(obj.panorama, f, &ctx13);
		const auto ctx14 = Context<Save, ParentCtx>{ "foreground_event_execstate", -1, &obj, parent_ctx };
		ForEachString(obj.foreground_event_execstate, f, &ctx14);
		for (int i = 0; i < static_cast<int>(obj.common_events.size()); ++i) {
			const auto ctx15 = Context<Save, ParentCtx>{ "common_events", i, &obj, parent_ctx };
			ForEachString(obj.common_events[i], f, &ctx15);
		}
		const auto ctx16 = Context<Save, ParentCtx>{ "easyrpg_data", -1, &obj, parent_ctx };
		ForEachString(obj.easyrpg_data, f, &ctx16);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
