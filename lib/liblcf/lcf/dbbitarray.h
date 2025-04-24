/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_DBBITARRAY_H
#define LCF_DBBITARRAY_H
#include <utility>
#include <cstring>
#include <iterator>
#include <cstdint>
#include <climits>
#include <limits>
#include <algorithm>
#include <type_traits>

#include "lcf/dbarrayalloc.h"

namespace lcf {

class DBBitProxy {
	public:
		using size_type = DBArrayAlloc::size_type;
		using ssize_type = DBArrayAlloc::ssize_type;

		constexpr DBBitProxy() = default;

		DBBitProxy& operator=(const DBBitProxy& x) noexcept = default;
		DBBitProxy& operator=(bool x) noexcept {
			auto& byte = _base[_idx / CHAR_BIT];
			auto bit = _idx % CHAR_BIT;
			byte ^= (size_type(-ssize_type(x)) ^ byte) & (size_type(1) << bit);
			return *this;
		}

		operator bool() const noexcept {
			auto byte = _base[_idx / CHAR_BIT];
			auto bit = _idx % CHAR_BIT;
			return byte & (1 << bit);
		}

		void flip() {
			auto& byte = _base[_idx / CHAR_BIT];
			auto bit = _idx % CHAR_BIT;
			byte ^= (1 << bit);
		}

		size_type index() const {
			return _idx;
		}

	private:
		uint8_t* _base = nullptr;
		size_type _idx = 0;

		constexpr DBBitProxy(void* base, size_type idx) : _base(static_cast<uint8_t*>(base)), _idx(idx) {}

		template <typename P>
		friend class DBBitProxyIterator;
		friend class DBBitArray;
};

template <typename ProxyType>
class DBBitProxyIterator {
	public:
		using size_type = typename DBBitProxy::size_type;
		using ssize_type = DBArrayAlloc::ssize_type;

		using difference_type = ssize_type;
		using value_type = ProxyType;
		using pointer = ProxyType*;
		using reference = ProxyType&;
		using iterator_category = std::bidirectional_iterator_tag;

		constexpr DBBitProxyIterator() = default;

		operator DBBitProxyIterator<const ProxyType>() const {
			return DBBitProxyIterator<const ProxyType>(_proxy._base, _proxy._idx);
		}

		ProxyType& operator*() const { return _proxy; }
		ProxyType* operator->() const { return &_proxy; }

		DBBitProxyIterator& operator++() { ++_proxy._idx; return *this; }
		DBBitProxyIterator operator++(int) { auto iter = *this; ++(*this); return iter; }

		DBBitProxyIterator& operator--() { --_proxy._idx; return *this; }
		DBBitProxyIterator operator--(int) { auto iter = *this; --(*this); return iter; }

		void swap(DBBitProxyIterator& o) {
			std::swap(_proxy._base, o._base);
			std::swap(_proxy._idx, o._idx);
		}

		friend bool operator==(DBBitProxyIterator l, DBBitProxyIterator r) { return l->index() == r->index(); }
		friend bool operator!=(DBBitProxyIterator l, DBBitProxyIterator r) { return !(l == r); }

	private:
		mutable DBBitProxy _proxy;

		constexpr DBBitProxyIterator(void* base, size_type idx) : _proxy(base, idx) {}

		friend class DBBitArray;
};


// An array data structure optimized for database storage.
// Low memory footprint and not dynamically resizable.
class DBBitArray {
	public:
		using value_type = bool;
		using size_type = DBArrayAlloc::size_type;
		using ssize_type = typename std::make_signed<size_type>::type;

		using iterator = DBBitProxyIterator<DBBitProxy>;
		using const_iterator = DBBitProxyIterator<const DBBitProxy>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		constexpr DBBitArray() = default;
		explicit DBBitArray(size_type count, bool value = false) : _storage(this->alloc(count)) {
			std::memset(_storage, value ? 0xff : 0x00, bytes_up_from_bits(count));
		}

		template <typename Iter,
				 typename std::enable_if<
					 std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>::value, int>::type = 0
				 >
		DBBitArray(Iter first, Iter last) : _storage(this->alloc(static_cast<size_type>(std::distance(first, last)))) {
			std::copy(first, last, begin());
		}

