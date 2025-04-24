#include "lcf/dbarrayalloc.h"
#include "lcf/dbarray.h"
#include "lcf/dbstring.h"
#include <cassert>
#include <cstddef>

//#define LCF_DEBUG_DBARRAY

#ifdef LCF_DEBUG_DBARRAY
#include <iostream>
#endif

namespace lcf {

const DBArrayAlloc::size_type DBArrayAlloc::_empty_buf[2] = { 0, 0 };
constexpr DBString::size_type DBString::npos;

static ptrdiff_t HeaderSize(size_t align) {
	return std::max(sizeof(DBArrayAlloc::size_type), align);
}

static size_t AllocSize(size_t size, size_t align) {
	return HeaderSize(align) + size;
}

static void* Adjust(void* p, ptrdiff_t off) {
	return reinterpret_cast<void*>(reinterpret_cast<intptr_t>(p) + off);
}

void* DBArrayAlloc::alloc(size_type size, size_type field_size, size_type align) {
	if (field_size == 0) {
		return empty_buf();
	}
	assert(align <= alignof(std::max_align_t));
	auto* raw = ::operator new(AllocSize(size, align));
	auto* p = Adjust(raw, HeaderSize(align));
	*get_size_ptr(p) = field_size;
#ifdef LCF_DEBUG_DBARRAY
	std::cout << "DBArray: Allocated"
		<< " size=" << size
		<< " field_size=" << *get_size_ptr(p)
		<< " align=" << align
		<< " ptr=" << raw
		<< " adjusted=" << p
		<< std::endl;
#endif
	return p;
}

void DBArrayAlloc::free(void* p, size_type align) noexcept {
	assert(p != nullptr);
	if (p != empty_buf()) {
		auto* raw = Adjust(p, -HeaderSize(align));
#ifdef LCF_DEBUG_DBARRAY
		std::cout << "DBArray: Free"
			<< " align=" << align
			<< " ptr=" << raw
			<< " adjusted=" << p
			<< " field_size=" << *get_size_ptr(p)
			<< std::endl;
#endif
		::operator delete(raw);
	}
}

char* DBString::construct_z(const char* s, size_t len) {
	auto* p = alloc(len);
	if (len) {
		std::memcpy(p, s, len + 1);
	}
	return p;
}

char* DBString::construct_sv(const char* s, size_t len) {
	auto* p = alloc(len);
	if (len) {
		std::memcpy(p, s, len);
		p[len] = '\0';
	}
	return p;
}

DBString& DBString::operator=(const DBString& o) {
	if (this != &o) {
		destroy();
		_storage = construct_z(o.data(), o.size());
	}
	return *this;
}

} // namespace lcf
