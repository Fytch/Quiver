/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_CONNECTED_COMPONENTS_HPP_INCLUDED
#define QUIVER_CONNECTED_COMPONENTS_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <quiver/disjoint_set.hpp>
#include <cstddef>
#include <vector>
#include <unordered_map>

namespace quiver
{
	template<typename graph_t>
	disjoint_set<> get_disjoint_set(graph_t const& graph)
	{
		static_assert(is_undirected_v<graph_t>, "connected components exist in undirected graphs");

		disjoint_set<> cc(graph.V());
		vertex_index_t vert_index = 0;
		for(auto const& vert : graph) {
			for(auto const& out_edge : vert.out_edges)
				if(vert_index < out_edge.to) // we only need half of the actual edges
					cc.unite(vert_index, out_edge.to);
			++vert_index;
		}
		return cc;
	}

	template<
		directivity_t dir,
		typename edge_properties_t,
		typename vertex_properties_t,
		template<typename> class out_edge_container,
		template<typename> class vertex_container
	>
	std::vector<adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>>
	split_ccs(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph)
	{
		using graph_t = adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>;
		auto ds = get_disjoint_set(graph);					// vertex [0..V] -> root [0..V]

		std::unordered_map<vertex_index_t, vertex_index_t> compressed_cc_index; // disjoint set roots -> cc index [0..|CC|]
		{
			for(vertex_index_t v = 0; v < graph.V(); ++v) {
				vertex_index_t parent_v = ds.find(v);
				if(compressed_cc_index.count(parent_v) == 0)
					compressed_cc_index.emplace(parent_v, compressed_cc_index.size());
			}
		}

		std::vector<std::size_t> cc_relative(graph.V());	// vertex [0..V] -> index in cc [0..V-|CC|+1]
		{
			std::vector<std::size_t> counters(ds.sets(), 0);
			for(vertex_index_t v = 0; v < graph.V(); ++v)
				cc_relative[v] = counters[compressed_cc_index[ds.find(v)]]++;
		}

		std::vector<graph_t> result(ds.sets());
		for(vertex_index_t v = 0; v < graph.V(); ++v) {
			auto& cc = result[compressed_cc_index[ds.find(v)]];
			cc.reserve(ds.cardinality(v));
			auto index = cc.add_vertex(std::move(graph.vertex(v)));
			for(auto& out_edge : cc.vertex(index).out_edges)
				out_edge.to = cc_relative[out_edge.to];
		}
		return result;
	}

	template<typename graph_t>
	std::size_t ccs(graph_t const& graph)
	{
		static_assert(is_undirected_v<graph_t>, "connected components exist in undirected graphs");
		return get_disjoint_set(graph).sets();
	}
}

#endif // !QUIVER_CONNECTED_COMPONENTS_HPP_INCLUDED
