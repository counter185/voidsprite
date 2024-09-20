/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "reader_struct.h"
#include "lcf/rpg/trooppagecondition.h"
#include "lcf/rpg/eventpagecondition.h"
#include "lcf/rpg/terrain.h"
#include "lcf/rpg/savepicture.h"

#include "ldb_trooppagecondition_flags.h"
#include "ldb_terrain_flags.h"
#include "lmu_eventpagecondition_flags.h"
#include "lsd_savepicture_flags.h"
#include "lsd_saveeasyrpgtext_flags.h"
#include "lsd_saveeasyrpgwindow_flags.h"

namespace lcf {
// Templates

template <class S>
void Flags<S>::ReadLcf(S& obj, LcfReader& stream, uint32_t length) {
	uint8_t byte;
	int bitidx = 0;
	int byteidx = 0;
	stream.Read(byte);
	for (size_t i = 0; i < num_flags; ++i) {
		if (bitidx == 8) {
			++byteidx;
			if (byteidx >= int(length)) {
				break;
			}
			stream.Read(byte);
			bitidx = 0;
		}
		obj.flags[i] = (byte >> bitidx) & 1;
		++bitidx;
	}
#ifdef LCF_DEBUG_TRACE
	unsigned long x = 0;
	for (size_t i = 0; i < obj.flags.size(); ++i) {
		x |= (obj.flags[i] << i);
	}
	fprintf(stderr, "0x%lx\n", x);
#endif
}

template <class S>
void Flags<S>::WriteLcf(const S& obj, LcfWriter& stream) {
	const bool db_is2k3 = stream.Is2k3();

	uint8_t byte = 0;
	int bitidx = 0;
	for (size_t i = 0; i < num_flags; ++i) {
		const auto flag_is2k3 = flags_is2k3[i];
		if (!db_is2k3 && flag_is2k3) {
			continue;
		}
		byte |= (obj.flags[i] << bitidx);

		++bitidx;
		if (bitidx == 8) {
			stream.Write(byte);
			bitidx = 0;
			byte = 0;
		}
	}

	if (bitidx != 0) {
		stream.Write(byte);
	}
}

template <class S>
int Flags<S>::LcfSize(const S& /* obj */, LcfWriter& stream) {
	const bool db_is2k3 = stream.Is2k3();
	int num_bits = 0;
	for (size_t i = 0; i < num_flags; ++i) {
		const auto flag_is2k3 = flags_is2k3[i];
		if (!db_is2k3 && flag_is2k3) {
			continue;
		}
		++num_bits;
	}
	auto num_bytes = (num_bits + 7) / 8;
	return num_bytes;
}

template <class S>
void Flags<S>::WriteXml(const S& obj, XmlWriter& stream) {
	const bool db_is2k3 = stream.Is2k3();
	stream.BeginElement(name);
	for (size_t i = 0; i < num_flags; ++i) {
		const auto flag_is2k3 = flags_is2k3[i];
		if (!db_is2k3 && flag_is2k3) {
			continue;
		}
		const auto* flag_name = flag_names[i];
		stream.WriteNode<bool>(flag_name, obj.flags[i]);
	}
	stream.EndElement(name);
}

template <class S>
class FlagsXmlHandler : public XmlHandler {
private:
	S& obj;
	bool* field;
public:
	FlagsXmlHandler(S& obj) : obj(obj), field(NULL) {
	}

	void StartElement(XmlReader& /* stream */, const char* name, const char** /* atts */) {
		const auto idx = Flags<S>::idx(name);
		if (idx < 0) {
			Log::Error("XML: Unrecognized field '%s'", name);
			field = NULL;
			return;
		}
		field = &obj.flags[idx];
	}
	void EndElement(XmlReader& /* stream */, const char* /* name */) {
		field = NULL;
	}
	void CharacterData(XmlReader& /* stream */, const std::string& data) {
		if (field != NULL)
			XmlReader::Read<bool>(*field, data);
	}
};

template <class S>
void Flags<S>::BeginXml(S& obj, XmlReader& stream) {
	stream.SetHandler(new WrapperXmlHandler(name, new FlagsXmlHandler<S>(obj)));
}

// Instantiate templates
#ifdef _MSC_VER
#pragma warning (disable : 4661)
#endif

// Do not forget to add new Flags here
template class Flags<rpg::TroopPageCondition::Flags>;
template class Flags<rpg::EventPageCondition::Flags>;
template class Flags<rpg::Terrain::Flags>;
template class Flags<rpg::SavePicture::Flags>;
template class Flags<rpg::SaveEasyRpgText::Flags>;
template class Flags<rpg::SaveEasyRpgWindow::Flags>;

} //namespace lcf
