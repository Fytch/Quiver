/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_FAMILIES_COMPLETE_HPP_INCLUDED
#define QUIVER_FAMILIES_COMPLETE_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <quiver/regular.hpp>
#include <cstddef>

namespace quiver
{
	template<
		typename edge_properties_t = void,
		typename vertex_properties_t = void,
		template<typename> class out_edge_container = vector,
		template<typename> class vertex_container = vector
	>
	adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>
	complete(std::size_t n)
	{
		adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> result(n);
		for(auto& vert : result)
			vert.out_edges.reserve(n - 1);
		for(std::size_t i = 1; i < n; ++i)
			for(std::size_t j = 0; j < i; ++j)
				result.add_edge(i, j);
		return result;
	}
	inline auto complete(std::size_t n)
	{
		return complete<>(n);
	}

	template<typename graph_t>
	bool is_complete(graph_t const& graph)
	{
		// return is_regular(graph, graph.V());
		return graph.E() == graph.max_edges();
	}
}

#endif // !QUIVER_FAMILIES_COMPLETE_HPP_INCLUDED
