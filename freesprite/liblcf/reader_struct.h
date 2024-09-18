/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_READER_STRUCT_H
#define LCF_READER_STRUCT_H

#ifdef LCF_DEBUG_TRACE
#include <iostream>
#endif
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <cinttypes>
#include "lcf/dbstring.h"
#include "lcf/reader_lcf.h"
#include "lcf/writer_lcf.h"
#include "lcf/reader_xml.h"
#include "lcf/writer_xml.h"
#include "lcf/rpg/eventpagecondition.h"
#include "lcf/rpg/trooppagecondition.h"
#include "lcf/rpg/terrain.h"
#include "lcf/rpg/equipment.h"
#include "lcf/rpg/parameters.h"
#include "lcf/rpg/eventcommand.h"
#include "lcf/rpg/movecommand.h"
#include "lcf/rpg/treemap.h"
#include "lcf/rpg/rect.h"
#include "lcf/rpg/savepicture.h"
#include "lcf/rpg/terms.h"
#include "lcf/rpg/saveeasyrpgtext.h"
#include "lcf/rpg/saveeasyrpgwindow.h"
#include "log.h"

namespace lcf {

// Forward declarations

template <class T>
class Struct;

// Type categories

struct Category {
	enum Index {
		Primitive,
		Struct,
		Flags,
		RawStruct,
		Void
	};
};

template <class T>
struct TypeCategory {
	static const Category::Index value = Category::Struct;
};

// Do not forget to add new Flags here
template <> struct TypeCategory<rpg::TroopPageCondition::Flags>	{ static const Category::Index value = Category::Flags; };
template <> struct TypeCategory<rpg::EventPageCondition::Flags>	{ static const Category::Index value = Category::Flags; };
template <> struct TypeCategory<rpg::Terrain::Flags>			{ static const Category::Index value = Category::Flags; };
template <> struct TypeCategory<rpg::SavePicture::Flags>		{ static const Category::Index value = Category::Flags; };
template <> struct TypeCategory<rpg::SaveEasyRpgText::Flags>	{ static const Category::Index value = Category::Flags; };
template <> struct TypeCategory<rpg::SaveEasyRpgWindow::Flags>	{ static const Category::Index value = Category::Flags; };

template <> struct TypeCategory<rpg::Equipment>					{ static const Category::Index value = Category::RawStruct; };
template <> struct TypeCategory<rpg::EventCommand>				{ static const Category::Index value = Category::RawStruct; };
template <> struct TypeCategory<rpg::MoveCommand>				{ static const Category::Index value = Category::RawStruct; };
template <> struct TypeCategory<rpg::Parameters>				{ static const Category::Index value = Category::RawStruct; };
template <> struct TypeCategory<rpg::TreeMap>					{ static const Category::Index value = Category::RawStruct; };
template <> struct TypeCategory<rpg::Rect>						{ static const Category::Index value = Category::RawStruct; };
template <>	struct TypeCategory<DBString>						{ static const Category::Index value = Category::RawStruct; };

template <>	struct TypeCategory<int8_t> 						{ static const Category::Index value = Category::Primitive; };
template <>	struct TypeCategory<uint8_t>						{ static const Category::Index value = Category::Primitive; };
template <>	struct TypeCategory<int16_t>						{ static const Category::Index value = Category::Primitive; };
template <>	struct TypeCategory<uint32_t>						{ static const Category::Index value = Category::Primitive; };
template <>	struct TypeCategory<int32_t>						{ static const Category::Index value = Category::Primitive; };
template <>	struct TypeCategory<bool>							{ static const Category::Index value = Category::Primitive; };
template <>	struct TypeCategory<double>							{ static const Category::Index value = Category::Primitive; };
template <>	struct TypeCategory<std::string>					{ static const Category::Index value = Category::Primitive; };
template <>	struct TypeCategory<DBBitArray>						{ static const Category::Index value = Category::Primitive; };

template <class T>
struct TypeCategory<std::vector<T>> {
	static const Category::Index value = TypeCategory<T>::value;
};

/**
 * Typed data readers.
 */
template <class T, Category::Index cat = TypeCategory<T>::value>
struct TypeReader {};


/**
 * Void reader template.
 */

template <class T>
struct TypeReader<T, Category::Void> {
	static void ReadLcf(T&, LcfReader& stream, uint32_t length) {
		stream.Seek(length, LcfReader::FromCurrent);
	}
	static void WriteLcf(const T&, LcfWriter&) {
	}
	static int LcfSize(const T&, LcfWriter&) {
		return 0;
	}
	static void WriteXml(const T&, XmlWriter&) {
	}
	static void BeginXml(T&, XmlReader&) {
	}
	static void ParseXml(T& /* ref */, const std::string& /* data */) {
		//no-op
	}
};

/**
 * Raw structure reader template.
 */
template <class T>
struct RawStruct {
	static void ReadLcf(T& ref, LcfReader& stream, uint32_t length);
	static void WriteLcf(const T& ref, LcfWriter& stream);
	static int LcfSize(const T& ref, LcfWriter& stream);
	static void WriteXml(const T& ref, XmlWriter& stream);
	static void BeginXml(T& ref, XmlReader& stream);
};

template <class T>
struct TypeReader<T, Category::RawStruct> {
	static void ReadLcf(T& ref, LcfReader& stream, uint32_t length) {
		RawStruct<T>::ReadLcf(ref, stream, length);
	}
	static void WriteLcf(const T& ref, LcfWriter& stream) {
		RawStruct<T>::WriteLcf(ref, stream);
	}
	static int LcfSize(const T& ref, LcfWriter& stream) {
		return RawStruct<T>::LcfSize(ref, stream);
	}
	static void WriteXml(const T& ref, XmlWriter& stream) {
		RawStruct<T>::WriteXml(ref, stream);
	}
	static void BeginXml(T& ref, XmlReader& stream) {
		RawStruct<T>::BeginXml(ref, stream);
	}
	static void ParseXml(T& /* ref */, const std::string& /* data */) {
		//no-op
	}
};

/**
 * Type Lcf size.
 */
template <class T>
struct LcfSizeT {
	static const uint32_t value = sizeof(T);
};

/**
 * Type Lcf bool size.
 */
template <>
struct LcfSizeT<bool> {
	static const uint32_t value = 1;
};

/**
 * Primitive type reader template.
 */
template <class T>
struct Primitive {
	static void ReadLcf(T& ref, LcfReader& stream, uint32_t length) {
		int dif = 0;
		// FIXME: Bug #174
		if (length != LcfSizeT<T>::value) {
			dif = length - LcfSizeT<T>::value;
			Log::Warning("Reading Primitive of incorrect size %" PRIu32 " (expected %" PRIu32 ") at %" PRIX32 "",
				length, LcfSizeT<T>::value, stream.Tell());
		}

		stream.Read(ref);
#ifdef LCF_DEBUG_TRACE
		DebugPrint(ref);
#endif

		if (dif != 0) {
			// Fix incorrect read pointer position
			Log::Warning("Invalid Primitive at %" PRIX32 "", stream.Tell());
			stream.Seek(dif, LcfReader::FromCurrent);
		}
	}
	static void WriteLcf(const T& ref, LcfWriter& stream) {
		stream.Write(ref);
	}
	static int LcfSize(const T& /* ref */, LcfWriter& /* stream */) {
		return LcfSizeT<T>::value;
	}
	static void WriteXml(const T& ref, XmlWriter& stream) {
		stream.Write(ref);
	}
	static void ParseXml(T& ref, const std::string& data) {
		XmlReader::Read(ref, data);
	}
	private:
#ifdef LCF_DEBUG_TRACE
	template <typename U>
		static void DebugPrint(U& ref) {
			std::cout << ref << '\n';
		}
	static void DebugPrint(int8_t ref) {
		std::cout << (int)ref << '\n';
	}
	static void DebugPrint(uint8_t ref) {
		std::cout << (int)ref << '\n';
	}
#endif
};

/**
 * Vector specialization.
 */
template <class T>
struct Primitive<std::vector<T>> {
	static void ReadLcf(std::vector<T>& ref, LcfReader& stream, uint32_t length) {
		stream.Read(ref, length);
#ifdef LCF_DEBUG_TRACE
		typename std::vector<T>::iterator it;
		fprintf(stderr, "  ");
		for (it = ref.begin(); it != ref.end(); ++it) {
			fprintf(stderr, "%d, ", static_cast<int>(*it));
		}
		fprintf(stderr, "\n");
#endif
	}
	static void WriteLcf(const std::vector<T>& ref, LcfWriter& stream) {
		stream.Write(ref);
	}
	static int LcfSize(const std::vector<T>& ref, LcfWriter& /* stream */) {
		return LcfSizeT<T>::value * ref.size();
	}
	static void WriteXml(const std::vector<T>& ref, XmlWriter& stream) {
		stream.Write(ref);
	}
	static void ParseXml(std::vector<T>& ref, const std::string& data) {
		XmlReader::Read(ref, data);
	}
};

/**
 * Int specialization.
 */
template <>
struct Primitive<int32_t> {
	static void ReadLcf(int32_t& ref, LcfReader& stream, uint32_t length) {
		if (length >= 1 && length <= 5) {
			ref = stream.ReadInt();
#ifdef LCF_DEBUG_TRACE
			fprintf(stderr, "  %d\n", ref);
#endif
		} else if (length == 0) {
			ref = 0;
#ifdef LCF_DEBUG_TRACE
			fprintf(stderr, "  %d\n", ref);
#endif
		} else {
			ref = 0;
			Log::Warning("Invalid integer at %X", stream.Tell());
			stream.Seek(length, LcfReader::FromCurrent);
		}
	}
	static void WriteLcf(const int32_t& ref, LcfWriter& stream) {
		stream.WriteInt(ref);
	}
	static int LcfSize(const int32_t& ref, LcfWriter& /* stream */) {
		return LcfReader::IntSize(ref);
	}
	static void WriteXml(const int32_t& ref, XmlWriter& stream) {
		stream.WriteInt(ref);
	}
	static void ParseXml(int32_t& ref, const std::string& data) {
		XmlReader::Read(ref, data);
	}
};

/**
 * String specialization.
 */
template <>
struct Primitive<std::string> {
	static void ReadLcf(std::string& ref, LcfReader& stream, uint32_t length) {
		stream.ReadString(ref, length);
#ifdef LCF_DEBUG_TRACE
		fprintf(stderr, "  %s\n", ref.c_str());
#endif
	}
	static void WriteLcf(const std::string& ref, LcfWriter& stream) {
		stream.Write(ref);
	}
	static int LcfSize(const std::string& ref, LcfWriter& stream) {
		return stream.Decode(ref).size();
	}
	static void WriteXml(const std::string& ref, XmlWriter& stream) {
		stream.Write(ref);
	}
	static void ParseXml(std::string& ref, const std::string& data) {
		XmlReader::Read(ref, data);
	}
};

/**
 * DBBitArray specialization.
 */
template <>
struct Primitive<DBBitArray> {
	static void ReadLcf(DBBitArray& ref, LcfReader& stream, uint32_t length) {
		stream.ReadBits(ref, length);
#ifdef LCF_DEBUG_TRACE
		fprintf(stderr, "  ");
		for (auto& b: ref) {
			fprintf(stderr, "%d", static_cast<int>(b));
		}
		fprintf(stderr, "\n");
#endif
	}
	static void WriteLcf(const DBBitArray& ref, LcfWriter& stream) {
		stream.Write(ref);
	}
	static int LcfSize(const DBBitArray& ref, LcfWriter& stream) {
		(void)stream;
		return ref.size();
	}
	static void WriteXml(const DBBitArray& ref, XmlWriter& stream) {
		stream.Write(ref);
	}
	static void ParseXml(DBBitArray& ref, const std::string& data) {
		XmlReader::Read(ref, data);
	}
};



/**
 * Primitive Reader.
 */
template <class T>
struct TypeReader<T, Category::Primitive> {
	static void ReadLcf(T& ref, LcfReader& stream, uint32_t length) {
		Primitive<T>::ReadLcf(ref, stream, length);
	}
	static void WriteLcf(const T& ref, LcfWriter& stream) {
		Primitive<T>::WriteLcf(ref, stream);
	}
	static int LcfSize(const T& ref, LcfWriter& stream) {
		return Primitive<T>::LcfSize(ref, stream);
	}
	static void WriteXml(const T& ref, XmlWriter& stream) {
		Primitive<T>::WriteXml(ref, stream);
	}
	static void BeginXml(T& /* ref */, XmlReader& /* stream */) {
		// no-op
	}
	static void ParseXml(T& ref, const std::string& data) {
		Primitive<T>::ParseXml(ref, data);
	}
};

/**
 * Field abstract base class template.
 */
template <class S>
struct Field {
	typedef S struct_type;

