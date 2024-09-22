/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "lcf/dbstring.h"
#include "log.h"
#include "reader_struct.h"
#include <iostream>

namespace lcf {

/*
DBString has the structure of a Pascal String (Length + Data).
It could be a primitive type but vector<DBString> needs special handling.

Vector<DBString> is Maniac Patch and is sparse:
When size is > 0xFFFFFFFF there is a gap of "0x800000000 - size"
In other cases it is as above: Size Data ... Size Data
*/

template <>
struct RawStruct<DBString> {
	static void ReadLcf(DBString& ref, LcfReader& stream, uint32_t length);
	static void WriteLcf(const DBString& ref, LcfWriter& stream);
	static int LcfSize(const DBString& ref, LcfWriter& stream);
	static void WriteXml(const DBString& ref, XmlWriter& stream);
	static void BeginXml(DBString& ref, XmlReader& stream);
};

template <>
struct RawStruct<std::vector<DBString> > {
	static void ReadLcf(std::vector<DBString>& ref, LcfReader& stream, uint32_t length);
	static void WriteLcf(const std::vector<DBString>& ref, LcfWriter& stream);
	static int LcfSize(const std::vector<DBString>& ref, LcfWriter& stream);
	static void WriteXml(const std::vector<DBString>& ref, XmlWriter& stream);
	static void BeginXml(std::vector<DBString>& ref, XmlReader& stream);
};

void RawStruct<DBString>::ReadLcf(DBString& ref, LcfReader& stream, uint32_t length) {
	stream.ReadString(ref, length);
#ifdef LCF_DEBUG_TRACE
	fprintf(stderr, "  %s\n", ref.c_str());
#endif
}

void RawStruct<DBString>::WriteLcf(const DBString& ref, LcfWriter& stream) {
	stream.Write(ref);
}

int RawStruct<DBString>::LcfSize(const DBString& ref, LcfWriter& stream) {
	return stream.Decode(ref).size();
}

void RawStruct<DBString>::WriteXml(const DBString& ref, XmlWriter& stream) {
	stream.Write(ref);
}

class DbStringXmlHandler : public XmlHandler {
private:
	DBString& ref;
public:
	DbStringXmlHandler(DBString& ref) :
		ref(ref) {}
	void StartElement(XmlReader& /* stream */, const char* /* name */, const char** /* atts */) {
		// no-op
	}
	void EndElement(XmlReader& /* stream */, const char* /* name */) {
		// no-op
	}
	void CharacterData(XmlReader& /* stream */, const std::string& data) {
		ref = DBString(data);
	}
};

void RawStruct<DBString>::BeginXml(DBString& ref, XmlReader& stream) {
	stream.SetHandler(new DbStringXmlHandler(ref));
}

void RawStruct<std::vector<DBString>>::ReadLcf(std::vector<DBString>& ref, LcfReader& stream, uint32_t length) {
	int index = 0;
	DBString string_var;

	uint32_t startpos = stream.Tell();
	uint32_t endpos = startpos + length;
	while (stream.Tell() < endpos) {
		// If size is bigger than 4 bytes, size indicates the gap size
		// Otherwise it indicates the size of the next string
		auto size = stream.ReadUInt64();
		if (size > std::numeric_limits<uint32_t>::max()) {
			index += static_cast<uint32_t>(0x800000000 - size);

			ref.resize(index);
		} else {
			stream.ReadString(string_var, size);
#ifdef LCF_DEBUG_TRACE
			fprintf(stderr, "t[%d]: %s\n", index + 1, string_var.c_str());
#endif
			ref.push_back(string_var);

			++index;
		}
	}

	if (stream.Tell() != endpos) {
		Log::Warning("vector<string> Misaligned at 0x%" PRIx32 "", stream.Tell());
		stream.Seek(endpos);
	}
}

void RawStruct<std::vector<DBString>>::WriteLcf(const std::vector<DBString>& ref, LcfWriter& stream) {
	int gap_size = 0;

	for (size_t i = 0; i < ref.size(); ++i) {
		const auto& e = ref[i];
		if (e.empty()) {
			++gap_size;
			continue;
		}

		if (gap_size > 0) {
			stream.WriteUInt64(0x800000000 - static_cast<uint64_t>(gap_size));
			gap_size = 0;
		}

		auto len = RawStruct<DBString>::LcfSize(e, stream);
		stream.WriteInt(len);
		RawStruct<DBString>::WriteLcf(e, stream);
	}
}

int RawStruct<std::vector<DBString>>::LcfSize(const std::vector<DBString>& ref, LcfWriter& stream) {
	int result = 0;
	int gap_size = 0;

	for (size_t i = 0; i < ref.size(); ++i) {
		const auto& e = ref[i];
		if (e.empty()) {
			++gap_size;
			continue;
		}

		if (gap_size > 0) {
			result += LcfReader::UInt64Size(0x800000000 - static_cast<uint64_t>(gap_size));
			gap_size = 0;
		}

		int size = RawStruct<DBString>::LcfSize(e, stream);
		result += LcfReader::IntSize(size);
		result += size;
	}

	return result;
}

void RawStruct<std::vector<DBString>>::WriteXml(const std::vector<DBString>& ref, XmlWriter& stream) {
	for (size_t i = 0; i < ref.size(); ++i) {
		const auto& e = ref[i];
		if (e.empty()) {
			continue;
		}

		stream.BeginElement("item", i + 1);
		RawStruct<DBString>::WriteXml(e, stream);
		stream.EndElement("item");
	}
}

class DbStringVectorXmlHandler : public XmlHandler {
public:
	DbStringVectorXmlHandler(std::vector<DBString>& ref) : ref(ref) {}

	void StartElement(XmlReader& stream, const char* name, const char** atts) {
		if (strcmp(name, "item") != 0) {
			Log::Error("XML: Expecting %s but got %s", "item", name);
			return;
		}

		int last_id = -1;
		int id = -1;
		for (int i = 0; atts[i] != NULL && atts[i + 1] != NULL; i += 2) {
			if (strcmp(atts[i], "id") == 0) {
				id = atoi(atts[i + 1]);
				break;
			}
		}

		if (id <= last_id || id < -1) {
			Log::Error("XML: Bad Id %d / %d", id, last_id);
			return;
		}

		ref.resize(id);
		DBString& obj = ref.back();

		stream.SetHandler(new DbStringXmlHandler(obj));
	}
private:
	std::vector<DBString>& ref;
};

void RawStruct<std::vector<DBString>>::BeginXml(std::vector<DBString>& obj, XmlReader& stream) {
	stream.SetHandler(new DbStringVectorXmlHandler(obj));
}

} //namspace lcf
