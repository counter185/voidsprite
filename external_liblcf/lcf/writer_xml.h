/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_WRITER_XML_H
#define LCF_WRITER_XML_H

#include <string>
#include <vector>
#include <memory>
#include <cstdio>
#include <stdint.h>
#include "lcf/string_view.h"
#include "lcf/saveopt.h"
#include "lcf/span.h"

namespace lcf {

/**
 * XmlWriter class.
 */
class XmlWriter {

public:
	/**
	 * Constructs a new XML File Writer.
	 *
	 * @param filestream already opened filestream.
	 * @param engine Which engine format to write.
	 */
	XmlWriter(std::ostream& filestream, EngineVersion engine);

	/**
	 * Destructor. Closes the opened file.
	 */
	~XmlWriter();

	/**
	 * Closes the opened file.
	 */
	void Close();

	/**
	 * Writes an integer to the stream.
	 *
	 * @param val the integer.
	 */
	void WriteInt(int val);

	/**
	 * Writes a primitive value to the stream.
	 *
	 * @param val the value.
	 */
	template <class T>
	void Write(const T& val);

	/**
	 * Writes a primitive value in a node to the stream.
	 * Calls BeginElement, Write and EndElement.
	 *
	 * @param name the node name string.
	 * @param val the value.
	 */
	template <class T>
	void WriteNode(const std::string& name, const T& val);

	/**
	 * Writes element starting tag to the stream.
	 *
	 * @param name the element name string.
	 */
	void BeginElement(const std::string& name);

	/**
	 * Writes element starting tag and attribute id to the stream.
	 *
	 * @param name the element name string.
	 * @param ID the attribute ID integer.
	 */
	void BeginElement(const std::string& name, int ID);

	/**
	 * Writes element ending tag to the stream.
	 *
	 * @param name the element name string.
	 */
	void EndElement(const std::string& name);

	/**
	 * Writes a line break to the stream.
	 */
	void NewLine();

	/**
	 * Checks if the file is writable and if no error
	 * occured.
	 *
	 * @return true if the stream is okay.
	 */
	bool IsOk() const;

	/** @return true if 2k3 format, false if 2k format */
	bool Is2k3() const;

protected:
	/** File-stream managed by this Writer. */
	std::ostream& stream;
	/** Stores indentation level. */
	int indent;
	/** Indicates if writer cursor is at the beginning of the line. */
	bool at_bol;
	/** Writing which engine format */
	EngineVersion engine;

	/**
	 * Writes an indentation to the stream.
	 */
	void Indent();

	/**
	 * Writes a vector of primitive values to the stream.
	 *
	 * @param val vector to write.
	 */
	template <class ArrayType>
	void WriteVector(const ArrayType& val);

	void WriteString(StringView s);
};

inline bool XmlWriter::Is2k3() const {
	return engine == EngineVersion::e2k3;
}

} //namespace lcf

#endif
