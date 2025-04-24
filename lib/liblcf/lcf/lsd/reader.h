/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_LSD_READER_H
#define LCF_LSD_READER_H

#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <stdint.h>
#include "lcf/rpg/save.h"
#include "lcf/saveopt.h"

namespace lcf {

/**
 * LSD Reader namespace.
 */
namespace LSD_Reader {
	/**
	 * Converts from UNIX timestamp to Delphi's TDateTime format.
	 */
	double ToTDateTime(std::time_t t);

	/**
	 * Converts from Delphi's TDateTime format to UNIX timestamp.
	 */
	std::time_t ToUnixTimestamp(double ms);

	/**
	 * Returns current system time encoded in Delphi's TDateTime format.
	 */
	double GenerateTimestamp(std::time_t t = std::time(nullptr));

	/**
	 * Increment the save save_count and update the timestamp.
	 */
	void PrepareSave(rpg::Save& save, int32_t version = 0, int32_t codepage = 0);

	/**
	 * Loads Savegame.
	 */
	std::unique_ptr<rpg::Save> Load(StringView filename, StringView encoding = "");

	/**
	 * Saves Savegame.
	 */
	bool Save(StringView filename, const rpg::Save& save, EngineVersion engine, StringView encoding = "");

	/*
	 * Saves Savegame as XML.
	 */
	bool SaveXml(StringView filename, const rpg::Save& save, EngineVersion engine);

	/**
	 * Loads Savegame as XML.
	 */
	std::unique_ptr<rpg::Save> LoadXml(StringView filename);

	/**
	 * Loads Savegame.
	 */
	std::unique_ptr<rpg::Save> Load(std::istream& filestream, StringView encoding = "");

	/**
	 * Saves Savegame.
	 */
	bool Save(std::ostream& filestream, const rpg::Save& save, EngineVersion engine, StringView encoding = "");

	/*
	 * Saves Savegame as XML.
	 */
	bool SaveXml(std::ostream& filestream, const rpg::Save& save, EngineVersion engine);

	/**
	 * Loads Savegame as XML.
	 */
	std::unique_ptr<rpg::Save> LoadXml(std::istream& filestream);
}

} //namespace lcf

#endif
