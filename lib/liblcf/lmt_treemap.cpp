/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "lcf/lmt/reader.h"
#include "lcf/lmt/chunks.h"
#include "log.h"
#include "reader_struct.h"

namespace lcf {

template <>
struct RawStruct<rpg::TreeMap> {
	static void ReadLcf(rpg::TreeMap& ref, LcfReader& stream, uint32_t length);
	static void WriteLcf(const rpg::TreeMap& ref, LcfWriter& stream);
	static int LcfSize(const rpg::TreeMap& ref, LcfWriter& stream);
	static void WriteXml(const rpg::TreeMap& ref, XmlWriter& stream);
	static void BeginXml(rpg::TreeMap& ref, XmlReader& stream);
};

/**
 * Reads Map Tree.
 */
void RawStruct<rpg::TreeMap>::ReadLcf(rpg::TreeMap& ref, LcfReader& stream, uint32_t /* length */) {
	Struct<rpg::MapInfo>::ReadLcf(ref.maps, stream);
	for (int i = stream.ReadInt(); i > 0; i--)
		ref.tree_order.push_back(stream.ReadInt());
	ref.active_node = stream.ReadInt();
	Struct<rpg::Start>::ReadLcf(ref.start, stream);
}

void RawStruct<rpg::TreeMap>::WriteLcf(const rpg::TreeMap& ref, LcfWriter& stream) {
	Struct<rpg::MapInfo>::WriteLcf(ref.maps, stream);
	int count = ref.tree_order.size();
	stream.WriteInt(count);
	for (int i = 0; i < count; i++)
		stream.WriteInt(ref.tree_order[i]);
	stream.WriteInt(ref.active_node);
	Struct<rpg::Start>::WriteLcf(ref.start, stream);
}

int RawStruct<rpg::TreeMap>::LcfSize(const rpg::TreeMap& /* ref */, LcfWriter& /* stream */) {
	/* doesn't matter; this structure never occurs within a chunk */
	return 0;
}

void RawStruct<rpg::TreeMap>::WriteXml(const rpg::TreeMap& ref, XmlWriter& stream) {
	stream.BeginElement("TreeMap");

	stream.BeginElement("maps");
	Struct<rpg::MapInfo>::WriteXml(ref.maps, stream);
	stream.EndElement("maps");

	stream.BeginElement("tree_order");
	stream.Write<std::vector<int32_t>>(ref.tree_order);
	stream.EndElement("tree_order");

	stream.WriteNode<int32_t>("active_node", ref.active_node);

	stream.BeginElement("start");
	Struct<rpg::Start>::WriteXml(ref.start, stream);
	stream.EndElement("start");

	stream.EndElement("TreeMap");
}

class TreeMapXmlHandler : public XmlHandler {
private:
	rpg::TreeMap& ref;
	bool active_node;
	bool tree_order;
public:
	TreeMapXmlHandler(rpg::TreeMap& ref) :
		ref(ref), active_node(false), tree_order(false) {}

	void StartElement(XmlReader& stream, const char* name, const char** /* atts */) {
		active_node = false;
		tree_order = false;
		if (strcmp(name, "maps") == 0)
			Struct<rpg::MapInfo>::BeginXml(ref.maps, stream);
		else if (strcmp(name, "tree_order") == 0)
			tree_order = true;
		else if (strcmp(name, "active_node") == 0)
			active_node = true;
		else if (strcmp(name, "start") == 0)
			Struct<rpg::Start>::BeginXml(ref.start, stream);
		else {
			Log::Error("XML: Unrecognized field '%s'", name);
		}
	}
	void EndElement(XmlReader& /* stream */, const char* /* name */) {
		active_node = false;
		tree_order = false;
	}
	void CharacterData(XmlReader& /* stream */, const std::string& data) {
		if (active_node)
			XmlReader::Read<int32_t>(ref.active_node, data);
		if (tree_order)
			XmlReader::Read<std::vector<int32_t>>(ref.tree_order, data);
	}
};

void RawStruct<rpg::TreeMap>::BeginXml(rpg::TreeMap& ref, XmlReader& stream) {
	stream.SetHandler(new WrapperXmlHandler("TreeMap", new TreeMapXmlHandler(ref)));
}

} //namespace lcf