	const char* const name;
	int id;
	bool present_if_default;
	bool is2k3;

	virtual void ReadLcf(S& obj, LcfReader& stream, uint32_t length) const = 0;
	virtual void WriteLcf(const S& obj, LcfWriter& stream) const = 0;
	virtual int LcfSize(const S& obj, LcfWriter& stream) const = 0;
	virtual bool IsDefault(const S& obj, const S& ref, bool is2k3) const = 0;
	virtual void WriteXml(const S& obj, XmlWriter& stream) const = 0;
	virtual void BeginXml(S& obj, XmlReader& stream) const = 0;
	virtual void ParseXml(S& obj, const std::string& data) const = 0;

	bool isPresentIfDefault(bool db_is2k3) const {
		if (std::is_same<S,rpg::Terms>::value && db_is2k3 && (id == 0x3 || id == 0x1)) {
			//Special case - only known fields that are 2k specific and not
			//written to a 2k3 db if defaulted.
			return false;
		}
		return present_if_default;
	}

	Field(int id, const char* name, bool present_if_default, bool is2k3) :
		name(name), id(id), present_if_default(present_if_default), is2k3(is2k3) {}
};

/**
 * TypedField class template.
 */
template <class S, class T>
struct TypedField : public Field<S> {
	T S::*ref;

