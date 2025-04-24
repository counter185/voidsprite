/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "lcf/encoder.h"
#include "lcf/reader_util.h"
#include "lcf/scope_guard.h"
#include "log.h"
#include <cstdio>
#include <cstdlib>

#if LCF_SUPPORT_ICU
#   include <unicode/ucsdet.h>
#   include <unicode/ucnv.h>
#else
#   include <cstdint>
#endif

#ifdef _WIN32
#   include <windows.h>
#else
#   include <locale>
#endif

namespace lcf {

static std::string filterUtf8Compatible(std::string enc) {
#if LCF_SUPPORT_ICU
	if (ucnv_compareNames(enc.c_str(), "UTF-8") == 0) {
		return "";
	}
#endif

	if (enc == "utf-8" || enc == "UTF-8" || enc == "65001") {
		return "";
	}

	return enc;
}

Encoder::Encoder(std::string encoding)
	: _encoding(filterUtf8Compatible(std::move(encoding)))
{
	Init();
}

Encoder::~Encoder() {
	Reset();
}

bool Encoder::IsOk() const {
	return _encoding.empty() || (_conv_storage && _conv_runtime);
}

void Encoder::Encode(std::string& str) {
	if (_encoding.empty() || str.empty()) {
		return;
	}
	Convert(str, _conv_runtime, _conv_storage);
}

void Encoder::Decode(std::string& str) {
	if (_encoding.empty() || str.empty()) {
		return;
	}
	Convert(str, _conv_storage, _conv_runtime);
}

void Encoder::Init() {
	if (_encoding.empty()) {
		return;
	}

	auto code_page = atoi(_encoding.c_str());
	const auto& storage_encoding = code_page > 0
		? ReaderUtil::CodepageToEncoding(code_page)
		: _encoding;

#if LCF_SUPPORT_ICU
	auto status = U_ZERO_ERROR;
	constexpr auto runtime_encoding = "UTF-8";
	auto conv_runtime = ucnv_open(runtime_encoding, &status);

	if (conv_runtime == nullptr) {
		Log::Error("ucnv_open() error for encoding \"%s\": %s", runtime_encoding, u_errorName(status));
		return;
	}
	status = U_ZERO_ERROR;
	auto sg = makeScopeGuard([&]() { ucnv_close(conv_runtime); });

	auto conv_storage = ucnv_open(storage_encoding.c_str(), &status);

	if (conv_storage == nullptr) {
		Log::Error("ucnv_open() error for dest encoding \"%s\": %s", storage_encoding.c_str(), u_errorName(status));
		return;
	}

	sg.Dismiss();

	_conv_runtime = conv_runtime;
	_conv_storage = conv_storage;
#else
	if (storage_encoding != "windows-1252") {
		return;
	}

	_conv_runtime = 65001;
	_conv_storage = 1252;
#endif
}

#if LCF_SUPPORT_ICU
void Encoder::Reset() {
	if (_conv_runtime) {
		ucnv_close(_conv_runtime);
		_conv_runtime = nullptr;
	}

	if (_conv_storage) {
		ucnv_close(_conv_storage);
		_conv_storage = nullptr;
	}
}

void Encoder::Convert(std::string& str, UConverter* conv_dst, UConverter* conv_src) {
	const auto& src = str;

	auto status = U_ZERO_ERROR;
	_buffer.resize(src.size() * 4);

	const auto* src_p = src.c_str();
	auto* dst_p = _buffer.data();

	ucnv_convertEx(conv_dst, conv_src,
			&dst_p, dst_p + _buffer.size(),
			&src_p, src_p + src.size(),
			nullptr, nullptr, nullptr, nullptr,
			true, true,
			&status);

	if (U_FAILURE(status)) {
		Log::Error("ucnv_convertEx() error when encoding \"%s\": %s", src.c_str(), u_errorName(status));
		_buffer.clear();
	}

	str.assign(_buffer.data(), dst_p);
}
#else
void Encoder::Convert(std::string& str, int conv_dst, int) {
	if (str.empty()) {
		return;
	}

	size_t buf_idx = 0;

	if (conv_dst == 65001) {
		// From 1252 to UTF-8
		// Based on https://stackoverflow.com/q/4059775/
		_buffer.resize(str.size() * 2 + 1);

		for (unsigned char ch: str) {
			if (ch < 0x80) {
				_buffer[buf_idx] = static_cast<char>(ch);
			} else {
				_buffer[buf_idx] = static_cast<char>(0xC0 | (ch >> 6));
				++buf_idx;
				_buffer[buf_idx] = static_cast<char>(0x80 | (ch & 0x3F));
			}

			++buf_idx;
		}
	} else {
		// From UTF-8 to 1252
		// Based on https://stackoverflow.com/q/23689733/
		_buffer.resize(str.size() + 1);
		uint32_t codepoint;

		for (size_t str_idx = 0; str_idx < str.size(); ++str_idx) {
			unsigned char ch = str[str_idx];
			if (ch <= 0x7F) {
				codepoint = ch;
			} else if (ch <= 0xBF) {
				codepoint = (codepoint << 6) | (ch & 0x3F);
			} else if (ch <= 0xDF) {
				codepoint = ch & 0x1F;
			} else if (ch <= 0xEF) {
				codepoint = ch & 0x0F;
			} else {
				codepoint = ch & 0x07;
			}
			++str_idx;
			ch = str[str_idx];
			if (((ch & 0xC0) != 0x80) && (codepoint <= 0x10ffff)) {
				if (codepoint <= 255) {
					_buffer[buf_idx] = static_cast<char>(codepoint);
				} else {
					_buffer[buf_idx] = '?';
				}
			}
			++buf_idx;
		}
	}

	str.assign(_buffer.data(), buf_idx);
}
#endif

} //namespace lcf
