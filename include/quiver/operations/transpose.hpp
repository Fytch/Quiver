/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_OPERATIONS_TRANSPOSE_HPP_INCLUDED
#define QUIVER_OPERATIONS_TRANSPOSE_HPP_INCLUDED

#include <quiver/util.hpp>
#include <quiver/adjacency_list.hpp>
#include <type_traits>
#include <utility>

namespace quiver
{
	template<typename graph_t>
	constexpr std::enable_if_t<is_undirected_v<std::decay_t<graph_t>>, graph_t> transpose(graph_t&& graph) noexcept
	{
		return std::forward<graph_t>(graph);
	}
	template<typename graph_t>
	std::enable_if_t<is_directed_v<std::decay_t<graph_t>>, std::decay_t<graph_t>> transpose(graph_t&& graph)
	{
		std::decay_t<graph_t> result = graph.vertices();
		for(vertex_index_t v = 0; v < graph.V(); ++v)
			for(auto&& out_edge : graph.vertex(v).out_edges)
				result.add_edge(out_edge.to, v, out_edge.properties());
		return result;
	}
}

#endif // !QUIVER_OPERATIONS_TRANSPOSE_HPP_INCLUDED
