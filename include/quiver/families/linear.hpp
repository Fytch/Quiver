/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_FAMILIES_LINEAR_HPP_INCLUDED
#define QUIVER_FAMILIES_LINEAR_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <cstddef>
#include <iterator>

namespace quiver
{
	template<
		typename edge_properties_t = void,
		typename vertex_properties_t = void,
		template<typename> class out_edge_container = vector,
		template<typename> class vertex_container = vector
	>
	adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>
	linear(std::size_t n)
	{
		adjacency_list<undirected, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> result(n);
		if(n > 1) {
			auto iter = result.v_begin();
			const auto last = std::prev(result.v_end());
			iter->out_edges.reserve(1);
			last->out_edges.reserve(1);
			if(n > 2)
				while(++iter != last)
					iter->out_edges.reserve(2);
		}
		if(n > 1)
			for(vertex_index_t i = 0; i < n - 1; ++i)
				result.add_edge(i, i + 1);
		return result;
	}
	inline auto linear(std::size_t n)
	{
		return linear<>(n);
	}
}

#endif // !QUIVER_FAMILIES_LINEAR_HPP_INCLUDED
