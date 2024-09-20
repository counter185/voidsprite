/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_WRITER_LCF_H
#define LCF_WRITER_LCF_H

#include "lcf/config.h"
#include "lcf/dbbitarray.h"
#include "lcf/dbstring.h"

#include <string>
#include <vector>
#include <iosfwd>
#include <cstring>
#include <memory>
#include <cassert>
#include <stdint.h>
#include "lcf/reader_util.h"
#include "lcf/encoder.h"
#include "lcf/saveopt.h"

namespace lcf {

/**
 * LcfWriter class.
 */
class LcfWriter {

public:
	/**
	 * Constructs a new File Writer.
	 *
	 * @param filestream already opened filestream.
	 * @param engine Which format to write.
	 * @param encoding name of the encoding.
	 */
	LcfWriter(std::ostream& filestream, EngineVersion engine, std::string encoding = "");

	/**
	 * Writes raw data to the stream (fwrite() wrapper).
	 *
	 * @param ptr pointer to buffer.
	 * @param size size of each element.
	 * @param nmemb number of elements.
	 */
	void Write(const void *ptr, size_t size, size_t nmemb);

	/**
	 * Write a primitive value to the stream.
	 *
	 * @param val the value.
	 */
	template <class T>
	void Write(T val);

	/**
	 * Writes a string to the stream.
	 *
	 * @param str the string.
	 *        Note: the string is converted to the native encoding.
	 */
	void Write(const std::string& str);
	void Write(const DBString& str);

	/**
	 * Writes a bit array to bytes in the stream.
	 *
	 * @param bits the bit array.
	 */
	void Write(const DBBitArray& bits);

	/**
	 * Writes a compressed integer to the stream.
	 *
	 * @param val the integer.
	 */
	void WriteInt(int val);

	/**
	 * Write a compressed 64bit unsigned integer to the stream.
	 *
	 * @return The integer.
	 */
	void WriteUInt64(uint64_t val);

	/**
	 * Write a vector of primitive values to the stream.
	 *
	 * @param buffer vector to write.
	 */
	template <class T>
	void Write(const std::vector<T>& buffer);

	/**
	 * Returns the current position of the read pointer in
	 * the stream.
	 *
	 * @return current location in the stream.
	 */
	uint32_t Tell();

	/**
	 * Checks if the file is writable and if no error occurred.
	 *
	 * @return true the stream is okay.
	 */
	bool IsOk() const;

	/**
	 * Decodes a string from Utf8 to the set encoding
	 * in the Writer constructor.
	 *
	 * @param str_to_encode UTF-8 string to encode.
	 * @return native version of string.
	 */
	std::string Decode(StringView str_to_encode);

	/** @return true if 2k3 format, false if 2k format */
	bool Is2k3() const;

private:
	/** File-stream managed by this Writer. */
	std::ostream& stream;
	/** Encoder object */
	Encoder encoder;
	/** Writing 2k3 format */
	EngineVersion engine;

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

inline bool LcfWriter::Is2k3() const {
	return engine == EngineVersion::e2k3;
}

} //namespace lcf

#endif
