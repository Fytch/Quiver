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

	// TODO: uniform interface for different container types
}

#endif // !QUIVER_CONTAINER_INTERFACE_HPP_INCLUDED
