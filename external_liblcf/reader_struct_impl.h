/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <cstring>
#include <iostream>
#include <iomanip>
#include <type_traits>
#include "lcf/ldb/reader.h"
#include "lcf/lmt/reader.h"
#include "lcf/lmu/reader.h"
#include "lcf/lsd/reader.h"
#include "reader_struct.h"
#include "lcf/rpg/save.h"
#include "log.h"

namespace lcf {

// Read/Write Struct

template <class S>
void Struct<S>::MakeFieldMap() {
	if (!field_map.empty())
		return;
	for (int i = 0; fields[i] != NULL; i++)
		field_map[fields[i]->id] = fields[i];
}

template <class S>
void Struct<S>::MakeTagMap() {
	if (!tag_map.empty())
		return;
	for (int i = 0; fields[i] != NULL; i++)
		tag_map[fields[i]->name] = fields[i];
}

template <typename T>
struct StructDefault {
	static T make(bool) {
		return T();
	}
};

template <>
struct StructDefault<rpg::Actor> {
	static rpg::Actor make(bool is2k3) {
		auto actor = rpg::Actor();
		actor.Setup(is2k3);
		return actor;
	}
};



template <class S>
void Struct<S>::ReadLcf(S& obj, LcfReader& stream) {
	MakeFieldMap();

	LcfReader::Chunk chunk_info;

	while (!stream.Eof()) {
		chunk_info.ID = stream.ReadInt();
		if (chunk_info.ID == 0)
			break;

		chunk_info.length = stream.ReadInt();

		auto it = field_map.find(chunk_info.ID);
		if (it != field_map.end()) {
#ifdef LCF_DEBUG_TRACE
			fprintf(stderr, "0x%02x (size: %" PRIu32 ", pos: 0x%" PRIx32 "): %s\n", chunk_info.ID, chunk_info.length, stream.Tell(), it->second->name);
#endif
			const uint32_t off = stream.Tell();
			it->second->ReadLcf(obj, stream, chunk_info.length);
			const uint32_t bytes_read = stream.Tell() - off;
			if (bytes_read != chunk_info.length) {
				Log::Warning("%s: Corrupted Chunk 0x%02" PRIx32 " (size: %" PRIu32 ", pos: 0x%" PRIx32 "): %s : Read %" PRIu32 " bytes!",
						Struct<S>::name, chunk_info.ID, chunk_info.length, off, it->second->name, bytes_read);
				stream.Seek(off + chunk_info.length);
			}
		}
		else {
			stream.Skip(chunk_info, Struct<S>::name);
		}
	}
}

template<typename T>
typename std::enable_if<std::is_same<T, rpg::Save>::value ||
		std::is_same<T, rpg::Database>::value>::type
conditional_zero_writer(LcfWriter&) {
	// no-op
}

template<typename T>
typename std::enable_if<!std::is_same<T, rpg::Save>::value &&
		!std::is_same<T, rpg::Database>::value>::type
conditional_zero_writer(LcfWriter& stream) {
	stream.WriteInt(0);
}

template <class S>
void Struct<S>::WriteLcf(const S& obj, LcfWriter& stream) {
	const bool db_is2k3 = stream.Is2k3();

	auto ref = StructDefault<S>::make(db_is2k3);
	int last = -1;
	for (int i = 0; fields[i] != NULL; i++) {
		const Field<S>* field = fields[i];
		if (!db_is2k3 && field->is2k3) {
			continue;
		}
		if (field->id < last)
			std::cerr << "field order mismatch: " << field->id
					  << " after " << last
					  << " in struct " << name
					  << std::endl;
		if (!field->isPresentIfDefault(db_is2k3) && field->IsDefault(obj, ref, db_is2k3)) {
			continue;
		}
		stream.WriteInt(field->id);
		auto len = field->LcfSize(obj, stream);
		stream.WriteInt(len);
		if (len > 0) {
			field->WriteLcf(obj, stream);
		}
	}
	// Writing a 0-byte after rpg::Database or rpg::Save breaks the parser in RPG_RT
	conditional_zero_writer<S>(stream);
}

template <class S>
int Struct<S>::LcfSize(const S& obj, LcfWriter& stream) {
	const bool db_is2k3 = stream.Is2k3();
	int result = 0;
	auto ref = StructDefault<S>::make(db_is2k3);
	for (int i = 0; fields[i] != NULL; i++) {
		const Field<S>* field = fields[i];
		if (!db_is2k3 && field->is2k3) {
			continue;
		}
		//printf("%s\n", field->name);
		if (!field->isPresentIfDefault(db_is2k3) && field->IsDefault(obj, ref, db_is2k3)) {
			continue;
		}
		result += LcfReader::IntSize(field->id);
		int size = field->LcfSize(obj, stream);
		result += LcfReader::IntSize(size);
		result += size;
	}
	result += LcfReader::IntSize(0);
	return result;
}

template <class S>
void Struct<S>::WriteXml(const S& obj, XmlWriter& stream) {
	IDReader::WriteXmlTag(obj, name, stream);
	for (int i = 0; fields[i] != NULL; i++) {
		const Field<S>* field = fields[i];
		field->WriteXml(obj, stream);
	}
	stream.EndElement(name);
}

template <class S>
class StructXmlHandler : public XmlHandler {
public:
	StructXmlHandler(S& ref) : ref(ref), field(NULL) {
		Struct<S>::MakeTagMap();
	}

