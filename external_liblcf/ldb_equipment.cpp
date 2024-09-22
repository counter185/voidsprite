/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "lcf/ldb/reader.h"
#include "lcf/ldb/chunks.h"
#include "log.h"
#include "reader_struct.h"

namespace lcf {

template <>
struct RawStruct<rpg::Equipment> {
	static void ReadLcf(rpg::Equipment& ref, LcfReader& stream, uint32_t length);
	static void WriteLcf(const rpg::Equipment& ref, LcfWriter& stream);
	static int LcfSize(const rpg::Equipment& ref, LcfWriter& stream);
	static void WriteXml(const rpg::Equipment& ref, XmlWriter& stream);
	static void BeginXml(rpg::Equipment& ref, XmlReader& stream);
};

/**
 * Reads Equipment.
 */
void RawStruct<rpg::Equipment>::ReadLcf(rpg::Equipment& ref, LcfReader& stream, uint32_t length) {
	if (length != 10) {
		Log::Warning("Equipment has incorrect size %" PRIu32 " (expected 10)", length);

		LcfReader::Chunk chunk_info;
		chunk_info.ID = 0x33;
		chunk_info.length = length;

		stream.Skip(chunk_info, "Equipment");

		return;
	}

	stream.Read(ref.weapon_id);
	stream.Read(ref.shield_id);
	stream.Read(ref.armor_id);
	stream.Read(ref.helmet_id);
	stream.Read(ref.accessory_id);
}

void RawStruct<rpg::Equipment>::WriteLcf(const rpg::Equipment& ref, LcfWriter& stream) {
	stream.Write(ref.weapon_id);
	stream.Write(ref.shield_id);
	stream.Write(ref.armor_id);
	stream.Write(ref.helmet_id);
	stream.Write(ref.accessory_id);
}

int RawStruct<rpg::Equipment>::LcfSize(const rpg::Equipment& /* ref */, LcfWriter& /* stream */) {
	return 2 * 5;
}

void RawStruct<rpg::Equipment>::WriteXml(const rpg::Equipment& ref, XmlWriter& stream) {
	stream.BeginElement("Equipment");
	stream.WriteNode<int16_t>("weapon_id", ref.weapon_id);
	stream.WriteNode<int16_t>("shield_id", ref.shield_id);
	stream.WriteNode<int16_t>("armor_id", ref.armor_id);
	stream.WriteNode<int16_t>("helmet_id", ref.helmet_id);
	stream.WriteNode<int16_t>("accessory_id", ref.accessory_id);
	stream.EndElement("Equipment");
}

class EquipmentXmlHandler : public XmlHandler {
private:
	rpg::Equipment& ref;
	int16_t* field;
public:
	EquipmentXmlHandler(rpg::Equipment& ref) : ref(ref), field(NULL) {}
	void StartElement(XmlReader& /* stream */, const char* name, const char** /* atts */) {
		if (strcmp(name, "weapon_id") == 0)
			field = &ref.weapon_id;
		else if (strcmp(name, "shield_id") == 0)
			field = &ref.shield_id;
		else if (strcmp(name, "armor_id") == 0)
			field = &ref.armor_id;
		else if (strcmp(name, "helmet_id") == 0)
			field = &ref.helmet_id;
		else if (strcmp(name, "accessory_id") == 0)
			field = &ref.accessory_id;
		else {
			Log::Error("XML: Unrecognized field '%s'", name);
			field = NULL;
		}
	}
	void EndElement(XmlReader& /* stream */, const char* /* name */) {
		field = NULL;
	}
	void CharacterData(XmlReader& /* stream*/, const std::string& data) {
		if (field != NULL)
			XmlReader::Read(*field, data);
	}
};

void RawStruct<rpg::Equipment>::BeginXml(rpg::Equipment& ref, XmlReader& stream) {
	stream.SetHandler(new WrapperXmlHandler("Equipment", new EquipmentXmlHandler(ref)));
}

} //namespace lcf
