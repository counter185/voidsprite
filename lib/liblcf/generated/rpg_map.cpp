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
#include "lcf/rpg/map.h"

namespace lcf {
namespace rpg {

std::ostream& operator<<(std::ostream& os, const Map& obj) {
	os << "Map{";
	os << "chipset_id="<< obj.chipset_id;
	os << ", width="<< obj.width;
	os << ", height="<< obj.height;
	os << ", scroll_type="<< obj.scroll_type;
	os << ", parallax_flag="<< obj.parallax_flag;
	os << ", parallax_name="<< obj.parallax_name;
	os << ", parallax_loop_x="<< obj.parallax_loop_x;
	os << ", parallax_loop_y="<< obj.parallax_loop_y;
	os << ", parallax_auto_loop_x="<< obj.parallax_auto_loop_x;
	os << ", parallax_sx="<< obj.parallax_sx;
	os << ", parallax_auto_loop_y="<< obj.parallax_auto_loop_y;
	os << ", parallax_sy="<< obj.parallax_sy;
	os << ", generator_flag="<< obj.generator_flag;
	os << ", generator_mode="<< obj.generator_mode;
	os << ", top_level="<< obj.top_level;
	os << ", generator_tiles="<< obj.generator_tiles;
	os << ", generator_width="<< obj.generator_width;
	os << ", generator_height="<< obj.generator_height;
	os << ", generator_surround="<< obj.generator_surround;
	os << ", generator_upper_wall="<< obj.generator_upper_wall;
	os << ", generator_floor_b="<< obj.generator_floor_b;
	os << ", generator_floor_c="<< obj.generator_floor_c;
	os << ", generator_extra_b="<< obj.generator_extra_b;
	os << ", generator_extra_c="<< obj.generator_extra_c;
	os << ", generator_x=";
	for (size_t i = 0; i < obj.generator_x.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.generator_x[i];
	}
	os << "]";
	os << ", generator_y=";
	for (size_t i = 0; i < obj.generator_y.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.generator_y[i];
	}
	os << "]";
	os << ", generator_tile_ids=";
	for (size_t i = 0; i < obj.generator_tile_ids.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.generator_tile_ids[i];
	}
	os << "]";
	os << ", lower_layer=";
	for (size_t i = 0; i < obj.lower_layer.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.lower_layer[i];
	}
	os << "]";
	os << ", upper_layer=";
	for (size_t i = 0; i < obj.upper_layer.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.upper_layer[i];
	}
	os << "]";
	os << ", events=";
	for (size_t i = 0; i < obj.events.size(); ++i) {
		os << (i == 0 ? "[" : ", ") << obj.events[i];
	}
	os << "]";
	os << ", save_count_2k3e="<< obj.save_count_2k3e;
	os << ", save_count="<< obj.save_count;
	os << "}";
	return os;
}

} // namespace rpg
} // namespace lcf
