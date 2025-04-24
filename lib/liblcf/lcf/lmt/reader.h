/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_LMT_READER_H
#define LCF_LMT_READER_H

#include <string>
#include <memory>
#include "lcf/rpg/treemap.h"
#include "lcf/saveopt.h"

namespace lcf {

/**
 * LMT Reader namespace.
 */
namespace LMT_Reader {

	/**
	 * Loads Map Tree.
	 */
	std::unique_ptr<lcf::rpg::TreeMap> Load(StringView filename, StringView encoding = "");

	/**
	 * Saves Map Tree.
	 */
	bool Save(StringView filename, const lcf::rpg::TreeMap& tmap, EngineVersion engine, StringView encoding = "", SaveOpt opt = SaveOpt::eNone);

	/**
	 * Saves Map Tree as XML.
	 */
	bool SaveXml(StringView filename, const lcf::rpg::TreeMap& tmap, EngineVersion engine);

	/**
	 * Loads Map Tree as XML.
	 */
	std::unique_ptr<lcf::rpg::TreeMap> LoadXml(StringView filename);

	/**
	 * Loads Map Tree.
	 */
	std::unique_ptr<lcf::rpg::TreeMap> Load(std::istream& filestream, StringView encoding = "");

	/**
	 * Saves Map Tree.
	 */
	bool Save(std::ostream& filestream, const lcf::rpg::TreeMap& tmap, EngineVersion engine, StringView encoding = "", SaveOpt opt = SaveOpt::eNone);

	/**
	 * Saves Map Tree as XML.
	 */
	bool SaveXml(std::ostream& filestream, const lcf::rpg::TreeMap& tmap, EngineVersion engine);

	/**
	 * Loads Map Tree as XML.
	 */
	std::unique_ptr<lcf::rpg::TreeMap> LoadXml(std::istream& filestream);
}

} //namespace lcf

#endif
