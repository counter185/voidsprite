/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_CONTEXT_H
#define LCF_CONTEXT_H

#include "string_view.h"

namespace lcf {

/**
 * Context information for iteraing lcf fields
 */
struct ContextNameBase {
	/** Constructor */
	constexpr ContextNameBase(StringView n, int i) : name(n), index(i) {}

	/** Name of the enumerated field */
	StringView name;

	/** Array index when the object is part of a list (-1 when not) */
	int index = -1;
};

/**
 * Context information for iteraing lcf fields
 * @tparam StructType The type of the Struct
 */
template <typename StructType>
struct ContextStructBase : ContextNameBase {
	/** The type of the struct */
	using StructType_t = StructType;

	/** Constructor */
	constexpr ContextStructBase(StringView n, int i, StructType_t* o)
		: ContextNameBase(n, i), obj(o) {}

	/** Object instance (cast to appropriate RPG-type */
	StructType_t* obj = nullptr;
};

/**
 * Context information for iteraing lcf fields
 * @tparam StructType The type of the Struct
 * @tparam ParentCtx The type of the parent context.
 */
template <typename StructType, typename ParentCtxType>
struct Context : ContextStructBase<StructType> {
	/** The type of the struct */
	using StructType_t = typename ContextStructBase<StructType>::StructType_t;

	/** The type of the parent context */
	using ParentCtxType_t = ParentCtxType;

	constexpr Context(StringView n, int i, StructType* o, const ParentCtxType_t* pctx)
		: ContextStructBase<StructType>{n, i, o}, parent(pctx) {}

	/** Context of the parent (nullptr when no parent) */
	const ParentCtxType_t* parent = nullptr;
};

} //namespace lcf

#endif
