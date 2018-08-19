/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_TYPEDEFS_HPP_INCLUDED
#define QUIVER_TYPEDEFS_HPP_INCLUDED

#include <cstddef>
#include <type_traits>

namespace quiver
{
	using vertex_index_t = std::size_t;

	enum directivity_t
	{
		directed,
		undirected
	};

	template<typename graph_t>
	struct is_directed : public std::integral_constant<bool, graph_t::directivity == directed>
	{
	};
	template<typename graph_t>
	inline constexpr bool is_directed_v = is_directed<graph_t>::value;

	template<typename graph_t>
	struct is_undirected : public std::integral_constant<bool, graph_t::directivity == undirected>
	{
	};
	template<typename graph_t>
	inline constexpr bool is_undirected_v = is_undirected<graph_t>::value;
}

#endif // !QUIVER_TYPEDEFS_HPP_INCLUDED
