/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "lcf/lmt/reader.h"
#include "lcf/lmt/chunks.h"
#include "reader_struct.h"

namespace lcf {

template <>
struct RawStruct<rpg::Rect> {
	static void ReadLcf(rpg::Rect& ref, LcfReader& stream, uint32_t length);
	static void WriteLcf(const rpg::Rect& ref, LcfWriter& stream);
	static int LcfSize(const rpg::Rect& ref, LcfWriter& stream);
	static void WriteXml(const rpg::Rect& ref, XmlWriter& stream);
	static void BeginXml(rpg::Rect& ref, XmlReader& stream);
};

/**
 * Reads MapInfo Rect.
 */
void RawStruct<rpg::Rect>::ReadLcf(rpg::Rect& ref, LcfReader& stream, uint32_t length) {
	assert(length == 16);
	(void)length;
	stream.Read(ref.l);
	stream.Read(ref.t);
	stream.Read(ref.r);
	stream.Read(ref.b);
}

void RawStruct<rpg::Rect>::WriteLcf(const rpg::Rect& ref, LcfWriter& stream) {
	stream.Write(ref.l);
	stream.Write(ref.t);
	stream.Write(ref.r);
	stream.Write(ref.b);
}

int RawStruct<rpg::Rect>::LcfSize(const rpg::Rect& /* ref */, LcfWriter& /* stream */) {
	return 4 * 4;
}

void RawStruct<rpg::Rect>::WriteXml(const rpg::Rect& ref, XmlWriter& stream) {
	stream.BeginElement("Rect");
	stream.WriteNode<int32_t>("l", ref.l);
	stream.WriteNode<int32_t>("t", ref.t);
	stream.WriteNode<int32_t>("r", ref.r);
	stream.WriteNode<int32_t>("b", ref.b);
	stream.EndElement("Rect");
}

class RectXmlHandler : public XmlHandler {
private:
	rpg::Rect& ref;
	uint32_t* field;
public:
	RectXmlHandler(rpg::Rect& ref) : ref(ref), field(NULL) {}
	void StartElement(XmlReader& /* stream */, const char* name, const char** /* atts */) {
		if (strcmp(name, "l") == 0)
			field = &ref.l;
		else if (strcmp(name, "t") == 0)
			field = &ref.t;
		else if (strcmp(name, "r") == 0)
			field = &ref.r;
		else if (strcmp(name, "b") == 0)
			field = &ref.b;
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
			XmlReader::Read(*field, data);
	}
};

void RawStruct<rpg::Rect>::BeginXml(rpg::Rect& ref, XmlReader& stream) {
	stream.SetHandler(new WrapperXmlHandler("Rect", new RectXmlHandler(ref)));
}

} //namespace lcf
