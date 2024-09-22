/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <ostream>

#include "lcf/writer_lcf.h"

namespace lcf {

LcfWriter::LcfWriter(std::ostream& filestream, EngineVersion engine, std::string encoding)
	: stream(filestream)
	, encoder(std::move(encoding))
	, engine(engine)
{
}

void LcfWriter::Write(const void *ptr, size_t size, size_t nmemb) {
	stream.write(reinterpret_cast<const char*>(ptr), size*nmemb);
	assert(stream.good());
}

template <>
void LcfWriter::Write<int8_t>(int8_t val) {
	Write(&val, 1, 1);
}

template <>
void LcfWriter::Write<uint8_t>(uint8_t val) {
	Write(&val, 1, 1);
}

template <>
void LcfWriter::Write<int16_t>(int16_t val) {
	SwapByteOrder(val);
	Write(&val, 2, 1);
}

template <>
void LcfWriter::Write<uint32_t>(uint32_t val) {
	SwapByteOrder(val);
	Write(&val, 4, 1);
}

void LcfWriter::WriteInt(int val) {
	uint32_t value = (uint32_t) val;
	for (int i = 28; i >= 0; i -= 7)
		if (value >= (1U << i) || i == 0)
			Write<uint8_t>((uint8_t)(((value >> i) & 0x7F) | (i > 0 ? 0x80 : 0)));
}

void LcfWriter::WriteUInt64(uint64_t value) {
	for (int i = 56; i >= 0; i -= 7)
		if (value >= (1ULL << i) || i == 0)
			Write<uint8_t>((uint8_t)(((value >> i) & 0x7F) | (i > 0 ? 0x80 : 0)));
}

template <>
void LcfWriter::Write<int32_t>(int32_t val) {
	WriteInt(val);
}

template <>
void LcfWriter::Write<bool>(bool val) {
	uint8_t x = val ? 1 : 0;
	Write(x);
}

template <>
void LcfWriter::Write<double>(double val) {
	SwapByteOrder(val);
	Write(&val, 8, 1);
}

template <>
void LcfWriter::Write<bool>(const std::vector<bool>& buffer) {
	std::vector<bool>::const_iterator it;
	for (it = buffer.begin(); it != buffer.end(); it++) {
		uint8_t val = *it ? 1 : 0;
		Write(val);
	}
}

template <>
void LcfWriter::Write<uint8_t>(const std::vector<uint8_t>& buffer) {
	Write(&buffer.front(), 1, buffer.size());
}

template <>
void LcfWriter::Write<int16_t>(const std::vector<int16_t>& buffer) {
	std::vector<int16_t>::const_iterator it;
	for (it = buffer.begin(); it != buffer.end(); it++)
		Write(*it);
}

template <>
void LcfWriter::Write<int32_t>(const std::vector<int32_t>& buffer) {
	std::vector<int32_t>::const_iterator it;
	for (it = buffer.begin(); it != buffer.end(); it++) {
		int32_t val = *it;
		SwapByteOrder(val);
		// Write<int32_t> writes a compressed integer
		Write(&val, 4, 1);
	}
}

template <>
void LcfWriter::Write<uint32_t>(const std::vector<uint32_t>& buffer) {
	std::vector<uint32_t>::const_iterator it;
	for (it = buffer.begin(); it != buffer.end(); it++)
		Write(*it);
}

void LcfWriter::Write(const std::string& _str) {
	std::string str = Decode(_str);
	if (!str.empty()) {
		Write(&*str.begin(), 1, str.size());
	}
}

void LcfWriter::Write(const DBString& _str) {
	std::string str = Decode(_str);
	if (!str.empty()) {
		Write(&*str.begin(), 1, str.size());
	}
}

void LcfWriter::Write(const DBBitArray& bits) {
	for (auto& b: bits) {
		Write(static_cast<uint8_t>(b));
	}
}

uint32_t LcfWriter::Tell() {
	return (uint32_t)stream.tellp();
}

bool LcfWriter::IsOk() const {
	return stream.good() && encoder.IsOk();
}

std::string LcfWriter::Decode(StringView str) {
	auto copy = std::string(str);
	encoder.Decode(copy);
	return copy;
}

#ifdef WORDS_BIGENDIAN
void LcfWriter::SwapByteOrder(uint16_t& us)
{
	us =	(us >> 8) |
			(us << 8);
}

void LcfWriter::SwapByteOrder(uint32_t& ui)
{
	ui =	(ui >> 24) |
			((ui<<8) & 0x00FF0000) |
			((ui>>8) & 0x0000FF00) |
			(ui << 24);
}

void LcfWriter::SwapByteOrder(double& d)
{
	char *p = reinterpret_cast<char*>(&d);
	std::swap(p[0], p[7]);
	std::swap(p[1], p[6]);
	std::swap(p[2], p[5]);
	std::swap(p[3], p[4]);
}
#else
void LcfWriter::SwapByteOrder(uint16_t& /* us */) {}
void LcfWriter::SwapByteOrder(uint32_t& /* ui */) {}
void LcfWriter::SwapByteOrder(double& /* d */) {}
#endif

void LcfWriter::SwapByteOrder(int16_t& s)
{
	SwapByteOrder((uint16_t&) s);
}

void LcfWriter::SwapByteOrder(int32_t& s)
{
	SwapByteOrder((uint32_t&) s);
}

} //namespace lcf
