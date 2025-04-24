/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <string>
#include <vector>
#include "log.h"
#include "reader_struct.h"
#include "lcf/rpg/eventcommand.h"

namespace lcf {

template <>
struct RawStruct<rpg::EventCommand> {
	static void ReadLcf(rpg::EventCommand& ref, LcfReader& stream, uint32_t length);
	static void WriteLcf(const rpg::EventCommand& ref, LcfWriter& stream);
	static int LcfSize(const rpg::EventCommand& ref, LcfWriter& stream);
	static void WriteXml(const rpg::EventCommand& ref, XmlWriter& stream);
	static void BeginXml(rpg::EventCommand& ref, XmlReader& stream);
};

template <>
struct RawStruct<std::vector<rpg::EventCommand> > {
	static void ReadLcf(std::vector<rpg::EventCommand>& ref, LcfReader& stream, uint32_t length);
	static void WriteLcf(const std::vector<rpg::EventCommand>& ref, LcfWriter& stream);
	static int LcfSize(const std::vector<rpg::EventCommand>& ref, LcfWriter& stream);
	static void WriteXml(const std::vector<rpg::EventCommand>& ref, XmlWriter& stream);
	static void BeginXml(std::vector<rpg::EventCommand>& ref, XmlReader& stream);
};

/**
 * Reads Event Command.
 */
void RawStruct<rpg::EventCommand>::ReadLcf(rpg::EventCommand& event_command, LcfReader& stream, uint32_t /* length */) {
	stream.Read(event_command.code);
	if (event_command.code != 0) {
		stream.Read(event_command.indent);
		stream.ReadString(event_command.string, stream.ReadInt());

		auto& param_buf = stream.IntBuffer();

		param_buf.clear();
		for (int i = stream.ReadInt(); i > 0; i--) {
			param_buf.push_back(stream.ReadInt());
		}
		if (!param_buf.empty()) {
			event_command.parameters = DBArray<int32_t>(param_buf.begin(), param_buf.end());
		}
	}
}

void RawStruct<rpg::EventCommand>::WriteLcf(const rpg::EventCommand& event_command, LcfWriter& stream) {
	stream.Write(event_command.code);
	stream.Write(event_command.indent);
	stream.WriteInt(stream.Decode(event_command.string).size());
	stream.Write(event_command.string);
	int32_t count = (int32_t)event_command.parameters.size();
	stream.Write(count);
	for (int i = 0; i < count; i++)
		stream.Write(event_command.parameters[i]);
}

int RawStruct<rpg::EventCommand>::LcfSize(const rpg::EventCommand& event_command, LcfWriter& stream) {
	int result = 0;
	result += LcfReader::IntSize(event_command.code);
	result += LcfReader::IntSize(event_command.indent);
	result += LcfReader::IntSize(stream.Decode(event_command.string).size());
	result += stream.Decode(event_command.string).size();
	int count = event_command.parameters.size();
	result += LcfReader::IntSize(count);
	for (int i = 0; i < count; i++)
		result += LcfReader::IntSize(event_command.parameters[i]);
	return result;
}

void RawStruct<rpg::EventCommand>::WriteXml(const rpg::EventCommand& event_command, XmlWriter& stream) {
	stream.BeginElement("EventCommand");
	stream.WriteNode("code", event_command.code);
	stream.WriteNode("indent", event_command.indent);
	stream.WriteNode("string", event_command.string);
	stream.WriteNode("parameters", event_command.parameters);
	stream.EndElement("EventCommand");
}

class EventCommandXmlHandler : public XmlHandler {
private:
	rpg::EventCommand& ref;
	enum {
		None,
		Code,
		Indent,
		String,
		Parameters
	} field;
public:
	EventCommandXmlHandler(rpg::EventCommand& ref) : ref(ref), field(None) {}
	void StartElement(XmlReader& /* stream */, const char* name, const char** /* atts */) {
		if (strcmp(name, "code") == 0)
			field = Code;
		else if (strcmp(name, "indent") == 0)
			field = Indent;
		else if (strcmp(name, "string") == 0)
			field = String;
		else if (strcmp(name, "parameters") == 0)
			field = Parameters;
		else {
			Log::Error("XML: Unrecognized field '%s'", name);
			field = None;
		}
	}
	void EndElement(XmlReader& /* stream */, const char* /* name */) {
		field = None;
	}
	void CharacterData(XmlReader& /* stream */, const std::string& data) {
		switch (field) {
			case None:
				break;
			case Code:
				XmlReader::Read(ref.code, data);
				break;
			case Indent:
				XmlReader::Read(ref.indent, data);
				break;
			case String:
				XmlReader::Read(ref.string, data);
				break;
			case Parameters:
				XmlReader::Read(ref.parameters, data);
				break;
		}
	}
};

void RawStruct<rpg::EventCommand>::BeginXml(rpg::EventCommand& ref, XmlReader& stream) {
	stream.SetHandler(new WrapperXmlHandler("EventCommand", new EventCommandXmlHandler(ref)));
}

/**
 * Reads event commands.
 */
void RawStruct<std::vector<rpg::EventCommand> >::ReadLcf(
	std::vector<rpg::EventCommand>& event_commands, LcfReader& stream, uint32_t length) {
	// Event Commands is a special array
	// Has no size information. Is terminated by 4 times 0x00.
	unsigned long startpos = stream.Tell();
	unsigned long endpos = startpos + length;

	// Since we don't know the number of event parameters without reading, we store
	// them all in a temporary buffer and then copy it to EventCommand::parameters.
	// This prevents extra allocations from repeated calls to push_back().
	for (;;) {
		uint8_t ch = (uint8_t)stream.Peek();
		if (ch == 0) {
			stream.Seek(4, LcfReader::FromCurrent);
			break;
		}

		if (stream.Tell() >= endpos) {
			stream.Seek(endpos, LcfReader::FromStart);
			Log::Warning("Event command corrupted at %" PRIu32 "", stream.Tell());
			for (;;) {
				// Try finding the real end of the event command (4 0-bytes)
				int i = 0;
				for (; i < 4; ++i) {
					stream.Read(ch);

					if (ch != 0) {
						break;
					}
				}

				if (i == 4 || stream.Eof()) {
					break;
				}
			}

			break;
		}

		rpg::EventCommand command;
		RawStruct<rpg::EventCommand>::ReadLcf(command, stream, 0);
		event_commands.push_back(command);
	}
}

void RawStruct<std::vector<rpg::EventCommand> >::WriteLcf(const std::vector<rpg::EventCommand>& event_commands, LcfWriter& stream) {
	int count = event_commands.size();
	for (int i = 0; i < count; i++)
		RawStruct<rpg::EventCommand>::WriteLcf(event_commands[i], stream);
	for (int i = 0; i < 4; i++)
		stream.WriteInt(0);
}

int RawStruct<std::vector<rpg::EventCommand> >::LcfSize(const std::vector<rpg::EventCommand>& event_commands, LcfWriter& stream) {
	int result = 0;
	int count = event_commands.size();
	for (int i = 0; i < count; i++)
		result += RawStruct<rpg::EventCommand>::LcfSize(event_commands[i], stream);
	result += 4;
	return result;
}

void RawStruct<std::vector<rpg::EventCommand> >::WriteXml(const std::vector<rpg::EventCommand>& event_commands, XmlWriter& stream) {
	std::vector<rpg::EventCommand>::const_iterator it;
	for (it = event_commands.begin(); it != event_commands.end(); it++)
		RawStruct<rpg::EventCommand>::WriteXml(*it, stream);
}

class EventCommandVectorXmlHandler : public XmlHandler {
public:
	EventCommandVectorXmlHandler(std::vector<rpg::EventCommand>& ref) : ref(ref) {}

	void StartElement(XmlReader& stream, const char* name, const char** /* atts */) {
		if (strcmp(name, "EventCommand") != 0)
			Log::Error("XML: Expecting %s but got %s", "EventCommand", name);
		ref.resize(ref.size() + 1);
		rpg::EventCommand& obj = ref.back();
		stream.SetHandler(new EventCommandXmlHandler(obj));
	}
private:
	std::vector<rpg::EventCommand>& ref;
};

void RawStruct<std::vector<rpg::EventCommand> >::BeginXml(std::vector<rpg::EventCommand>& obj, XmlReader& stream) {
	stream.SetHandler(new EventCommandVectorXmlHandler(obj));
}

} //namespace lcf
