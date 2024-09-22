/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_READER_LCF_H
#define LCF_READER_LCF_H

#include "lcf/config.h"
#include "lcf/dbstring.h"
#include "lcf/dbbitarray.h"

#include <string>
#include <vector>
#include <iosfwd>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <cinttypes>
#include <memory>
#include "lcf/reader_util.h"
#include "lcf/encoder.h"

namespace lcf {

/**
 * LcfReader class.
 */
class LcfReader {
public:
	/**
	 * Constructs a new File Reader.
	 *
	 * @param filestream already opened filestream.
	 * @param encoding name of the encoding.
	 */
	explicit LcfReader(std::istream& filestream, std::string encoding = "");

	/**
	 * Returns the last set error.
	 *
	 * @return Error Message.
	 */
	static const std::string& GetError();

	/**
	 * Sets the error message of the Reader.
	 * This is not used by the Reader directly
	 * but by the classes that are using the Reader.
	 *
	 * @param fmt error message.
	 */
	static void SetError(const char* fmt, ...);

	/**
	 * The chunk defines the basic layout of the binary blocks
	 * used by the RPG Maker files.
	 */
	struct Chunk {
		Chunk() {
			ID = 0;
			length = 0;
		}
		uint32_t ID;
		uint32_t length;
	};

	/**
	 * Defines the Seek start position for the Seek function.
	 */
	enum SeekMode {
		FromStart,
		FromEnd,
		FromCurrent
	};

	/**
	 * Reads raw data from the stream (fread() wrapper).
	 *
	 * @param ptr pointer to buffer.
	 * @param size size of each element.
	 * @param nmemb number of elements.
	 * @return number of elements read.
	 */
	size_t Read0(void *ptr, size_t size, size_t nmemb);

	/**
	 * Reads raw data from the stream (fread() wrapper).
	 *
	 * @param ptr pointer to buffer.
	 * @param size size of each element.
	 * @param nmemb number of elements.
	 */
	void Read(void *ptr, size_t size, size_t nmemb);

	/**
	 * Reads a primitive type.
	 *
	 * @param ref reference to store result.
	 */
	template <class T>
	void Read(T& ref);

	/**
	 * Reads a vector of primitive type.
	 *
	 * @param buffer vector to fill.
	 * @param size how many bytes to read.
	 */
	template <class T>
	void Read(std::vector<T> &buffer, size_t size);

	/**
	 * Reads a DBBitsArray of primitive type.
	 *
	 * @param buffer DBBitArray to fill.
	 * @param size how many bytes to read as bits.
	 */
	void ReadBits(DBBitArray &buffer, size_t size);

	/**
	 * Reads a compressed integer from the stream.
	 *
	 * @return The decompressed integer.
	 */
	int ReadInt();

	/**
	 * Reads a compressed 64bit unsigned integer from the stream.
	 *
	 * @return The decompressed integer.
	 */
	uint64_t ReadUInt64();

	/**
	 * Reads a string.
	 *
	 * @param size string length.
	 * @param ref reference to store result.
	 *        Note: The string is converted to UTF-8.
	 */
	void ReadString(std::string& ref, size_t size);
	void ReadString(DBString& ref, size_t size);

	/**
	 * Checks if the file is readable and if no error occured.
	 *
	 * @return true if the stream is okay.
	 */
	bool IsOk() const;

	/**
	 * Checks if the end of the file has been reached.
	 *
	 * @return If the end of file is reached.
	 */
	bool Eof() const;

	/**
	 * Moves the read pointer to a different position in
	 * the stream.
	 *
	 * @param pos new position.
	 * @param mode seek mode.
	 */
	void Seek(size_t pos, SeekMode mode = FromStart);

	/**
	 * Returns the current position of the read pointer in
	 * the stream.
	 *
	 * @return current location in the stream.
	 */
	uint32_t Tell();

	/**
	 * Returns the next char in the stream and doesn't advance the position.
	 *
	 * @return next char in the buffer or EOF
	 */
	int Peek();

	/**
	 * Skips a chunk (seeks chunk_info.length bytes from
	 * the current stream position).
	 * In debug builds, dumps the content of the
	 * skipped chunk to stderr.
	 *
	 * @param chunk_info chunk that will be skipped.
	 * @param where name of the caller that caused the skip, for finding unknown chunks
	 */
	void Skip(const struct LcfReader::Chunk& chunk_info, const char* where);

	/**
	 * Encodes a string to UTF-8 using the set encoding
	 * in the reader constructor.
	 *
	 * @param str to convert from encoding to UTF-8
	 */
	void Encode(std::string& str);

	/**
	 * Calculates the size of a compressed integer.
	 *
	 * @param x the integer.
	 * @return the compressed size.
	 */
	static int IntSize(unsigned int x);

	/**
	 * Calculates the size of a compressed 64bit unsigned integer
	 *
	 * @param x the integer.
	 * @return the compressed size.
	 */
	static int UInt64Size(uint64_t x);

	/** @return a buffer which can be reused for parsing */
	std::vector<int32_t>& IntBuffer();

	/** @return a buffer which can be reused for parsing */
	std::string& StrBuffer();

private:
	/** File-stream managed by this Reader. */
	std::istream& stream;
	/** Cached file stream offset */
	int64_t offset;
	/** Contains the last set error. */
	static std::string error_str;
	/** The internal Encoder */
	Encoder encoder;
	/** A temporary buffer to be used in parsing */
	std::vector<int32_t> buffer;
	/** A temporary buffer to be used in parsing */
	std::string str_buffer;

	/**
	 * Converts a 16bit signed integer to/from little-endian.
	 *
	 * @param us integer to convert.
	 */
	static void SwapByteOrder(int16_t &us);

	/**
	 * Converts a 16bit unsigned integer to/from little-endian.
	 *
	 * @param us integer to convert.
	 */
	static void SwapByteOrder(uint16_t &us);

	/**
	 * Converts a 32bit signed integer to/from little-endian.
	 *
	 * @param us integer to convert.
	 */
	static void SwapByteOrder(int32_t &us);

	/**
	 * Converts a 32bit unsigned integer to/from little-endian.
	 *
	 * @param ui integer to convert.
	 */
	static void SwapByteOrder(uint32_t &ui);

	/**
	 * Converts a double to/from little-endian.
	 *
	 * @param d double to convert.
	 */
	static void SwapByteOrder(double &d);
};

inline std::vector<int32_t>& LcfReader::IntBuffer() {
	return buffer;
}

inline std::string& LcfReader::StrBuffer() {
	return str_buffer;
}

} //namespace lcf

#endif
