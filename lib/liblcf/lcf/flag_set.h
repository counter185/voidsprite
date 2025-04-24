/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_FLAG_SET_H
#define LCF_FLAG_SET_H

#include <bitset>

namespace lcf {

template <typename E, size_t N=32>
class FlagSet {
	public:
		using underlying = std::bitset<N>;
		using reference = typename underlying::reference;

		constexpr FlagSet() = default;
		FlagSet(std::initializer_list<E> ilist);

		constexpr bool operator[](E tag) const { return _bits[size_t(tag)]; }
		reference operator[](E tag) { return _bits[size_t(tag)]; }

		size_t count() const { return _bits.count(); }
		bool any() const noexcept { return _bits.any(); }
		bool none() const noexcept { return _bits.none(); }

		size_t size() const { return _bits.size(); }

		FlagSet operator~() const noexcept { FlagSet o; o._bits = ~_bits; return o; }
		FlagSet& operator&=(const FlagSet& o) noexcept { _bits &= o._bits; return *this; }
		FlagSet& operator|=(const FlagSet& o) noexcept { _bits |= o._bits; return *this; }
		FlagSet& operator^=(const FlagSet& o) noexcept { _bits ^= o._bits; return *this; }

		template <typename EE, size_t NN>
		friend bool operator==(const FlagSet<EE,NN>& l, const FlagSet<EE,NN>& r);

		friend struct std::hash<FlagSet<E,N>>;
	private:
		std::bitset<N> _bits = {};
};

template <typename E, size_t N>
inline bool operator==(const FlagSet<E,N>& l, const FlagSet<E,N>& r) {
	return l._bits == r._bits;
}

template <typename E, size_t N>
inline bool operator!=(const FlagSet<E,N>& l, const FlagSet<E,N>& r) {
	return !(l == r);
}

template <typename E, size_t N>
inline FlagSet<E,N> operator&(const FlagSet<E,N>& l, const FlagSet<E,N>& r) {
	auto copy = l;
	copy &= r;
	return copy;
}

template <typename E, size_t N>
inline FlagSet<E,N> operator|(const FlagSet<E,N>& l, const FlagSet<E,N>& r) {
	auto copy = l;
	copy |= r;
	return copy;
}

template <typename E, size_t N>
inline FlagSet<E,N> operator^(const FlagSet<E,N>& l, const FlagSet<E,N>& r) {
	auto copy = l;
	copy ^= r;
	return copy;
}

template <typename E, size_t N>
inline FlagSet<E,N>::FlagSet(std::initializer_list<E> ilist) {
	for (auto&& tag: ilist) {
		_bits.set(static_cast<size_t>(tag));
	}
}

} //namespace lcf

namespace std {
template <typename E, size_t N>
	struct hash<lcf::FlagSet<E,N>> {
		size_t operator()(const lcf::FlagSet<E,N>& fs) {
			return hash<typename lcf::FlagSet<E,N>::underlying>()(fs._bits);
		}
	};
}

#endif
