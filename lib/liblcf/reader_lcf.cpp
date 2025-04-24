/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <cstdarg>
#include <cstdio>
#include <iomanip>
#include <istream>
#include <limits>
#include <sstream>

#include "lcf/reader_lcf.h"
#include "log.h"

namespace lcf {
// Statics

std::string LcfReader::error_str;

LcfReader::LcfReader(std::istream& filestream, std::string encoding)
	: stream(filestream)
	, encoder(std::move(encoding))
{
	offset = filestream.tellg();
}

size_t LcfReader::Read0(void *ptr, size_t size, size_t nmemb) {
	if (size == 0) { //avoid division by 0
		return 0;
	}
	//Read nmemb elements of size and return the number of read elements
	stream.read(reinterpret_cast<char*>(ptr), size*nmemb);
	auto bytes_read = stream.gcount();
	offset += bytes_read;
	size_t result = bytes_read / size;
#ifdef NDEBUG
	if (result != nmemb && !Eof()) {
		perror("Reading error: ");
	}
#endif
	return result;
}

void LcfReader::Read(void *ptr, size_t size, size_t nmemb) {
#ifdef NDEBUG
	Read0(ptr, size, nmemb);
#else
	if (Read0(ptr, size, nmemb) != nmemb) {
		Log::Warning("Read error at %" PRIu32 ". The file is probably corrupted", Tell());
	}
#endif
}

template <>
void LcfReader::Read<bool>(bool& ref) {
	ref = ReadInt() > 0;
}

template <>
void LcfReader::Read<int8_t>(int8_t& ref) {
	Read(&ref, 1, 1);
}

template <>
void LcfReader::Read<uint8_t>(uint8_t& ref) {
	Read(&ref, 1, 1);
}

template <>
void LcfReader::Read<int16_t>(int16_t& ref) {
	Read(&ref, 2, 1);
	SwapByteOrder(ref);
}

template <>
void LcfReader::Read<uint32_t>(uint32_t& ref) {
	Read(&ref, 4, 1);
	SwapByteOrder(ref);
}

int LcfReader::ReadInt() {
	int value = 0;
	unsigned char temp = 0;
	int loops = 0;
	do {
		value <<= 7;
		if (Read0(&temp, 1, 1) == 0) {
			assert(value == 0);
			return 0;
		}
		value |= temp & 0x7F;

		if (loops > 5) {
			Log::Warning("Invalid compressed integer at %" PRIu32 "", Tell());
		}
		++loops;
	} while (temp & 0x80);

	return loops > 5 ? 0 : value;
}

uint64_t LcfReader::ReadUInt64() {
	uint64_t value = 0;
	unsigned char temp = 0;
	int loops = 0;
	do {
		value <<= 7;
		if (Read0(&temp, 1, 1) == 0) {
			assert(value == 0);
			return 0;
		}
		value |= static_cast<uint64_t>(temp & 0x7F);

		if (loops > 9) {
			Log::Warning("Invalid compressed integer at %" PRIu32 "", Tell());
		}
		++loops;
	} while (temp & 0x80);

	return loops > 9 ? 0 : value;
}

template <>
void LcfReader::Read<int32_t>(int32_t& ref) {
	ref = ReadInt();
}

template <>
void LcfReader::Read<double>(double& ref) {
	Read(&ref, 8, 1);
	SwapByteOrder(ref);
}

template <>
void LcfReader::Read<bool>(std::vector<bool>& buffer, size_t size) {
	buffer.clear();

	for (unsigned i = 0; i < size; ++i) {
		uint8_t val;
		Read(&val, 1, 1);
		buffer.push_back(val > 0);
	}
}

template <>
void LcfReader::Read<uint8_t>(std::vector<uint8_t>& buffer, size_t size) {
	buffer.clear();

	for (unsigned int i = 0; i < size; ++i) {
		uint8_t val;
		Read(&val, 1, 1);
		buffer.push_back(val);
	}
}

template <>
void LcfReader::Read<int16_t>(std::vector<int16_t>& buffer, size_t size) {
	buffer.clear();
	size_t items = size / 2;
	for (unsigned int i = 0; i < items; ++i) {
		int16_t val;
		Read(&val, 2, 1);
		SwapByteOrder(val);
		buffer.push_back(val);
	}
	if (size % 2 != 0) {
		Seek(1, FromCurrent);
		buffer.push_back(0);
	}
}

template <>
void LcfReader::Read<int32_t>(std::vector<int32_t>& buffer, size_t size) {
	buffer.clear();
	size_t items = size / 4;
	for (unsigned int i = 0; i < items; ++i) {
		int32_t val;
		Read(&val, 4, 1);
		SwapByteOrder(val);
		buffer.push_back(val);
	}
	if (size % 4 != 0) {
		Seek(size % 4, FromCurrent);
		buffer.push_back(0);
	}
}

template <>
void LcfReader::Read<uint32_t>(std::vector<uint32_t>& buffer, size_t size) {
	buffer.clear();
	size_t items = size / 4;
	for (unsigned int i = 0; i < items; ++i) {
		uint32_t val;
		Read(&val, 4, 1);
		SwapByteOrder(val);
		buffer.push_back(val);
	}
	if (size % 4 != 0) {
		Seek(size % 4, FromCurrent);
		buffer.push_back(0);
	}
}

void LcfReader::ReadBits(DBBitArray& buffer, size_t size) {
	buffer = DBBitArray(size);
	for (size_t i = 0; i < size; ++i) {
		uint8_t val;
		Read(&val, sizeof(val), 1);
		buffer[i] = static_cast<bool>(val);
	}
}

void LcfReader::ReadString(std::string& ref, size_t size) {
	ref.resize(size);
	Read((size > 0 ? &ref.front(): nullptr), 1, size);
	Encode(ref);
}

void LcfReader::ReadString(DBString& ref, size_t size) {
	auto& tmp = StrBuffer();
	ReadString(tmp, size);
	ref = DBString(tmp);
}

bool LcfReader::IsOk() const {
	return stream.good() && encoder.IsOk();
}

bool LcfReader::Eof() const {
	return stream.eof();
}

void LcfReader::Seek(size_t pos, SeekMode mode) {
	constexpr auto fast_seek_size = 32;
	switch (mode) {
	case LcfReader::FromStart:
		stream.seekg(pos, std::ios_base::beg);
		offset = stream.tellg();
		break;
	case LcfReader::FromCurrent:
		if (pos <= fast_seek_size) {
			// seekg() always results in a system call which is slow.
			// For small values just read and throwaway.
			char buf[fast_seek_size];
			stream.read(buf, pos);
			offset += stream.gcount();
		} else {
			stream.seekg(pos, std::ios_base::cur);
			offset = stream.tellg();
		}
		break;
	case LcfReader::FromEnd:
		stream.seekg(pos, std::ios_base::end);
		offset = stream.tellg();
		break;
	default:
		assert(false && "Invalid SeekMode");
	}
}

uint32_t LcfReader::Tell() {
	// Calling iostream tellg() results in a system call everytime and was found
	// to dominate the runtime of lcf reading. So we cache our own offset.
	// The result of this was shown to have a 30-40% improvement in LDB loading times.
	// return (uint32_t)stream.tellg();
	// This assert can be enabled to verify this method is correct. Disabled by
	// default as it will slow down debug loading considerably.
	// assert(stream.tellg() == offset);
	return offset;
}

int LcfReader::Peek() {
	return stream.peek();
}

void LcfReader::Skip(const struct LcfReader::Chunk& chunk_info, const char* where) {
	Log::Debug("Skipped Chunk %02X (%" PRIu32 " byte) in lcf at %" PRIX32 " (%s)",
			chunk_info.ID, chunk_info.length, Tell(), where);

	std::stringstream ss;
	ss << std::hex;

	for (uint32_t i = 0; i < chunk_info.length; ++i) {
		uint8_t byte;
		LcfReader::Read(byte);
		ss << std::setfill('0') << std::setw(2) << (int)byte << " ";
		if ((i+1) % 16 == 0) {
			Log::Debug("%s", ss.str().c_str());
			ss.str("");
		}
		if (Eof()) {
			break;
		}
	}
	if (!ss.str().empty()) {
		Log::Debug("%s", ss.str().c_str());
	}
}

void LcfReader::SetError(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	char str[256];
#ifdef _MSVC_LANG
	//vsprintf(str, fmt, args);
	vsprintf_s(str, fmt, args);
#else
	vsprintf(str, fmt, args);
#endif

	error_str = str;
	Log::Error("%s", error_str.c_str());

	va_end(args);
}

const std::string& LcfReader::GetError() {
	return error_str;
}

void LcfReader::Encode(std::string& str) {
	encoder.Encode(str);
}

int LcfReader::IntSize(unsigned int x) {
	int result = 0;
	do {
		x >>= 7;
		result++;
	} while (x != 0);
	return result;
}

int LcfReader::UInt64Size(uint64_t x) {
	int result = 0;
	do {
		x >>= 7;
		result++;
	} while (x != 0);
	return result;
}

#ifdef WORDS_BIGENDIAN
void LcfReader::SwapByteOrder(uint16_t& us)
{
	us =	(us >> 8) |
			(us << 8);
}

void LcfReader::SwapByteOrder(uint32_t& ui)
{
	ui =	(ui >> 24) |
			((ui<<8) & 0x00FF0000) |
			((ui>>8) & 0x0000FF00) |
			(ui << 24);
}

void LcfReader::SwapByteOrder(double& d)
{
	char *p = reinterpret_cast<char*>(&d);
	std::swap(p[0], p[7]);
	std::swap(p[1], p[6]);
	std::swap(p[2], p[5]);
	std::swap(p[3], p[4]);
}
#else
void LcfReader::SwapByteOrder(uint16_t& /* us */) {}
void LcfReader::SwapByteOrder(uint32_t& /* ui */) {}
void LcfReader::SwapByteOrder(double& /* d */) {}
#endif

void LcfReader::SwapByteOrder(int16_t& s)
{
	SwapByteOrder((uint16_t&) s);
}

void LcfReader::SwapByteOrder(int32_t& s)
{
	SwapByteOrder((uint32_t&) s);
}

} //namespace lcf
