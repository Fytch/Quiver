/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_FAMILIES_CYCLE_HPP_INCLUDED
#define QUIVER_FAMILIES_CYCLE_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
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
	cycle(std::size_t n)
	{
		assert(n >= 3);
		adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> result(n);
		for(auto iter = result.v_begin(); iter != result.v_end(); ++iter)
			iter->out_edges.reserve(2);
		for(vertex_index_t i = 0; i < n - 1; ++i)
			result.add_edge(i, i + 1);
		result.add_edge(0, n - 1); // so we can spare us the modulo
		return result;
	}
	inline auto cycle(std::size_t n)
	{
		return cycle<>(n);
	}

	/*
	template<typename graph_t>
	std::enable_if_t<is_directed_v<graph_t>, bool> is_cycle(graph_t const& graph)
	{
		// TODO: implement
	}
	*/
	template<typename graph_t>
	std::enable_if_t<is_undirected_v<graph_t>, bool> is_cycle(graph_t const& graph)
	{
		if(graph.E() != graph.V())
			return false;
		return std::all_of(graph.v_begin(), graph.v_end(), [](auto const& v){ return v.out_edges() == 2; });
	}
}

#endif // !QUIVER_FAMILIES_CYCLE_HPP_INCLUDED
