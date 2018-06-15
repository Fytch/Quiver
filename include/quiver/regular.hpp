/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_REGULAR_HPP_INCLUDED
#define QUIVER_REGULAR_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <limits>
#include <vector>
#include <optional>

namespace quiver
{
	template<typename graph_t>
	bool is_regular(graph_t const& graph, std::size_t degree) noexcept
	{
		if constexpr(is_directed_v<graph_t>) {
			if(graph.E() != graph.V() * degree)
				return false;
			if(!std::all_of(graph.v_begin(), graph.v_end(), [degree](auto const& v){ return v.out_degree() == degree; }))
				return false;
			std::vector<std::size_t> in_degrees(graph.V(), 0);
			for(auto iter = graph.v_begin(); iter != graph.v_end(); ++iter)
				for(auto const& out_edge : iter->out_edges)
					if(++in_degrees[out_edge->to] > degree)
						return false;
			return true; // we don't need to check that in_degrees[.] == degree, because of the first if-condition
		} else if constexpr(is_undirected_v<graph_t>) {
			if(graph.E() != graph.V() * degree / 2)
				return false;
			return std::all_of(graph.v_begin(), graph.v_end(), [degree](auto const& v){ return v.out_degree() == degree; });
		}
	}

	template<typename graph_t>
	std::optional<std::size_t> regular_degree(graph_t const& graph) noexcept
	{
		if(graph.empty())
			return std::nullopt;
		const std::size_t hypothesis = graph.vertex(0).out_degree();
		if(is_regular(graph, hypothesis))
			return hypothesis;
		else
			return std::nullopt;
	}

	template<typename graph_t>
	bool is_regular(graph_t const& graph) noexcept
	{
		return regular_degree(graph);
	}
}

#endif // !QUIVER_REGULAR_HPP_INCLUDED
