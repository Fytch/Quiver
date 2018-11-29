/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_STRONGLY_CONNECTED_COMPONENTS_HPP_INCLUDED
#define QUIVER_STRONGLY_CONNECTED_COMPONENTS_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <cstddef>
#include <stack>
#include <vector>

namespace quiver
{
	namespace tarjan_detail
	{
		struct tarjan_state{
			std::vector<bool> discovered;
			std::vector<std::size_t> scc_index;
			std::vector<vertex_index_t> lowlink;
			std::stack<std::size_t> scc_stack;
			std::vector<bool> on_scc_stack;
			std::size_t current_scc_index {0};

			std::size_t nr_sccs_found {0};
			std::vector<std::size_t> node_to_scc_index_mapping;

			tarjan_state(std::size_t V)
				: discovered(V, false),
				  scc_index(V),
				  lowlink(V),
				  on_scc_stack(V, false),
				  node_to_scc_index_mapping(V)
			{}
		};

		template<
			typename edge_properties_t,
			typename vertex_properties_t,
			template<typename> class out_edge_container,
			template<typename> class vertex_container
		>
		void tarjan_recurse(const vertex_index_t v,
							adjacency_list<directed, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph,
							tarjan_state& state)
		{
			state.scc_index[v] = state.current_scc_index;
			state.discovered[v] = true;
			state.lowlink[v] = state.current_scc_index;
			++state.current_scc_index;

			state.scc_stack.push(v);
			state.on_scc_stack[v] = true;

			for(const auto& out_edge : graph.vertex(v).out_edges) {
				vertex_index_t w = out_edge.to;
				if(!state.discovered[w]) {
					tarjan_recurse(w, graph, state);
					state.lowlink[v] = std::min(state.lowlink[v], state.lowlink[w]);
				} else if (state.on_scc_stack[w]) {
					state.lowlink[v] = std::min(state.lowlink[v], state.scc_index[w]);
				}
			}

			if (state.lowlink[v] == state.scc_index[v]) {
				vertex_index_t w;
				do {
					w = state.scc_stack.top(); state.scc_stack.pop();
					state.on_scc_stack[w] = false;
					state.node_to_scc_index_mapping[w] = state.nr_sccs_found;
				} while (w != v);
				++ state.nr_sccs_found;
			}
		}
	}

	template<
		typename edge_properties_t,
		typename vertex_properties_t,
		template<typename> class out_edge_container,
		template<typename> class vertex_container
	>
	std::vector<std::size_t> // vertex [0..V] -> scc index
	tarjan_recursive(adjacency_list<directed, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph)
	{
		tarjan_detail::tarjan_state state(graph.V());

		for(vertex_index_t start_node = 0; start_node < graph.V(); ++start_node) {
			if(!state.discovered[start_node])
				tarjan_recurse(start_node, graph, state);
		}

		return state.node_to_scc_index_mapping;
	}
}

#endif // !QUIVER_STRONGLY_CONNECTED_COMPONENTS_HPP_INCLUDED
