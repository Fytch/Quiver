/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_FAMILIES_LINEAR_HPP_INCLUDED
#define QUIVER_FAMILIES_LINEAR_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace quiver
{
	template<
		typename edge_properties_t = void,
		typename vertex_properties_t = void,
		template<typename> class out_edge_container = vector,
		template<typename> class vertex_container = vector
	>
	adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>
	linear(std::size_t n)
	{
		adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> result(n);
		if(n > 1) {
			auto iter = result.V.begin();
			const auto last = std::prev(result.V.end());
			iter->out_edges.reserve(1);
			last->out_edges.reserve(1);
			if(n > 2)
				while(++iter != last)
					iter->out_edges.reserve(2);
		}
		if(n > 1)
			for(vertex_index_t i = 0; i < n - 1; ++i)
				result.add_edge(i, i + 1);
		return result;
	}
	inline auto linear(std::size_t n)
	{
		return linear<>(n);
	}

	/*
	template<typename graph_t>
	std::enable_if_t<is_directed_v<graph_t>, bool> is_linear(graph_t const& graph)
	{
		// TODO: implement
	}
	*/
	template<typename graph_t>
	std::enable_if_t<is_undirected_v<graph_t>, bool> is_linear(graph_t const& graph)
	{
		if(graph.empty())
			return true;
		if(graph.E() != graph.V.size() - 1)
			return false;
		std::size_t leaves = 0;
		for(auto iter = graph.V.begin(); iter != graph.V.end(); ++iter) {
			switch(iter->out_degree()) {
			case 1:
				if(++leaves > 2)
					return false;
				break;
			case 2:
				break;
			default:
				return false;
			}
		}
		return leaves == 2;
	}
}

#endif // !QUIVER_FAMILIES_LINEAR_HPP_INCLUDED
