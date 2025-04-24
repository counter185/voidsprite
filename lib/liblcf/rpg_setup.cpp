/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "lcf/config.h"
#include "lcf/rpg/actor.h"
#include "lcf/rpg/event.h"
#include "lcf/rpg/map.h"
#include "lcf/rpg/mapinfo.h"
#include "lcf/rpg/system.h"
#include "lcf/rpg/save.h"
#include "lcf/rpg/chipset.h"
#include "lcf/rpg/parameters.h"

namespace lcf {

void rpg::Actor::Setup(bool is2k3) {
	int max_final_level = 0;
	if (is2k3) {
		max_final_level = 99;
		if (final_level == -1) {
			final_level = max_final_level;
		}
		exp_base = exp_base == -1 ? 300 : exp_base;
		exp_inflation = exp_inflation == -1 ? 300 : exp_inflation;
	}
	else {
		max_final_level = 50;
		if (final_level == -1) {
			final_level = max_final_level;
		}
		exp_base = exp_base == -1 ? 30 : exp_base;
		exp_inflation = exp_inflation == -1 ? 30 : exp_inflation;
	}
	parameters.Setup(max_final_level);
}

void rpg::Parameters::Setup(int final_level) {
	size_t level = 0;
	if (final_level > 0) level = final_level;
	if (maxhp.size() < level) maxhp.resize(level, 1);
	if (maxsp.size() < level) maxsp.resize(level, 0);
	if (attack.size() < level) attack.resize(level, 1);
	if (defense.size() < level) defense.resize(level, 1);
	if (spirit.size() < level) spirit.resize(level, 1);
	if (agility.size() < level) agility.resize(level, 1);
}

} // namespace lcf
