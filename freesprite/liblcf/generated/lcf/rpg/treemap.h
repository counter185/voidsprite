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

#ifndef LCF_RPG_TREEMAP_H
#define LCF_RPG_TREEMAP_H

// Headers
#include <stdint.h>
#include <vector>
#include "lcf/rpg/mapinfo.h"
#include "lcf/rpg/start.h"
#include "lcf/context.h"
#include <ostream>
#include <type_traits>

/**
 * rpg::TreeMap class.
 */
namespace lcf {
namespace rpg {
	class TreeMap {
	public:
		std::string lmt_header;
		enum MapType {
			MapType_root = 0,
			MapType_map = 1,
			MapType_area = 2
		};
		static constexpr auto kMapTypeTags = lcf::makeEnumTags<MapType>(
			"root",
			"map",
			"area"
		);

		std::vector<MapInfo> maps;
		std::vector<int32_t> tree_order;
		int32_t active_node = 0;
		Start start;
	};
	inline std::ostream& operator<<(std::ostream& os, TreeMap::MapType code) {
		os << static_cast<std::underlying_type_t<decltype(code)>>(code);
		return os;
	}

	inline bool operator==(const TreeMap& l, const TreeMap& r) {
		return l.maps == r.maps
		&& l.tree_order == r.tree_order
		&& l.active_node == r.active_node
		&& l.start == r.start;
	}

	inline bool operator!=(const TreeMap& l, const TreeMap& r) {
		return !(l == r);
	}

	std::ostream& operator<<(std::ostream& os, const TreeMap& obj);

	template <typename F, typename ParentCtx = Context<void,void>>
	void ForEachString(TreeMap& obj, const F& f, const ParentCtx* parent_ctx = nullptr) {
		for (int i = 0; i < static_cast<int>(obj.maps.size()); ++i) {
			const auto ctx1 = Context<TreeMap, ParentCtx>{ "maps", i, &obj, parent_ctx };
			ForEachString(obj.maps[i], f, &ctx1);
		}
		const auto ctx4 = Context<TreeMap, ParentCtx>{ "start", -1, &obj, parent_ctx };
		ForEachString(obj.start, f, &ctx4);
		(void)obj;
		(void)f;
		(void)parent_ctx;
	}

} // namespace rpg
} // namespace lcf

#endif
