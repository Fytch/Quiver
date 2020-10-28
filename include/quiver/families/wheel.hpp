/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_FAMILIES_WHEEL_HPP_INCLUDED
#define QUIVER_FAMILIES_WHEEL_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <quiver/regular.hpp>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <algorithm>

namespace quiver
{
	template<
		typename edge_properties_t = void,
		typename vertex_properties_t = void,
		template<typename> class out_edge_container = vector,
		template<typename> class vertex_container = vector
	>
	adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>
	wheel(std::size_t n)
	{
		assert(n >= 4);
		adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> result(n);
		result.V[0].out_edges.reserve(n - 1);
		for(auto iter = std::next(result.V.begin()); iter != result.V.end(); ++iter)
			iter->out_edges.reserve(3);
		// so we can spare us the modulo:
		result.E.emplace(0, n - 1);
		result.E.emplace(1, n - 1);
		for(vertex_index_t i = 1; i < n - 1; ++i) {
			result.E.emplace(0, i);
			result.E.emplace(i, i + 1);
		}
		return result;
	}
	inline auto wheel(std::size_t n)
	{
		return wheel<>(n);
	}
}

#endif // !QUIVER_FAMILIES_WHEEL_HPP_INCLUDED
