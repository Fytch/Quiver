/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_SEARCH_DFS_HPP_INCLUDED
#define QUIVER_SEARCH_DFS_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <quiver/util.hpp>
#include <vector>
#include <stack>
#include <cassert>

namespace quiver
{
	// visitor shall have the signature bool(vertex_index_t).
	// If visitor returns true, exit the search and return true.
	// Returns false if no visitation returned true.
	template<typename graph_t, typename visitor_t>
	bool dfs(graph_t& graph, vertex_index_t start, visitor_t visitor)
	{
		using vertex_t = copy_const<graph_t, typename graph_t::vertex_t>;
		// using out_edge_t = typename graph_t::out_edge_t;

		assert(start < graph.V.size());

		std::vector<bool> enqueued(graph.V.size(), false);
		enqueued[start] = true;
		std::stack<vertex_index_t> neighbors;
		neighbors.push(start);

		do {
			vertex_index_t index = neighbors.top();
			assert(enqueued[index]);
			vertex_t& vertex = graph.V[index];

			if(visitor(index))
				return true;

			neighbors.pop();
			{
				// reverse here so that first inserted is first visited
				const auto begin = vertex.out_edges.rbegin();
				const auto end = vertex.out_edges.rend();
				for(auto iter = begin; iter != end; ++iter)
					if(!enqueued[iter->to]) {
						neighbors.push(iter->to);
						enqueued[iter->to] = true;
					}
			}
		} while(!neighbors.empty());
		return false;
	}
}

#endif // !QUIVER_SEARCH_DFS_HPP_INCLUDED
