/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_ORIENTATION_HPP_INCLUDED
#define QUIVER_ORIENTATION_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <cstddef>
#include <vector>

namespace quiver
{
	// TODO: write overloads that take an rvalue reference in order to reuse memory

	// orientation is a function bool(vertex_index_t from, vertex_index_t to) that returns true iff (from,to) should be element of E.
	// The existence of (from,to) implies the nonexistence of (to,from) in E.
	template<typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container, typename orientation_t>
	adjacency_list<directed, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>
	orient(adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph, orientation_t orientation)
	{
		const std::size_t V = graph.V.size();
		adjacency_list<directed, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> result;
		result.reserve(V);
		for(auto const& vertex : graph)
			result.V.emplace(vertex.properties());
		for(vertex_index_t v = 0; v < V; ++v) {
			for(auto const& out_edge : graph.V[v].out_edges) {
				if(out_edge.to < v) {
					if(orientation(out_edge.to, v))
						result.E.emplace(out_edge.to, v, out_edge.properties());
					else
						result.E.emplace(v, out_edge.to, out_edge.properties());
				}
			}
		}
		return result;
	}
	template<typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>
	disorient(adjacency_list<directed, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph)
	{
		const std::size_t V = graph.V.size();
		adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> result;
		result.reserve(V);
		std::vector<bool> exists(V * V, false); // TODO: only needs to be a strict triangular matrix; saves more than half the memory
		for(auto const& vertex : graph)
			result.V.emplace(vertex.properties());
		for(vertex_index_t v = 0; v < V; ++v) {
			for(auto const& out_edge : graph.V[v].out_edges) {
				if(!exists[V * out_edge.to + v]) {
					result.E.emplace(out_edge.to, v, out_edge.properties());
					exists[V * out_edge.to + v] = true;
					exists[V * v + out_edge.to] = true;
				}
			}
		}
		return result;
	}
}

#endif // !QUIVER_ORIENTATION_HPP_INCLUDED