		DBBitArray(std::initializer_list<bool> ilist)
			: DBBitArray(ilist.begin(), ilist.end()) {}

		DBBitArray(const DBBitArray& o) : _storage(this->alloc(o.size())) {
			std::memcpy(_storage, o._storage, bytes_up_from_bits(o.size()));
		}
		DBBitArray(DBBitArray&& o) noexcept { swap(o); }

		DBBitArray& operator=(const DBBitArray& o);
		DBBitArray& operator=(DBBitArray&& o) noexcept;

		void swap(DBBitArray& o) noexcept {
			std::swap(_storage, o._storage);
		}

		~DBBitArray() { destroy(); }

		DBBitProxy operator[](size_type i) { return DBBitProxy(_storage, i); }
		bool operator[](size_type i) const { return DBBitProxy(_storage, i); }

		DBBitProxy front() { return (*this)[0]; }
		bool front() const { return (*this)[0]; }

		DBBitProxy back() { return (*this)[size()-1]; }
		bool back() const { return (*this)[size()-1]; }

		iterator begin() { return iterator(_storage, 0); }
		iterator end() { return iterator(_storage, size()); }

		const_iterator begin() const { return const_iterator(_storage, 0); }
		const_iterator end() const { return const_iterator(_storage, size()); }

		const_iterator cbegin() const { return begin(); }
		const_iterator cend() const { return end(); }

		reverse_iterator rbegin() { return reverse_iterator(end()); }
		reverse_iterator rend() { return reverse_iterator(begin()); }

		const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
		const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

		const_reverse_iterator crbegin() const { return rbegin(); }
		const_reverse_iterator crend() const { return rend(); }

		bool empty() const { return size() == 0; }
		size_type size() const { return *DBArrayAlloc::get_size_ptr(_storage); }

		void set_all() { std::memset(_storage, 0xff, bytes_up_from_bits(size())); }
		void reset_all() { std::memset(_storage, 0, bytes_up_from_bits(size())); }
		void flip_all() {
			auto* p = static_cast<uint8_t*>(_storage);
			for (size_t i = 0; i < bytes_up_from_bits(size()); ++i) {
				p[i] ^= p[i];
			}
		}

		void set(size_type i) { (*this)[i] = true; }
		void reset(size_type i) { (*this)[i] = false; }
		void flip(size_type i) { (*this)[i].flip(); }

	private:
		static constexpr size_type bytes_up_from_bits(size_type bits) {
			return (bits / CHAR_BIT) + (bits % CHAR_BIT != 0);
		}

		void* alloc(size_type bits) {
			auto bytes = bytes_up_from_bits(bits);
			return DBArrayAlloc::alloc(bytes, bits, alignof(size_type));
		}

		void free(void* p) {
			DBArrayAlloc::free(p, alignof(size_type));
		}

		void destroy() noexcept;
	private:
		void* _storage = DBArrayAlloc::empty_buf();
};

inline bool operator==(const DBBitArray& l, const DBBitArray& r) { return std::equal(l.begin(), l.end(), r.begin(), r.end()); }
inline bool operator!=(const DBBitArray& l, const DBBitArray& r) { return !(l == r); }
inline bool operator<(const DBBitArray& l, const DBBitArray& r) { return std::lexicographical_compare(l.begin(), l.end(), r.begin(), r.end()); }
inline bool operator>(const DBBitArray& l, const DBBitArray& r) { return r < l; }
inline bool operator<=(const DBBitArray& l, const DBBitArray& r) { return !(l > r); }
inline bool operator>=(const DBBitArray& l, const DBBitArray& r) { return !(l < r); }

inline DBBitArray& DBBitArray::operator=(const DBBitArray& o) {
	if (this != &o) {
		destroy();
		_storage = this->alloc(o.size());
		std::memcpy(_storage, o._storage, bytes_up_from_bits(o.size()));
	}
	return *this;
}

inline DBBitArray& DBBitArray::operator=(DBBitArray&& o) noexcept {
	if (this != &o) {
		destroy();
		swap(o);
	}
	return *this;
}

inline void DBBitArray::destroy() noexcept {
	if (_storage != DBArrayAlloc::empty_buf()) {
		free(_storage);
		_storage = DBArrayAlloc::empty_buf();
	}
}

} // namespace lcf

#endif
