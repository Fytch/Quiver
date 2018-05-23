/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_UTIL_HPP_INCLUDED
#define QUIVER_UTIL_HPP_INCLUDED

#include <type_traits>
#include <utility>

namespace quiver
{
	struct empty_class
	{
	};

	template<typename T>
	using void2empty = std::conditional_t<std::is_same_v<T, void>, empty_class, T>;

	template<typename T>
	constexpr T sq(T const& x)
	{
		return x * x;
	}
}

#endif // !QUIVER_UTIL_HPP_INCLUDED
