/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_STRING_VIEW_H
#define LCF_STRING_VIEW_H
#include <cstring>
#include <string>
#include <cassert>
#include <iterator>
#include <ostream>
#include <algorithm>

#define nssv_CONFIG_NO_EXCEPTIONS 1
#define nssv_CONFIG_CONVERSION_STD_STRING 1
#define nssv_CONFIG_SELECT_STRING_VIEW nssv_STRING_VIEW_NONSTD
#include <lcf/third_party/string_view.h>

namespace lcf {

template <typename CharT, typename Traits = std::char_traits<CharT>>
	using BasicStringView = nonstd::basic_string_view<CharT,Traits>;

using StringView = BasicStringView<char>;
using WStringView = BasicStringView<wchar_t>;
using U16StringView = BasicStringView<char16_t>;
using U32StringView = BasicStringView<char32_t>;

template< class CharT, class Traits, class Allocator = std::allocator<CharT> >
std::basic_string<CharT, Traits, Allocator>
ToString(BasicStringView<CharT,Traits> sv, const Allocator& a = Allocator()) {
	return nonstd::to_string(sv, a);
}

template< class CharT, class Traits, class Allocator >
BasicStringView<CharT, Traits>
ToStringView(const std::basic_string<CharT, Traits, Allocator>& s )
{
	return nonstd::to_string_view(s);
}

/** A reimplementation of std::atoi() which works for StringView */
inline int SvAtoi(StringView str) {
	const char* b = str.data();
	const char* e = str.data() + str.length();
	auto value = std::strtol(b, const_cast<char**>(&e), 10);
	return e > b ? value : 0;
}

} // namespace lcf

#endif