	void ReadLcf(S& obj, LcfReader& stream, uint32_t length) const {
		TypeReader<T>::ReadLcf(obj.*ref, stream, length);
	}
	void WriteLcf(const S& obj, LcfWriter& stream) const {
		TypeReader<T>::WriteLcf(obj.*ref, stream);
	}
	int LcfSize(const S& obj, LcfWriter& stream) const {
		return TypeReader<T>::LcfSize(obj.*ref, stream);
	}
	void WriteXml(const S& obj, XmlWriter& stream) const {
		stream.BeginElement(this->name);
		TypeReader<T>::WriteXml(obj.*ref, stream);
		stream.EndElement(this->name);
	}
	void BeginXml(S& obj, XmlReader& stream) const {
		TypeReader<T>::BeginXml(obj.*ref, stream);
	}
	void ParseXml(S& obj, const std::string& data) const {
		TypeReader<T>::ParseXml(obj.*ref, data);
	}
	bool IsDefault(const S& a, const S& b, bool) const {
		return a.*ref == b.*ref;
	}

	TypedField(T S::*ref, int id, const char* name, bool present_if_default, bool is2k3) :
		Field<S>(id, name, present_if_default, is2k3), ref(ref) {}
};

/**
 * DatabaseVersionField class template.
 */

template <typename S, typename T>
struct DatabaseVersionField : public TypedField<S,T> {

