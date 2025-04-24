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
#include "reader_struct.h"

namespace lcf {

template <>
struct RawStruct<rpg::Parameters> {
	static void ReadLcf(rpg::Parameters& ref, LcfReader& stream, uint32_t length);
	static void WriteLcf(const rpg::Parameters& ref, LcfWriter& stream);
	static int LcfSize(const rpg::Parameters& ref, LcfWriter& stream);
	static void WriteXml(const rpg::Parameters& ref, XmlWriter& stream);
	static void BeginXml(rpg::Parameters& ref, XmlReader& stream);
};

/**
 * Reads Parameters.
 */
void RawStruct<rpg::Parameters>::ReadLcf(rpg::Parameters& ref, LcfReader& stream, uint32_t length) {
	int n = length / 6;
	stream.Read(ref.maxhp, n);
	stream.Read(ref.maxsp, n);
	stream.Read(ref.attack, n);
	stream.Read(ref.defense, n);
	stream.Read(ref.spirit, n);
	stream.Read(ref.agility, n);
}

void RawStruct<rpg::Parameters>::WriteLcf(const rpg::Parameters& ref, LcfWriter& stream) {
	stream.Write(ref.maxhp);
	stream.Write(ref.maxsp);
	stream.Write(ref.attack);
	stream.Write(ref.defense);
	stream.Write(ref.spirit);
	stream.Write(ref.agility);
}

int RawStruct<rpg::Parameters>::LcfSize(const rpg::Parameters& ref, LcfWriter& /* stream */) {
	return ref.maxhp.size() * 2 * 6;
}

void RawStruct<rpg::Parameters>::WriteXml(const rpg::Parameters& ref, XmlWriter& stream) {
	stream.BeginElement("Parameters");
	stream.WriteNode<std::vector<int16_t> >("maxhp", ref.maxhp);
	stream.WriteNode<std::vector<int16_t> >("maxsp", ref.maxsp);
	stream.WriteNode<std::vector<int16_t> >("attack", ref.attack);
	stream.WriteNode<std::vector<int16_t> >("defense", ref.defense);
	stream.WriteNode<std::vector<int16_t> >("spirit", ref.spirit);
	stream.WriteNode<std::vector<int16_t> >("agility", ref.agility);
	stream.EndElement("Parameters");
}

class ParametersXmlHandler : public XmlHandler {
private:
	rpg::Parameters& ref;
	std::vector<int16_t>* field;
public:
	ParametersXmlHandler(rpg::Parameters& ref) : ref(ref), field(NULL) {}
	void StartElement(XmlReader& /* stream */, const char* name, const char** /* atts */) {
		if (strcmp(name, "maxhp") == 0)
			field = &ref.maxhp;
		else if (strcmp(name, "maxsp") == 0)
			field = &ref.maxsp;
		else if (strcmp(name, "attack") == 0)
			field = &ref.attack;
		else if (strcmp(name, "defense") == 0)
			field = &ref.defense;
		else if (strcmp(name, "spirit") == 0)
			field = &ref.spirit;
		else if (strcmp(name, "agility") == 0)
			field = &ref.agility;
		else {
			Log::Error("XML: Unrecognized field '%s'", name);
			field = NULL;
		}
	}
	void EndElement(XmlReader& /* stream */, const char* /* name */) {
		field = NULL;
	}
	void CharacterData(XmlReader& /* stream */, const std::string& data) {
		if (field != NULL)
			XmlReader::Read<std::vector<int16_t> >(*field, data);
	}
};

void RawStruct<rpg::Parameters>::BeginXml(rpg::Parameters& ref, XmlReader& stream) {
	stream.SetHandler(new WrapperXmlHandler("Parameters", new ParametersXmlHandler(ref)));
}

} //namespace lcf
