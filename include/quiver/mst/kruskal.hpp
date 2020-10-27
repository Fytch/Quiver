/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_MST_KRUSKAL_HPP_INCLUDED
#define QUIVER_MST_KRUSKAL_HPP_INCLUDED

// #include <quiver/adjacency_list.hpp>
#include <quiver/disjoint_set.hpp>
#include <vector>
#include <algorithm>
#include <type_traits>

namespace quiver
{
	template<typename graph_t>
	std::enable_if_t<is_weighted_v<graph_t>, graph_t> kruskal(graph_t const& graph)
	{
		static_assert(is_undirected_v<graph_t>, "kruskal operates on undirected graphs");

		disjoint_set<> cc(graph.V());
		struct edge_t
		{
			vertex_index_t from, to;
			typename graph_t::out_edge_t const* ptr;

			constexpr edge_t(decltype(from) from, decltype(to) to, decltype(ptr) ptr)
			: from(std::move(from)), to(std::move(to)), ptr(std::move(ptr))
			{
			}
		};
		std::vector<edge_t> edges;
		{
			const std::size_t E = graph.E();
			edges.reserve(E);
			vertex_index_t vert_index = 0;
			for(auto const& vert : graph) {
				for(auto const& out_edge : vert.out_edges)
					if(vert_index < out_edge.to) // we only need half of the actual edges and no loops
						edges.emplace_back(vert_index, out_edge.to, &out_edge);
				++vert_index;
			}
		}
		std::sort(edges.begin(), edges.end(), [](edge_t const& lhs, edge_t const& rhs){ return lhs.ptr->weight < rhs.ptr->weight; });

		graph_t mst = graph.strip_edges();
		for(edge_t const& e : edges)
		{
			if(cc.unite(e.from, e.to))
				mst.add_edge(e.from, e.to, e.ptr->properties());
		}
		return mst;
	}
	template<typename graph_t>
	std::enable_if_t<!is_weighted_v<graph_t>, graph_t> kruskal(graph_t const& graph)
	{
		static_assert(is_undirected_v<graph_t>, "kruskal operates on undirected graphs");

		graph_t mst = graph.strip_edges();
		disjoint_set<> cc(graph.V());
		vertex_index_t vert_index = 0;
		for(auto const& vert : graph) {
			for(auto const& out_edge : vert.out_edges)
				if(vert_index < out_edge.to) // we only need half of the actual edges and no loops
					if(cc.unite(vert_index, out_edge.to))
						mst.add_edge(vert_index, out_edge.to, out_edge.properties());
			++vert_index;
		}
		return mst;
	}
}

#endif // !QUIVER_MST_KRUSKAL_HPP_INCLUDED