	using TypedField<S,T>::TypedField;

	int LcfSize(const S& obj, LcfWriter& stream) const {
		//If db version is 0, it's like a "version block" is not present.
		if ((obj.*(this->ref)) == 0) {
			return 0;
		}
		return TypedField<S,T>::LcfSize(obj, stream);
	}
	bool IsDefault(const S& a, const S& b, bool is2k3) const {
		if (is2k3) {
			//DB Version always present in 2k3 db
			return false;
		}
		//Only present if not 0 in 2k db.
		return TypedField<S,T>::IsDefault(a, b, is2k3);
	}
};

/**
 * EmptyField class template.
 */

template <typename S>
struct EmptyField : public Field<S> {

	using Field<S>::Field;

	void ReadLcf(S& /* obj */, LcfReader& /* stream */, uint32_t /* length */) const { }
	void WriteLcf(const S& /* obj */, LcfWriter& /* stream */) const { }
	int LcfSize(const S& /* obj */, LcfWriter& /* stream */) const {
		//This is always an "empty block"
		return 0;
	}
	void WriteXml(const S& /* obj */, XmlWriter& /* stream */) const { }
	void BeginXml(S& /* obj */, XmlReader& /* stream */) const { }
	void ParseXml(S& /* obj */, const std::string& /* data */) const { }

