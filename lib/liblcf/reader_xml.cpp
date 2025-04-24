/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <sstream>
#include <cstdarg>
#include "lcf/reader_lcf.h"
#include "lcf/reader_xml.h"
#include "lcf/dbstring.h"
#include "log.h"

// Expat callbacks
#if LCF_SUPPORT_XML
extern "C" {
static void StartElementHandler(void* closure, const XML_Char* name, const XML_Char** atts) {
	((lcf::XmlReader*) closure)->StartElement(name, atts);
}

static void EndElementHandler(void* closure, const XML_Char* name) {
	((lcf::XmlReader*) closure)->EndElement(name);
}

static void CharacterDataHandler(void* closure, const XML_Char* s, int len) {
	((lcf::XmlReader*) closure)->CharacterData(s, len);
}
}
#endif

namespace lcf {

XmlReader::XmlReader(std::istream& filestream) :
	stream(filestream),
	parser(NULL)
{
#if LCF_SUPPORT_XML
	parser = XML_ParserCreate("UTF-8");

	XML_SetUserData(parser, (void*) this);
	XML_SetElementHandler(parser, StartElementHandler, EndElementHandler);
	XML_SetCharacterDataHandler(parser, CharacterDataHandler);

	handlers.push_back(NULL);
#endif
}

XmlReader::~XmlReader() {
#if LCF_SUPPORT_XML
	if (parser != NULL)
		XML_ParserFree(parser);
	parser = NULL;
#endif
}

bool XmlReader::IsOk() const {
	return (stream.good() && parser != NULL);
}

void XmlReader::Parse() {
#if LCF_SUPPORT_XML
	static const int bufsize = 4096;
	while (IsOk() && !stream.eof()) {
		void* buffer = XML_GetBuffer(parser, bufsize);
		int len = stream.read(reinterpret_cast<char*>(buffer),bufsize).gcount();
		int result = XML_ParseBuffer(parser, len, len <= 0);
		if (result == 0)
			Log::Error("XML: %s", XML_ErrorString(XML_GetErrorCode(parser)));
	}
#endif
}

void XmlReader::SetHandler(XmlHandler* handler) {
	handlers.back() = handler;
}

void XmlReader::StartElement(const char* name, const char** atts) {
	XmlHandler* handler = handlers.back();
	handlers.push_back(handler);
	handlers.back()->StartElement(*this, name, atts);
	buffer.clear();
}

void XmlReader::CharacterData(const char* s, int len) {
	buffer.append(s, len);
}

void XmlReader::EndElement(const char* name) {
	XmlHandler* handler = handlers.back();
	handler->CharacterData(*this, buffer);
	handlers.pop_back();
	if (handler != handlers.back())
		delete handler;
	handlers.back()->EndElement(*this, name);
}

// Primitive type readers

template <>
void XmlReader::Read<bool>(bool& val, const std::string& data) {
	std::istringstream s(data);
	std::string str;
	s >> str;
	val = str == "T";
}

template <>
void XmlReader::Read<int32_t>(int32_t& val, const std::string& data) {
	std::istringstream s(data);
	s >> val;
}

template <>
void XmlReader::Read<int8_t>(int8_t& val, const std::string& data) {
	std::istringstream s(data);
	int x;
	s >> x;
	val = x;
}

template <>
void XmlReader::Read<uint8_t>(uint8_t& val, const std::string& data) {
	std::istringstream s(data);
	int x;
	s >> x;
	val = x;
}

template <>
void XmlReader::Read<int16_t>(int16_t& val, const std::string& data) {
	std::istringstream s(data);
	s >> val;
}

template <>
void XmlReader::Read<uint32_t>(uint32_t& val, const std::string& data) {
	std::istringstream s(data);
	s >> val;
}

template <>
void XmlReader::Read<double>(double& val, const std::string& data) {
	std::istringstream s(data);
	s >> val;
}

template <>
void XmlReader::Read<std::string>(std::string& val, const std::string& data) {
	static const std::string prefix("\xee\x80");

	if (data.find(prefix) == std::string::npos) {
		val = data;
		return;
	}

	// XML doesn't allow most C0 control codes, so they're re-mapped
	// to the private-use area at U+E000. The following code restores
	// re-mapped codes to their original value.

	val.clear();

	for (size_t pos = 0; ; ) {
		size_t next = data.find(prefix, pos);
		if (next > pos)
			val.append(data, pos, next - pos);
		if (next == std::string::npos)
			return;
		pos = next + 2;
		val.append(1, data[pos] - '\x80');
		pos++;
	}
}

template <>
void XmlReader::Read<DBString>(DBString& val, const std::string& data) {
	std::string sval;
	Read(sval, data);
	val = DBString(sval);
}

template <>
void XmlReader::Read<DBBitArray>(DBBitArray& val, const std::string& data) {
	// FIXME: Adds copies
	std::vector<bool> tmp;
	ReadVector(tmp, data);
	val = DBBitArray(tmp.begin(), tmp.end());
}

template <class T>
void XmlReader::ReadVector(std::vector<T>& val, const std::string& data) {
	val.clear();
	std::istringstream s(data);
	for (;;) {
		std::string str;
		s >> str;
		if (!s.fail()) {
			T x;
			XmlReader::Read<T>(x, str);
			val.push_back(x);
		}
		if (!s.good())
			break;
	}
}

template <class T>
void XmlReader::ReadVector(DBArray<T>& val, const std::string& data) {
	// FIXME: Adds copies
	std::vector<T> tmp;
	ReadVector(tmp, data);
	val = DBArray<T>(tmp.begin(), tmp.end());
}

template <>
void XmlReader::Read<std::vector<int32_t>>(std::vector<int32_t>& val, const std::string& data) {
	ReadVector<int32_t>(val, data);
}

template <>
void XmlReader::Read<std::vector<bool>>(std::vector<bool>& val, const std::string& data) {
	ReadVector<bool>(val, data);
}

template <>
void XmlReader::Read<std::vector<uint8_t>>(std::vector<uint8_t>& val, const std::string& data) {
	ReadVector<uint8_t>(val, data);
}

template <>
void XmlReader::Read<std::vector<int16_t>>(std::vector<int16_t>& val, const std::string& data) {
	ReadVector<int16_t>(val, data);
}

template <>
void XmlReader::Read<std::vector<uint32_t>>(std::vector<uint32_t>& val, const std::string& data) {
	ReadVector<uint32_t>(val, data);
}

template <>
void XmlReader::Read<std::vector<double>>(std::vector<double>& val, const std::string& data) {
	ReadVector<double>(val, data);
}

template <>
void XmlReader::Read<DBArray<int32_t>>(DBArray<int32_t>& val, const std::string& data) {
	ReadVector<int32_t>(val, data);
}

template <>
void XmlReader::Read<DBArray<bool>>(DBArray<bool>& val, const std::string& data) {
	ReadVector<bool>(val, data);
}

template <>
void XmlReader::Read<DBArray<uint8_t>>(DBArray<uint8_t>& val, const std::string& data) {
	ReadVector<uint8_t>(val, data);
}

template <>
void XmlReader::Read<DBArray<int16_t>>(DBArray<int16_t>& val, const std::string& data) {
	ReadVector<int16_t>(val, data);
}

template <>
void XmlReader::Read<DBArray<uint32_t>>(DBArray<uint32_t>& val, const std::string& data) {
	ReadVector<uint32_t>(val, data);
}

template <>
void XmlReader::Read<DBArray<double>>(DBArray<double>& val, const std::string& data) {
	ReadVector<double>(val, data);
}


} //namespace lcf
