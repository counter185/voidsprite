/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_READER_XML_H
#define LCF_READER_XML_H

#include "lcf/config.h"
#include "lcf/dbarray.h"

#include <string>
#include <vector>
#include <cstdio>
#if LCF_SUPPORT_XML
#  include <expat.h>
#endif
#include <stdint.h>
#include "lcf/reader_util.h"

namespace lcf {

/**
 * XmlHandler abstract base class (forward reference).
 */
class XmlHandler;

/**
 * XmlReader class template.
 */
class XmlReader {

public:
	/**
	 * Constructs a new File Reader.
	 *
	 * @param filestream already opened filestream.
	 */
	XmlReader(std::istream& filestream);

	/**
	 * Destructor. Closes the opened file.
	 */
	~XmlReader();

	/**
	 * Checks if the file is readable and if no error occured.
	 *
	 * @return true if the stream is okay.
	 */
	bool IsOk() const;

	/**
	 * Parses the XML file.
	 */
	void Parse();

	/**
	 * Changes the handler.
	 */
	void SetHandler(XmlHandler* handler);

	/**
	 * Parses a primitive type.
	 */
	template <class T>
	static void Read(T& ref, const std::string& data);

	/**
	 * Parses a vector of primitive type.
	 */
	template <class T>
	static void ReadVector(std::vector<T>& ref, const std::string& data);

	/**
	 * Parses a vector of primitive type.
	 */
	template <class T>
	static void ReadVector(DBArray<T>& ref, const std::string& data);

	/**
	 * Start element callback.
	 */
	void StartElement(const char* name, const char** atts);

	/**
	 * Character data callback.
	 */
	void CharacterData(const char* s, int len);

	/**
	 * End element callback.
	 */
	void EndElement(const char* name);

protected:
	/** File-stream managed by this Reader. */
	std::istream& stream;
	/** Expat XML parser object. */
#if LCF_SUPPORT_XML
	XML_Parser parser;
#else
	void* parser;
#endif
	/** Nesting depth. */
	int nesting;
	/** Handler stack. */
	std::vector<XmlHandler*> handlers;
	/** Text buffer. */
	std::string buffer;

};

/**
 * XmlHandler abstract base class.
 */
class XmlHandler {

public:
	virtual void StartElement(XmlReader& /* reader */, const char* /* name */, const char** /* atts */) {}
	virtual void CharacterData(XmlReader& /* reader */, const std::string& /* data */) {}
	virtual void EndElement(XmlReader& /* reader */, const char* /* name */) {}
	XmlHandler() {}
	virtual ~XmlHandler() {}

};

} //namespace lcf

#endif