	bool IsDefault(const S& /* a */, const S& /* b */, bool) const {
		return true;
	}

};



/**
 * SizeField class template.
 */
template <class S, class T>
struct SizeField : public Field<S> {
	const T S::*ref;

	void ReadLcf(S& /* obj */, LcfReader& stream, uint32_t length) const {
		int32_t dummy;
		TypeReader<int32_t>::ReadLcf(dummy, stream, length);
	}
	void WriteLcf(const S& obj, LcfWriter& stream) const {
		int size = TypeReader<T>::LcfSize(obj.*ref, stream);
		TypeReader<int32_t>::WriteLcf(size, stream);
	}
	int LcfSize(const S& obj, LcfWriter& stream) const {
		int size = TypeReader<T>::LcfSize(obj.*ref, stream);
		return LcfReader::IntSize(size);
	}
	void WriteXml(const S& /* obj */, XmlWriter& /* stream */) const {
		// no-op
	}
	void BeginXml(S& /* obj */, XmlReader& /* stream */) const {
		// no-op
	}
	void ParseXml(S& /* obj */, const std::string& /* data */) const {
		// no-op
	}
	bool IsDefault(const S& a, const S& b, bool) const {
		return (a.*ref).size() == (b.*ref).size();
	}

	SizeField(const T S::*ref, int id, bool present_if_default, bool is2k3) :
		Field<S>(id, "", present_if_default, is2k3), ref(ref) {}
};


/**
 * CountField class template.
 */
template <class S, class T>
struct CountField : public SizeField<S,T> {

	using SizeField<S,T>::SizeField;

	void WriteLcf(const S& obj, LcfWriter& stream) const {
		int size = (obj.*(this->ref)).size();
		TypeReader<int32_t>::WriteLcf(size, stream);
	}
	int LcfSize(const S& obj, LcfWriter& /* stream */) const {
		int size = (obj.*(this->ref)).size();
		return LcfReader::IntSize(size);
	}
};

/**
 * ID handling for Struct class.
 */
template <class T>
struct IDChecker {
	typedef char no;
	typedef int yes;

	template <typename U, U> struct type_check;
	template <class C>
	static yes check(type_check<int C::*, &C::ID> *);
	template <class C>
	static no  check(...);

