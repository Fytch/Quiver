/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_SEARCH_BFS_HPP_INCLUDED
#define QUIVER_SEARCH_BFS_HPP_INCLUDED

#include <quiver/search/visitation_table.hpp>
#include <quiver/adjacency_list.hpp>
#include <quiver/util.hpp>
#include <ranges>
#include <vector>
#include <queue>
#include <cassert>

namespace quiver
{
	using bfs_path_length_t = unsigned int;

	namespace detail
	{
		template<typename out_edge_t, template<typename, typename> typename... additional_t>
		struct bfs_queue_entry_t : public additional_t<out_edge_t, bfs_queue_entry_t<out_edge_t, additional_t...>>...
		{
			vertex_index_t index;

			[[nodiscard]] constexpr bfs_queue_entry_t(vertex_index_t index)
			: additional_t<out_edge_t, bfs_queue_entry_t>(index)..., index(index)
			{
			}
			[[nodiscard]] constexpr bfs_queue_entry_t(vertex_index_t index, out_edge_t const& edge, bfs_queue_entry_t&& previous)
			: additional_t<out_edge_t, bfs_queue_entry_t>(std::as_const(index), edge, std::as_const(previous))..., index(index)
			{
			}

			template<typename invocable_t>
			constexpr decltype(auto) apply_to(invocable_t invocable) const
			{
				return invocable(index, this->additional_t<out_edge_t, bfs_queue_entry_t>::get()...);
			}
		};

		template<template<typename, typename> typename... additional_t>
		struct bind_bfs_queue_entry_t
		{
			template<typename out_edge_t>
			using templ = bfs_queue_entry_t<out_edge_t, additional_t...>;
		};

		template<typename out_edge_t, typename bfs_queue_entry_t>
		struct bfs_distance_t
		{
			bfs_path_length_t distance;

			[[nodiscard]] constexpr bfs_distance_t(vertex_index_t index) noexcept
			: distance(0)
			{
				(void)index; // -Wunused-parameter
			}
			[[nodiscard]] constexpr bfs_distance_t(vertex_index_t index, out_edge_t const& edge, bfs_queue_entry_t const& previous) noexcept
			: distance(previous.distance + 1)
			{
				(void)index; // -Wunused-parameter
				(void)edge; // -Wunused-parameter
			}

			[[nodiscard]] constexpr bfs_path_length_t const& get() const noexcept
			{
				return distance;
			}
		};

		template<typename out_edge_t, typename bfs_queue_entry_t>
		struct bfs_predecessor_t
		{
			vertex_index_t predecessor;

			[[nodiscard]] constexpr bfs_predecessor_t(vertex_index_t index) noexcept
			: predecessor(index)
			{
			}
			[[nodiscard]] constexpr bfs_predecessor_t(vertex_index_t index, out_edge_t const& edge, bfs_queue_entry_t const& previous) noexcept
			: predecessor(previous.index)
			{
				(void)index; // -Wunused-parameter
				(void)edge; // -Wunused-parameter
			}

			[[nodiscard]] constexpr vertex_index_t const& get() const noexcept
			{
				return predecessor;
			}
		};

		// TODO: can we somehow remove "enqueued" in favor of "has_been_visited" but without allowing multiple insertions into the queue?

		template<template<typename> typename basic_queue_entry_t, typename graph_t, typename visitor_t>
		bool basic_bfs(graph_t& graph, std::ranges::input_range auto const& start, visitor_t visitor)
		{
			using vertex_t = copy_const<graph_t, typename graph_t::vertex_t>;
			using out_edge_t = typename graph_t::out_edge_t;
			using queue_entry_t = basic_queue_entry_t<out_edge_t>;

			std::vector<bool> enqueued(graph.V.size(), false);
			std::queue<queue_entry_t> neighbors;
			for(vertex_index_t index : start) {
				assert(index < graph.V.size());
				enqueued[index] = true;
				neighbors.emplace(index);
			}

			do {
				queue_entry_t front = neighbors.front();
				assert(enqueued[front.index]);
				vertex_t& vertex = graph.V[front.index];

				if(front.apply_to(visitor))
					return true;

				neighbors.pop();
				for(out_edge_t const& edge : vertex.out_edges)
					if(!enqueued[edge.to]) {
						neighbors.emplace(edge.to, edge, std::move(front));
						enqueued[edge.to] = true;
					}
			} while(!neighbors.empty());
			return false;
		}
	}

	// visitor shall have the signature bool(vertex_index_t).
	// If visitor returns true, exit the search and return true.
	// Returns false if no visitation returned true.
	template<typename graph_t, typename visitor_t>
	bool bfs(graph_t& graph, std::ranges::input_range auto const& start, visitor_t visitor)
	{
		return detail::basic_bfs<detail::bind_bfs_queue_entry_t<>::templ, graph_t, visitor_t>(graph, start, visitor);
	}
	template<typename graph_t, typename visitor_t>
	bool bfs(graph_t& graph, vertex_index_t start, visitor_t visitor)
	{
		return bfs<graph_t, visitor_t>(graph, std::ranges::single_view(start), visitor);
	}

	// visitor shall have the signature bool(vertex_index_t, bfs_path_length_t, vertex_index_t).
	// If visitor returns true, exit the search and return true.
	// Returns false if no visitation returned true.
	// has_been_visited shall have the signature bool(vertex_index_t)
	// and shall return true iff the vertex has been visited by visitor.
	template<typename graph_t, typename visitor_t>
	bool bfs_shortest_path(graph_t& graph, std::ranges::input_range auto const& start, visitor_t visitor)
	{
		return detail::basic_bfs<detail::bind_bfs_queue_entry_t<detail::bfs_distance_t, detail::bfs_predecessor_t>::templ, graph_t, visitor_t>(graph, start, visitor);
	}
	template<typename graph_t, typename visitor_t>
	bool bfs_shortest_path(graph_t& graph, vertex_index_t start, visitor_t visitor)
	{
		return bfs_shortest_path<graph_t, visitor_t>(graph, std::ranges::single_view(start), visitor);
	}
	// Return type is a map from vertex index to (distance from start, predecessor)
	template<typename graph_t>
	[[nodiscard]] auto bfs_shortest_path(graph_t& graph, std::ranges::input_range auto const& start)
	{
		constexpr vertex_index_t no_predecessor = ~vertex_index_t{};
		std::vector<std::pair<bfs_path_length_t, vertex_index_t>> result(graph.V.size(), { inf<bfs_path_length_t>, no_predecessor });
		auto visitor = [&result](vertex_index_t vertex, bfs_path_length_t distance, vertex_index_t predecessor) -> bool {
			result[vertex] = { distance, predecessor };
			// maybe break; if vertex is an end node
			return false;
		};
		bfs_shortest_path<graph_t, decltype(visitor)>(graph, start, std::move(visitor));
		return result;
	}
	template<typename graph_t>
	[[nodiscard]] auto bfs_shortest_path(graph_t& graph, vertex_index_t start)
	{
		return bfs_shortest_path<graph_t>(graph, std::ranges::single_view(start));
	}

	// TODO: the same TODOs as for dijkstra_shortest_path
}

#endif // !QUIVER_SEARCH_BFS_HPP_INCLUDED
