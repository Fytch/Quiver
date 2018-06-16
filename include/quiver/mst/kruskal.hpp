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
			const auto v_end = graph.v_end();
			std::size_t v_index = 0;
			for(auto vert = graph.v_begin(); vert != v_end; ++vert, ++v_index) {
				auto const& out_edges = vert->out_edges;
				const auto e_end = out_edges.end();
				for(auto edge = out_edges.begin(); edge != e_end; ++edge)
					if(v_index < edge->to) // we only need half of the actual edges and no loops
						edges.emplace_back(v_index, edge->to, &*edge);
			}
		}
		std::sort(edges.begin(), edges.end(), [](edge_t const& lhs, edge_t const& rhs){ return lhs.ptr->weight < rhs.ptr->weight; });

		graph_t mst = graph.vertices();
		for(edge_t const& e : edges)
		{
			if(!cc.same_set(e.from, e.to))
			{
				mst.add_edge(e.from, e.to, e.ptr->properties());
				cc.unite(e.from, e.to);
			}
		}
		return mst;
	}
	template<typename graph_t>
	std::enable_if_t<!is_weighted_v<graph_t>, graph_t> kruskal(graph_t const& graph)
	{
		static_assert(is_undirected_v<graph_t>, "kruskal operates on undirected graphs");

		graph_t mst = graph.vertices();
		disjoint_set<> cc(graph.V());
		const auto v_end = graph.v_end();
		std::size_t v_index = 0;
		for(auto vert = graph.v_begin(); vert != v_end; ++vert, ++v_index) {
			auto const& out_edges = vert->out_edges;
			const auto e_end = out_edges.end();
			for(auto edge = out_edges.begin(); edge != e_end; ++edge)
				if(v_index < edge->to) // we only need half of the actual edges and no loops
					if(!cc.same_set(v_index, edge->to))
					{
						mst.add_edge(v_index, edge->to, edge->properties());
						cc.unite(v_index, edge->to);
					}
		}
		return mst;
	}
}

#endif // !QUIVER_MST_KRUSKAL_HPP_INCLUDED