	static const bool value = sizeof(check<T>(0)) == sizeof(yes);
};

// ID reader for Struct class

template <class S, bool T>
struct IDReaderT {
};

template <class S>
struct IDReaderT<S, true> {
	static void ReadID(S& obj, LcfReader& stream) {
		obj.ID = stream.ReadInt();
	}
	static void WriteID(const S& obj, LcfWriter& stream) {
		stream.WriteInt(obj.ID);
	}
	static int IDSize(const S& obj) {
		return LcfReader::IntSize(obj.ID);
	}
	static void WriteXmlTag(const S& obj, const std::string& name, XmlWriter& stream) {
		stream.BeginElement(name, obj.ID);
	}
	static void ReadIDXml(S& obj, const char** atts) {
		for (int i = 0; atts[i] != NULL && atts[i + 1] != NULL; i += 2) {
			if (strcmp(atts[i], "id") == 0)
				obj.ID = atoi(atts[i + 1]);
		}
	}
};

template <class S>
struct IDReaderT<S, false> {
	static void ReadID(S& /* obj */, LcfReader& /* stream */) {}
	static void WriteID(const S& /* obj */, LcfWriter& /* stream */) {}
	static int IDSize(const S& /* obj */) { return 0; }
	static void WriteXmlTag(const S& /* obj */, const std::string& name, XmlWriter& stream) {
		stream.BeginElement(name);
	}
	static void ReadIDXml(S& /* obj */, const char** /* atts */) {}
};

struct StringComparator {
	bool operator() (const char* const& lhs, const char* const& rhs) const {
		return strcmp(lhs, rhs) < 0;
	}
};

// Struct class template

template <class S>
class Struct {
private:
	typedef std::map<int, const Field<S>* > field_map_type;
	typedef std::map<const char* const, const Field<S>*, StringComparator> tag_map_type;
	typedef IDReaderT<S, IDChecker<S>::value > IDReader;
	static const Field<S>* fields[];
	static field_map_type field_map;
	static tag_map_type tag_map;
	static const char* const name;

	static void MakeFieldMap();
	static void MakeTagMap();

	template <class T> friend class StructXmlHandler;
	template <class T> friend class StructVectorXmlHandler;
	template <class T> friend class StructFieldXmlHandler;

public:
	static void ReadLcf(S& obj, LcfReader& stream);
	static void WriteLcf(const S& obj, LcfWriter& stream);
	static int LcfSize(const S& obj, LcfWriter& stream);
	static void WriteXml(const S& obj, XmlWriter& stream);
	static void BeginXml(S& obj, XmlReader& stream);

	static void ReadLcf(std::vector<S>& obj, LcfReader& stream);
	static void WriteLcf(const std::vector<S>& obj, LcfWriter& stream);
	static int LcfSize(const std::vector<S>& obj, LcfWriter& stream);
	static void WriteXml(const std::vector<S>& obj, XmlWriter& stream);
	static void BeginXml(std::vector<S>& obj, XmlReader& stream);
};

template <class S>
std::map<int, const Field<S>* > Struct<S>::field_map;

template <class S>
std::map<const char* const, const Field<S>*, StringComparator> Struct<S>::tag_map;

/**
 * Struct reader.
*/
template <class T>
struct TypeReader<T, Category::Struct> {
	static void ReadLcf(T& ref, LcfReader& stream, uint32_t /* length */) {
		Struct<T>::ReadLcf(ref, stream);
	}
	static void WriteLcf(const T& ref, LcfWriter& stream) {
		Struct<T>::WriteLcf(ref, stream);
	}
	static int LcfSize(const T& ref, LcfWriter& stream) {
		return Struct<T>::LcfSize(ref, stream);
	}
	static void WriteXml(const T& ref, XmlWriter& stream) {
		Struct<T>::WriteXml(ref, stream);
	}
	static void BeginXml(T& ref, XmlReader& stream) {
		Struct<T>::BeginXml(ref, stream);
	}
	static void ParseXml(T& /* ref */, const std::string& /* data */) {
		// no-op
	}
};

template <class T>
struct TypeReader<std::vector<T>, Category::Struct> {
	static void ReadLcf(std::vector<T>& ref, LcfReader& stream, uint32_t /* length */) {
		Struct<T>::ReadLcf(ref, stream);
	}
	static void WriteLcf(const std::vector<T>& ref, LcfWriter& stream) {
		Struct<T>::WriteLcf(ref, stream);
	}
	static int LcfSize(const std::vector<T>& ref, LcfWriter& stream) {
		return Struct<T>::LcfSize(ref, stream);
	}
	static void WriteXml(const std::vector<T>& ref, XmlWriter& stream) {
		Struct<T>::WriteXml(ref, stream);
	}
	static void BeginXml(std::vector<T>& ref, XmlReader& stream) {
		Struct<T>::BeginXml(ref, stream);
	}
	static void ParseXml(std::vector<T>& /* ref */, const std::string& /* data */) {
		// no-op
	}
};



/**
 * Flags class template.
 */
template <class S>
class Flags {
private:
	static const char* const name;
	static constexpr size_t num_flags = std::tuple_size<decltype(S::flags)>::value;
	static const std::array<const char* const, num_flags> flag_names;
	static const std::array<bool, num_flags> flags_is2k3;

public:
	static const char* tag(int idx);
	static int idx(const char* tag);

