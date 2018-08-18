/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_TOPOSORT_HPP_INCLUDED
#define QUIVER_TOPOSORT_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <quiver/exception.hpp>
#include <vector>

namespace quiver
{
	// the iterator variant of toposort_kahn writes the sorting in correct order
	// time complexity:  O(E + V)
	// space complexity: O(V)
	template<typename graph_t, typename output_iterator_t>
	void toposort_kahn(graph_t const& graph, output_iterator_t iterator)
	{
		std::vector<std::size_t> in_deg(graph.V(), 0);
		for(auto const& vert : graph)
			for(auto const& out_edge : vert.out_edges)
				++in_deg[out_edge.to];
		std::vector<vertex_index_t> starts;
		for(vertex_index_t i = 0; i < graph.V(); ++i)
			if(in_deg[i] == 0)
				starts.emplace_back(i);
		std::size_t out_size = 0;
		while(!starts.empty()) {
			const vertex_index_t next = starts.back();
			*iterator++ = next;
			++out_size;
			starts.pop_back();
			for(auto const& out_edge : graph.vertex(next).out_edges)
				if(--in_deg[out_edge.to] == 0)
					starts.emplace_back(out_edge.to);
		}
		if(out_size != graph.V())
			throw cyclic_graph{ "toposort_kahn: graph contains cycles" };
	}
	template<typename graph_t>
	std::vector<vertex_index_t> toposort_kahn(graph_t const& graph)
	{
		std::vector<vertex_index_t> result;
		result.resize(graph.V());
		toposort_kahn(graph, result.begin());
		return result;
	}

	template<typename graph_t, typename output_iterator_t>
	void toposort(graph_t const& graph, output_iterator_t iterator)
	{
		return toposort_kahn(graph, iterator);
	}
	template<typename graph_t>
	std::vector<vertex_index_t> toposort(graph_t const& graph)
	{
		return toposort_kahn(graph);
	}
}

#endif // !QUIVER_TOPOSORT_HPP_INCLUDED
