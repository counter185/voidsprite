/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_DBARRAYALLOC_H
#define LCF_DBARRAYALLOC_H
#include <utility>
#include <string>
#include <iterator>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <type_traits>

namespace lcf {

struct DBArrayAlloc {
	using size_type = uint32_t;
	using ssize_type = int32_t;

	static void* alloc(size_type size, size_type field_size, size_type align);
	static void free(void* p, size_type align) noexcept;

	static void* empty_buf() {
		return const_cast<size_type*>(&_empty_buf[1]);
	}

	static constexpr size_type* get_size_ptr(void* p) {
		return static_cast<size_type*>(p) - 1;
	}

	static constexpr const size_type* get_size_ptr(const void* p) {
		return static_cast<const size_type*>(p) - 1;
	}

	private:
	static const size_type _empty_buf[2];
};

} // namespace lcf

#endif