	static void ReadLcf(S& obj, LcfReader& stream, uint32_t length);
	static void WriteLcf(const S& obj, LcfWriter& stream);
	static int LcfSize(const S& obj, LcfWriter& stream);
	static void WriteXml(const S& obj, XmlWriter& stream);
	static void BeginXml(S& obj, XmlReader& stream);
};

template <class S>
inline const char* Flags<S>::tag(int idx) {
	return Flags<S>::flag_names[idx];
}

template <class S>
inline int Flags<S>::idx(const char* tag) {
	for (size_t i = 0; i < flag_names.size(); ++i) {
		if (std::strcmp(flag_names[i], tag) == 0) {
			return i;
		}
	}
	return -1;
}

/**
 * Wrapper XML handler struct.
 */
template <class T>
struct TypeReader<T, Category::Flags> {
	static void ReadLcf(T& ref, LcfReader& stream, uint32_t length) {
		Flags<T>::ReadLcf(ref, stream, length);
	}
	static void WriteLcf(const T& ref, LcfWriter& stream) {
		Flags<T>::WriteLcf(ref, stream);
	}
	static int LcfSize(const T& ref, LcfWriter& stream) {
		return Flags<T>::LcfSize(ref, stream);
	}
	static void WriteXml(const T& ref, XmlWriter& stream) {
		Flags<T>::WriteXml(ref, stream);
	}
	static void BeginXml(T& ref, XmlReader& stream) {
		Flags<T>::BeginXml(ref, stream);
	}
	static void ParseXml(T& /* ref */, const std::string& /* data */) {
		// no-op
	}
};

/**
 * Wrapper XML handler class.
 */
class WrapperXmlHandler : public XmlHandler {
public:
	WrapperXmlHandler(const char* const name, XmlHandler* handler) :
		name(name), handler(handler) {}

	void StartElement(XmlReader& stream, const char* name, const char** /* atts */) {
		if (strcmp(name, this->name) != 0)
			Log::Error("XML: Expecting %s but got %s", this->name, name);
		stream.SetHandler(handler);
	}

private:
	const char* const name;
	XmlHandler* handler;
};

/**
 * Root node XML handler.
 */
template <class S>
class RootXmlHandler : public XmlHandler {

public:
	RootXmlHandler(S& ref, const char* const name) : ref(ref), name(name) {}

	void StartElement(XmlReader& stream, const char* name, const char** /* atts */) {
		if (strcmp(name, this->name) != 0)
			Log::Error("XML: Expecting %s but got %s", this->name, name);
		TypeReader<S>::BeginXml(ref, stream);
	}

private:
	S& ref;
	const char* const name;

};

} //namespace lcf

#endif
