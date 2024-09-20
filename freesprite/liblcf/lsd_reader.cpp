/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <cmath>
#include <fstream>
#include <cerrno>
#include <cstring>

#include "lcf/lsd/reader.h"
#include "lcf/lsd/chunks.h"
#include "lcf/rpg/save.h"
#include "lcf/reader_util.h"
#include "log.h"
#include "reader_struct.h"

namespace lcf {

double LSD_Reader::ToTDateTime(std::time_t t) {
	// 25569 is UnixDateDelta: number of days between 1970-01-01 and 1900-01-01
	return(t / 86400.0 + 25569.0);
}

std::time_t LSD_Reader::ToUnixTimestamp(double ms) {
	return(std::time_t(ms * 86400.0 - 25569.0 * 86400.0 + 0.5));
}

double LSD_Reader::GenerateTimestamp(std::time_t t) {
	return ToTDateTime(t);
}

void LSD_Reader::PrepareSave(rpg::Save& save, int32_t version, int32_t codepage) {
	++save.system.save_count;
	save.title.timestamp = LSD_Reader::GenerateTimestamp();
	save.easyrpg_data.version = version;
	save.easyrpg_data.codepage = codepage;
}

std::unique_ptr<rpg::Save> LSD_Reader::Load(StringView filename, StringView encoding) {
	std::ifstream stream(ToString(filename), std::ios::binary);
	if (!stream.is_open()) {
		Log::Error("Failed to open LSD file '%s' for reading", ToString(filename).c_str());
		return nullptr;
	}
	return LSD_Reader::Load(stream, encoding);
}

bool LSD_Reader::Save(StringView filename, const rpg::Save& save, EngineVersion engine, StringView encoding) {
	std::ofstream stream(ToString(filename), std::ios::binary);
	if (!stream.is_open()) {
		Log::Error("Failed to open LSD file '%s' for reading", ToString(filename).c_str());
		return false;
	}
	return LSD_Reader::Save(stream, save, engine, encoding);
}

bool LSD_Reader::SaveXml(StringView filename, const rpg::Save& save, EngineVersion engine) {
	std::ofstream stream(ToString(filename), std::ios::binary);
	if (!stream.is_open()) {
		Log::Error("Failed to open LSD XML file '%s' for writing", ToString(filename).c_str());
		return false;
	}
	return LSD_Reader::SaveXml(stream, save, engine);
}

std::unique_ptr<rpg::Save> LSD_Reader::LoadXml(StringView filename) {
	std::ifstream stream(ToString(filename), std::ios::binary);
	if (!stream.is_open()) {
		Log::Error("Failed to open LSD XML file `%s' for reading", ToString(filename).c_str());
		return nullptr;
	}
	return LSD_Reader::LoadXml(stream);
}

std::unique_ptr<rpg::Save> LSD_Reader::Load(std::istream& filestream, StringView encoding) {
	LcfReader reader(filestream, ToString(encoding));

	if (!reader.IsOk()) {
		LcfReader::SetError("Couldn't parse save file.");
		return {};
	}
	std::string header;
	reader.ReadString(header, reader.ReadInt());
	if (header.length() != 11) {
		LcfReader::SetError("This is not a valid RPG2000 save.");
		return {};
	}
	if (header != "LcfSaveData") {
		Log::Warning("Header %s != LcfSaveData and might not be a valid RPG2000 save.", header.c_str());
	}

	auto pos = reader.Tell();

	std::unique_ptr<rpg::Save> save(new rpg::Save());
	Struct<rpg::Save>::ReadLcf(*save, reader);

	if (save->easyrpg_data.codepage > 0) {
		filestream.clear();
		filestream.seekg(pos, std::ios_base::beg);
		LcfReader reader2(filestream, std::to_string(save->easyrpg_data.codepage));
		if (!reader2.IsOk()) {
			LcfReader::SetError("Couldn't parse save file.");
			return {};
		}
		save.reset(new rpg::Save());
		Struct<rpg::Save>::ReadLcf(*save, reader2);
	}

	return save;
}

bool LSD_Reader::Save(std::ostream& filestream, const rpg::Save& save, EngineVersion engine, StringView encoding) {
	std::string enc;

	if (save.easyrpg_data.codepage > 0) {
		enc = std::to_string(save.easyrpg_data.codepage);
	} else {
		enc = ToString(encoding);
	}

	LcfWriter writer(filestream, engine, enc);
	if (!writer.IsOk()) {
		LcfReader::SetError("Couldn't parse save file.\n");
		return false;
	}
	const std::string header("LcfSaveData");
	writer.WriteInt(header.size());
	writer.Write(header);

	Struct<rpg::Save>::WriteLcf(save, writer);
	return true;
}

bool LSD_Reader::SaveXml(std::ostream& filestream, const rpg::Save& save, EngineVersion engine) {
	XmlWriter writer(filestream, engine);
	if (!writer.IsOk()) {
		LcfReader::SetError("Couldn't parse save file.");
		return false;
	}

	writer.BeginElement("LSD");
	Struct<rpg::Save>::WriteXml(save, writer);
	writer.EndElement("LSD");
	return true;
}

std::unique_ptr<rpg::Save> LSD_Reader::LoadXml(std::istream& filestream) {
	XmlReader reader(filestream);
	if (!reader.IsOk()) {
		LcfReader::SetError("Couldn't parse save file.");
		return {};
	}

	rpg::Save* save = new rpg::Save();
	reader.SetHandler(new RootXmlHandler<rpg::Save>(*save, "LSD"));
	reader.Parse();
	return std::unique_ptr<rpg::Save>(save);
}

} //namespace lcf
