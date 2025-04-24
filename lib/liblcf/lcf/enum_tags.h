/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_ENUM_TAGS_H
#define LCF_ENUM_TAGS_H

#include <type_traits>
#include <iterator>
#include <cstddef>
#include <array>
#include <cstring>
#include <tuple>

namespace lcf {

template <typename E, size_t N>
class EnumTags {
	public:
		static constexpr size_t num_tags = N;
		using int_type = typename std::underlying_type<E>::type;

		struct EnumItem {
			int_type value;
			const char* name;

			constexpr EnumItem() noexcept : value(), name() {}
			constexpr EnumItem(int_type value, const char* name) noexcept : value(value), name(name) {}
		};

		using iterator = EnumItem const *;
		using reverse_iterator = std::reverse_iterator<iterator>;

		template<typename... Args>
		explicit constexpr EnumTags(Args const&...args) noexcept {
			using FirstElemT = std::tuple_element_t<0, std::tuple<Args...>>;

			if constexpr (std::is_array_v<FirstElemT>) {
				// Passed in via makeEnumTags
				AddMonotonicTag<0>(args...);
				monotonic_from_zero = true;
				return;
			} else {
				// Passed in as {A, "A", B, "B", ...} (for non-monotonic enums)
				AddTag<0>(args...);
				int_type i = 0;
				for (const auto& it : _tags) {
					if (it.value != i) {
						monotonic_from_zero = false;
					}
					++i;
				}
			}
		}

		constexpr const char* tag(int_type value) const;
		constexpr const char* tag(E etag) const;

		constexpr const char* operator[](E etag) const;
		constexpr const char* operator[](int_type value) const;

		bool has_etag(const char* tag) const;
		bool etag(const char* tag, E& result) const;
		E etagOr(const char* tag, E other) const;

		const std::array<EnumItem, num_tags>& tags() const { return _tags; }

		constexpr bool is_monotonic_from_zero() const { return monotonic_from_zero; }

		constexpr iterator begin() const { return iterator(_tags.data()); }
		constexpr iterator end() const { return iterator(_tags.data() + size()); }

		constexpr iterator cbegin() const { return begin(); }
		constexpr iterator cend() const { return end(); }

		reverse_iterator rbegin() const { return reverse_iterator(end()); }
		reverse_iterator rend() const { return reverse_iterator(begin()); }

		constexpr size_t size() const { return num_tags; }

	private:
		template<std::size_t I, typename... Next>
		constexpr void AddTag(E value, const char* name, Next const&...next) noexcept {
			_tags[I] = {int_type(value), name};

			if constexpr (sizeof...(Next) > 0) {
				AddTag<I + 1>(next...);
			}
		}

		template<std::size_t I, typename... Next>
		constexpr void AddMonotonicTag(const char* name, Next const&...next) noexcept {
			_tags[I] = {I, name};

			if constexpr (sizeof...(Next) > 0) {
				AddMonotonicTag<I + 1>(next...);
			}
		}

		std::array<EnumItem, num_tags> _tags;
		bool monotonic_from_zero = true;
};

template<typename ValueType, typename... Next>
EnumTags(ValueType const&, const char*, Next const&...) -> EnumTags<ValueType, (sizeof...(Next) + 2) / 2>;

template <typename E, size_t... N>
constexpr EnumTags<E,sizeof...(N)> makeEnumTags(const char (&...literals)[N]) {
	return EnumTags<E,sizeof...(N)>(literals...);
}

template <typename E, size_t N>
inline constexpr const char* EnumTags<E, N>::tag(E etag) const {
	return tag(int_type(etag));
}

template <typename E, size_t N>
inline constexpr const char* EnumTags<E, N>::tag(int_type value) const {
	if (monotonic_from_zero) {
		if (value < 0 || value >= N) {
			return nullptr;
		} else {
			return _tags[value].name;
		}
	}

	for (const auto& it: _tags) {
		if (it.value == value) {
			return it.name;
		}
	}

	return nullptr;
}

template <typename E, size_t N>
inline constexpr const char* EnumTags<E, N>::operator[](E etag) const {
	return tag(etag);
}

template <typename E, size_t N>
inline constexpr const char* EnumTags<E, N>::operator[](int_type value) const {
	return tag(value);
}

template <typename E, size_t N>
inline bool EnumTags<E, N>::has_etag(const char* tag) const {
	for (size_t i = 0; i < _tags.size(); ++i) {
		if (std::strcmp(_tags[i].name, tag) == 0) {
			return true;
		}
	}

	return false;
}

template <typename E, size_t N>
inline bool EnumTags<E, N>::etag(const char* tag, E& result) const {
	for (size_t i = 0; i < _tags.size(); ++i) {
		if (std::strcmp(_tags[i].name, tag) == 0) {
			result = E(_tags[i].value);
			return true;
		}
	}

	return false;
}

template <typename E, size_t N>
inline E EnumTags<E, N>::etagOr(const char* tag, E other) const {
	for (size_t i = 0; i < _tags.size(); ++i) {
		if (std::strcmp(_tags[i].name, tag) == 0) {
			return E(_tags[i].value);
		}
	}

	return other;
}

} //namespace lcf

#endif
