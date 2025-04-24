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

#ifndef LCF_LMT_CHUNKS_H
#define LCF_LMT_CHUNKS_H

namespace lcf {

/**
 * LMT Reader namespace.
 */
namespace LMT_Reader {
	struct ChunkEncounter {
		enum Index {
			/** Integer */
			troop_id = 0x01
		};
	};
	struct ChunkMapInfo {
		enum Index {
			/** String. Note: Map ID 0 used to be game title but it should be ignored (TreeCtrl dummy editor dumped data); always use RPG_RT.ini GameTitle instead */
			name = 0x01,
			/** Integer. Used to inherit parent map properties */
			parent_map = 0x02,
			/** Integer. Dummy editor dumped data. Branch indentation level in TreeCtrl */
			indentation = 0x03,
			/** Integer */
			type = 0x04,
			/** Integer. Editor only */
			scrollbar_x = 0x05,
			/** Integer. Editor only */
			scrollbar_y = 0x06,
			/** Flag. Editor only */
			expanded_node = 0x07,
			/** Integer. 0=inherit; 1=from event; 2=specified in 0x0C */
			music_type = 0x0B,
			/** Array - rpg::Music */
			music = 0x0C,
			/** Integer. 0=inherit; 1=from terrain ldb data; 2=specified in 0x16 */
			background_type = 0x15,
			/** String */
			background_name = 0x16,
			/** Flag. 0=inherit; 1=allow; 2=disallow */
			teleport = 0x1F,
			/** Flag. 0=inherit; 1=allow; 2=disallow */
			escape = 0x20,
			/** Flag. 0=inherit; 1=allow; 2=disallow */
			save = 0x21,
			/** Array - rpg::Encounter */
			encounters = 0x29,
			/** 0=Encounters Disabled; 1=Encounter Rate for the map */
			encounter_steps = 0x2C,
			/** Uint32 x 4 (Left; Top; Right; Bottom). Normal map (non-area) is 0; 0; 0; 0 */
			area_rect = 0x33
		};
	};
	struct ChunkStart {
		enum Index {
			/** Integer */
			party_map_id = 0x01,
			/** Integer */
			party_x = 0x02,
			/** Integer */
			party_y = 0x03,
			/** Integer */
			boat_map_id = 0x0B,
			/** Integer */
			boat_x = 0x0C,
			/** Integer */
			boat_y = 0x0D,
			/** Integer */
			ship_map_id = 0x15,
			/** Integer */
			ship_x = 0x16,
			/** Integer */
			ship_y = 0x17,
			/** Integer */
			airship_map_id = 0x1F,
			/** Integer */
			airship_x = 0x20,
			/** Integer */
			airship_y = 0x21
		};
	};
}

} //namespace lcf

#endif
