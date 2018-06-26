/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_CONTAINER_INTERFACE_HPP_INCLUDED
#define QUIVER_CONTAINER_INTERFACE_HPP_INCLUDED

#include <vector>
#include <type_traits>
#include <iterator>
#include <algorithm>

namespace quiver
{
	template<typename T>
	using vector = std::vector<T>;

	template<typename T>
	inline constexpr bool is_sane_container =
		std::is_nothrow_constructible_v<T> &&
		std::is_nothrow_move_constructible_v<T> &&
		std::is_nothrow_move_assignable_v<T> &&
		std::is_nothrow_swappable_v<T>;

	namespace detail
	{
		template<typename container_t>
		struct is_ordered_helper
		{
		};
		template<typename T>
		struct is_ordered_helper<vector<T>> : std::true_type
		{
		};
	}
	template<typename container_t>
	inline constexpr bool is_ordered = detail::is_ordered_helper<container_t>::value;
	template<template<typename> typename container>
	inline constexpr bool is_ordered_templ = is_ordered<container<int>>;

	// TODO: uniform interface for different container types

	template<typename container_t, typename comparator_t>
	void sort(container_t& container, comparator_t comparator)
	{
		using std::begin, std::end;
		std::sort(begin(container), end(container), comparator);
	}
}

#endif // !QUIVER_CONTAINER_INTERFACE_HPP_INCLUDED
