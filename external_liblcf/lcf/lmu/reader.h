/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_LMU_READER_H
#define LCF_LMU_READER_H

#include <string>
#include <memory>
#include "lcf/rpg/map.h"
#include "lcf/saveopt.h"

namespace lcf {

/**
 * LMU Reader namespace.
 */
namespace LMU_Reader {

	/**
	 * Increment the map save count.
	 */
	void PrepareSave(rpg::Map& map);

	/**
	 * Loads map.
	 */
	std::unique_ptr<rpg::Map> Load(StringView filename, StringView encoding = "");

	/**
	 * Saves map.
	 */
	bool Save(StringView filename, const rpg::Map& map, EngineVersion engine, StringView encoding = "", SaveOpt opt = SaveOpt::eNone);

	/**
	 * Saves map as XML.
	 */
	bool SaveXml(StringView filename, const rpg::Map& map, EngineVersion engine);

	/**
	 * Loads map as XML.
	 */
	std::unique_ptr<rpg::Map> LoadXml(StringView filename);

	/**
	 * Loads map.
	 */
	std::unique_ptr<rpg::Map> Load(std::istream& filestream, StringView encoding = "");

	/**
	 * Saves map.
	 */
	bool Save(std::ostream& filestream, const rpg::Map& map, EngineVersion engine, StringView encoding = "", SaveOpt opt = SaveOpt::eNone);

	/**
	 * Saves map as XML.
	 */
	bool SaveXml(std::ostream& filestream, const rpg::Map& map, EngineVersion engine);

	/**
	 * Loads map as XML.
	 */
	std::unique_ptr<rpg::Map> LoadXml(std::istream& filestream);
}

} //namespace lcf

#endif