	void StartElement(XmlReader& stream, const char* name, const char** /* atts */) {
		field = Struct<S>::tag_map[name];
		field->BeginXml(ref, stream);
	}

	void EndElement(XmlReader& /* stream */, const char* /* name */) {
		field = NULL;
	}

	void CharacterData(XmlReader& /* stream */, const std::string& data) {
		if (field != NULL)
			field->ParseXml(ref, data);
	}
private:
	S& ref;
	const Field<S>* field;
};

template <class S>
class StructFieldXmlHandler : public XmlHandler {
public:
	StructFieldXmlHandler(S& ref) : ref(ref) {}

	void StartElement(XmlReader& stream, const char* name, const char** atts) {
		if (strcmp(name, Struct<S>::name) != 0)
			Log::Error("XML: Expecting %s but got %s", Struct<S>::name, name);
		Struct<S>::IDReader::ReadIDXml(ref, atts);
		stream.SetHandler(new StructXmlHandler<S>(ref));
	}
private:
	S& ref;
};

template <class S>
void Struct<S>::BeginXml(S& obj, XmlReader& stream) {
	stream.SetHandler(new StructFieldXmlHandler<S>(obj));
}

// Read/Write std::vector<Struct>

template <class S>
void Struct<S>::ReadLcf(std::vector<S>& vec, LcfReader& stream) {
	int count = stream.ReadInt();
	vec.resize(count);
	for (int i = 0; i < count; i++) {
		IDReader::ReadID(vec[i], stream);
		TypeReader<S>::ReadLcf(vec[i], stream, 0);
	}
}

template <class S>
void Struct<S>::WriteLcf(const std::vector<S>& vec, LcfWriter& stream) {
	int count = vec.size();
	stream.WriteInt(count);
	for (int i = 0; i < count; i++) {
		IDReader::WriteID(vec[i], stream);
		TypeReader<S>::WriteLcf(vec[i], stream);
	}
}

template <class S>
int Struct<S>::LcfSize(const std::vector<S>& vec, LcfWriter& stream) {
	int result = 0;
	int count = vec.size();
	result += LcfReader::IntSize(count);
	for (int i = 0; i < count; i++) {
		result += IDReader::IDSize(vec[i]);
		result += TypeReader<S>::LcfSize(vec[i], stream);
	}
	return result;
}

template <class S>
void Struct<S>::WriteXml(const std::vector<S>& vec, XmlWriter& stream) {
	int count = vec.size();
	for (int i = 0; i < count; i++)
		TypeReader<S>::WriteXml(vec[i], stream);
}

template <class S>
class StructVectorXmlHandler : public XmlHandler {
public:
	StructVectorXmlHandler(std::vector<S>& ref) : ref(ref) {}

	void StartElement(XmlReader& stream, const char* name, const char** atts) {
		if (strcmp(name, Struct<S>::name) != 0)
			Log::Error("XML: Expecting %s but got %s", Struct<S>::name, name);
		ref.resize(ref.size() + 1);
		S& obj = ref.back();
		Struct<S>::IDReader::ReadIDXml(obj, atts);
		stream.SetHandler(new StructXmlHandler<S>(obj));
	}
private:
	std::vector<S>& ref;
};

template <class S>
void Struct<S>::BeginXml(std::vector<S>& obj, XmlReader& stream) {
	stream.SetHandler(new StructVectorXmlHandler<S>(obj));
}

} //namespace lcf

#include "fwd_struct_impl.h"
