/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <fstream>
#include <cerrno>
#include <cstring>

#include "lcf/ldb/reader.h"
#include "lcf/ldb/chunks.h"
#include "lcf/reader_util.h"
#include "log.h"
#include "reader_struct.h"

namespace lcf {

void LDB_Reader::PrepareSave(rpg::Database& db) {
	++db.system.save_count;
}

std::unique_ptr<lcf::rpg::Database> LDB_Reader::Load(StringView filename, StringView encoding) {
	std::ifstream stream(ToString(filename), std::ios::binary);
	if (!stream.is_open()) {
		Log::Error("Failed to open LDB file '%s' for reading", ToString(filename).c_str());
		return nullptr;
	}
	return LDB_Reader::Load(stream, encoding);
}

bool LDB_Reader::Save(StringView filename, const lcf::rpg::Database& db, StringView encoding, SaveOpt opt) {
	std::ofstream stream(ToString(filename), std::ios::binary);
	if (!stream.is_open()) {
		Log::Error("Failed to open LDB file '%s' for writing", ToString(filename).c_str());
		return false;
	}
	return LDB_Reader::Save(stream, db, encoding, opt);
}

bool LDB_Reader::SaveXml(StringView filename, const lcf::rpg::Database& db) {
	std::ofstream stream(ToString(filename), std::ios::binary);
	if (!stream.is_open()) {
		Log::Error("Failed to open LDB XML file '%s' for writing", ToString(filename).c_str());
		return false;
	}
	return LDB_Reader::SaveXml(stream, db);
}

std::unique_ptr<lcf::rpg::Database> LDB_Reader::LoadXml(StringView filename) {
	std::ifstream stream(ToString(filename), std::ios::binary);
	if (!stream.is_open()) {
		Log::Error("Failed to open LDB XML file '%s' for reading", ToString(filename).c_str());
		return nullptr;
	}
	return LDB_Reader::LoadXml(stream);
}

std::unique_ptr<lcf::rpg::Database> LDB_Reader::Load(std::istream& filestream, StringView encoding) {
	LcfReader reader(filestream, ToString(encoding));
	if (!reader.IsOk()) {
		LcfReader::SetError("Couldn't parse database file.");
		return nullptr;
	}
	std::string header;
	reader.ReadString(header, reader.ReadInt());
	if (header.length() != 11) {
		LcfReader::SetError("This is not a valid RPG2000 database.");
		return nullptr;
	}
	if (header != "LcfDataBase") {
		Log::Warning("Header %s != LcfDataBase and might not be a valid RPG2000 database.", header.c_str());
	}
	auto db = std::make_unique<lcf::rpg::Database>();
	db->ldb_header = header;
	TypeReader<rpg::Database>::ReadLcf(*db, reader, 0);

	const auto engine = GetEngineVersion(*db);
	// Delayed initialization of some actor fields because they are engine
	// dependent
	for (auto& actor: db->actors) {
		actor.Setup(engine == EngineVersion::e2k3);
	}

	return db;
}

bool LDB_Reader::Save(std::ostream& filestream, const lcf::rpg::Database& db, StringView encoding, SaveOpt opt) {
	const auto engine = GetEngineVersion(db);
	LcfWriter writer(filestream, engine, ToString(encoding));
	if (!writer.IsOk()) {
		LcfReader::SetError("Couldn't parse database file.");
		return false;
	}
	std::string header;
	if ( db.ldb_header.empty() || !bool(opt & SaveOpt::ePreserveHeader)) {
		header = "LcfDataBase";
	} else {
		header= db.ldb_header;
	}
	writer.WriteInt(header.size());
	writer.Write(header);
	TypeReader<rpg::Database>::WriteLcf(db, writer);
	return true;
}

bool LDB_Reader::SaveXml(std::ostream& filestream, const lcf::rpg::Database& db) {
	const auto engine = GetEngineVersion(db);
	XmlWriter writer(filestream, engine);
	if (!writer.IsOk()) {
		LcfReader::SetError("Couldn't parse database file.\n");
		return false;
	}
	writer.BeginElement("LDB");
	TypeReader<rpg::Database>::WriteXml(db, writer);
	writer.EndElement("LDB");
	return true;
}

std::unique_ptr<lcf::rpg::Database> LDB_Reader::LoadXml(std::istream& filestream) {
	XmlReader reader(filestream);
	if (!reader.IsOk()) {
		LcfReader::SetError("Couldn't parse database file.\n");
		return nullptr;;
	}
	auto db = std::make_unique<lcf::rpg::Database>();
	reader.SetHandler(new RootXmlHandler<rpg::Database>(*db, "LDB"));
	reader.Parse();

	const auto engine = GetEngineVersion(*db);
	// Delayed initialization of some actor fields because they are engine
	// dependent
	for (auto& actor: db->actors) {
		actor.Setup(engine == EngineVersion::e2k3);
	}

	return db;
}

} // namespace lcf
