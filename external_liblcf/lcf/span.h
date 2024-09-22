/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_SPAN_H
#define LCF_SPAN_H
#include <cstring>
#include <string>
#include <cassert>
#include <iterator>
#include <ostream>
#include <algorithm>

#define span_CONFIG_NO_EXCEPTIONS 1
#define span_FEATURE_WITH_CONTAINER 1
#define span_FEATURE_CONSTRUCTION_FROM_STDARRAY_ELEMENT_TYPE 1
#define span_FEATURE_MAKE_SPAN 1
#define span_CONFIG_SLECT_SPAN span_SPAN_NONSTD
#include <lcf/third_party/span.h>

namespace lcf {

using ExtentT = nonstd::span_lite::extent_t;
using nonstd::dynamic_extent;

template <typename T, ExtentT Extent= dynamic_extent>
	using Span = nonstd::span<T,Extent>;

template <typename... Args>
constexpr inline auto MakeSpan(Args&&... args) noexcept -> decltype(nonstd::make_span(std::forward<Args>(args)...)) {
	return nonstd::make_span(std::forward<Args>(args)...);
}


} // namespace lcf

#endif
