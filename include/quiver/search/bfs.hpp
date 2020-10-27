/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_SEARCH_BFS_HPP_INCLUDED
#define QUIVER_SEARCH_BFS_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <quiver/util.hpp>
#include <vector>
#include <queue>
#include <cassert>

namespace quiver
{
	// visitor shall have the signature bool(vertex_index_t)
	// if the visitor returns true, exit the bfs and return true
	// returns false otherwise
	template<typename graph_t, typename visitor_t>
	bool bfs(graph_t& graph, vertex_index_t start, visitor_t visitor)
	{
		using vertex_t = copy_const<graph_t, typename graph_t::vertex_t>;
		using out_edge_t = typename graph_t::out_edge_t;

		assert(start < graph.V().size());

		std::vector<bool> enqueued(graph.V().size(), false);
		enqueued[start] = true;
		std::queue<vertex_index_t> neighbors;
		neighbors.push(start);

		do {
			vertex_index_t next = neighbors.front();
			assert(enqueued[next]);
			vertex_t& vertex = graph.V()[next];

			// if(visitor(vertex))
			if(visitor(next))
				return true;

			neighbors.pop();
			for(out_edge_t const& edge : vertex.out_edges)
				if(!enqueued[edge.to]) {
					neighbors.push(edge.to);
					enqueued[edge.to] = true;
				}
		} while(!neighbors.empty());
		return false;
	}
}

#endif // !QUIVER_SEARCH_BFS_HPP_INCLUDED
