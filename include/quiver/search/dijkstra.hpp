/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_SEARCH_DIJKSTRA_HPP_INCLUDED
#define QUIVER_SEARCH_DIJKSTRA_HPP_INCLUDED

#include <quiver/search/visitation_table.hpp>
#include <quiver/adjacency_list.hpp>
#include <quiver/heaps.hpp>
#include <quiver/util.hpp>
#include <type_traits>
#include <utility>
#include <cassert>
#include <ranges>

namespace quiver
{
	// TODO: Thus far, I've only implemented the "always insert" variant of Dijkstra. I shall also implement the "decrease key" variant.

	template<typename graph_t>
	[[nodiscard]] constexpr auto dijkstra_default_weight_invokable() noexcept
	{
		using out_edge_t = typename graph_t::out_edge_t;
		return [](vertex_index_t vi, out_edge_t const& oe) constexpr {
			(void)vi; // -Wunused-parameter
			return oe.weight;
		};
	}

	namespace detail
	{
		template<typename out_edge_t, typename scalar_t, template<typename, typename, typename> typename... additional_t>
		struct dijkstra_queue_entry_t : public additional_t<out_edge_t, scalar_t, dijkstra_queue_entry_t<out_edge_t, scalar_t, additional_t...>>...
		{
			vertex_index_t index;
			scalar_t distance;

			[[nodiscard]] constexpr dijkstra_queue_entry_t(vertex_index_t index)
			: additional_t<out_edge_t, scalar_t, dijkstra_queue_entry_t>(std::as_const(index))..., index(index), distance(0)
			{
			}
			[[nodiscard]] constexpr dijkstra_queue_entry_t(vertex_index_t index, scalar_t&& distance, out_edge_t const& edge, dijkstra_queue_entry_t&& previous)
			: additional_t<out_edge_t, scalar_t, dijkstra_queue_entry_t>(std::as_const(index), std::as_const(distance), edge, std::as_const(previous))..., index(index), distance(std::move(previous.distance) + std::move(distance))
			{
			}

			template<typename invocable_t>
			constexpr decltype(auto) apply_to(invocable_t invocable) const
			{
				return invocable(index, distance, this->additional_t<out_edge_t, scalar_t, dijkstra_queue_entry_t>::get()...);
			}
		};
		template<typename out_edge_t, typename scalar_t, template<typename, typename, typename> typename... additional_t>
		[[nodiscard]] constexpr bool operator<(dijkstra_queue_entry_t<out_edge_t, scalar_t, additional_t...> const& lhs, dijkstra_queue_entry_t<out_edge_t, scalar_t, additional_t...> const& rhs)
		{
			return lhs.distance < rhs.distance;
		}

		template<template<typename, typename, typename> typename... additional_t>
		struct bind_dijkstra_queue_entry_t
		{
			template<typename out_edge_t, typename scalar_t>
			using templ = dijkstra_queue_entry_t<out_edge_t, scalar_t, additional_t...>;
		};

		template<typename out_edge_t, typename scalar_t, typename dijkstra_queue_entry_t>
		struct dijkstra_predecessor_t
		{
			vertex_index_t predecessor;

			[[nodiscard]] constexpr dijkstra_predecessor_t(vertex_index_t index) noexcept
			: predecessor(index)
			{
			}
			[[nodiscard]] constexpr dijkstra_predecessor_t(vertex_index_t index, scalar_t const& distance, out_edge_t const& edge, dijkstra_queue_entry_t const& previous) noexcept
			: predecessor(previous.index)
			{
				(void)index; // -Wunused-parameter
				(void)distance; // -Wunused-parameter
				(void)edge; // -Wunused-parameter
			}

			[[nodiscard]] constexpr vertex_index_t const& get() const noexcept
			{
				return predecessor;
			}
		};

		// TODO: let the caller choose the heap data structure

		template<template<typename, typename> typename basic_queue_entry_t, typename graph_t, typename visitor_t, visited_predicate has_been_visited_t, typename weight_invokable_t>
		bool basic_dijkstra(graph_t& graph, std::ranges::input_range auto const& start, visitor_t visitor, has_been_visited_t has_been_visited, weight_invokable_t weight_invokable)
		{
			using out_edge_t = typename graph_t::out_edge_t;
			using scalar_t = std::invoke_result_t<weight_invokable_t, vertex_index_t, const out_edge_t>;
			using queue_entry_t = basic_queue_entry_t<out_edge_t, scalar_t>;

			binary_heap<queue_entry_t> queue;
			for(vertex_index_t index : start) {
				assert(index < graph.V.size());
				queue.push(index);
			}

			do {
				// The reason we don't already .extract_top here is because it
				// causes unnecessary workload in case visitor returns true.
				queue_entry_t const& top = queue.top();
				if(has_been_visited(std::as_const(top.index))) {
					queue.pop();
					continue;
				}
				if(top.apply_to(visitor))
					return true;

				queue_entry_t extracted = queue.extract_top();
				for(out_edge_t const& edge : graph.V[extracted.index].out_edges)
					if(!has_been_visited(edge.to)) {
						scalar_t&& edge_weight = weight_invokable(std::as_const(extracted.index), edge);
						assert(std::as_const(extracted.distance) + std::as_const(edge_weight) >= std::as_const(extracted.distance));
						queue.push(edge.to, std::move(edge_weight), edge, std::move(extracted));
					}
			} while(!queue.empty());
			return false;
		}
	}

	// visitor shall have the signature bool(vertex_index_t, scalar_t).
	// If visitor returns true, exit the search and return true.
	// Returns false if no visitation returned true.
	// has_been_visited shall have the signature bool(vertex_index_t)
	// and shall return true iff the vertex has been visited by visitor.
	// weight_invokable shall have the signature scalar_t(vertex_index_t, out_edge_t).
	template<typename graph_t, typename visitor_t, visited_predicate has_been_visited_t, typename weight_invokable_t>
	bool dijkstra(graph_t& graph, std::ranges::input_range auto const& start, visitor_t visitor, has_been_visited_t has_been_visited, weight_invokable_t weight_invokable)
	{
		return detail::basic_dijkstra<detail::bind_dijkstra_queue_entry_t<>::templ, graph_t, visitor_t, has_been_visited_t, weight_invokable_t>(graph, start, visitor, has_been_visited, weight_invokable);
	}
	template<typename graph_t, typename visitor_t, visited_predicate has_been_visited_t, typename weight_invokable_t>
	bool dijkstra(graph_t& graph, vertex_index_t start, visitor_t visitor, has_been_visited_t has_been_visited, weight_invokable_t weight_invokable)
	{
		return dijkstra<graph_t, visitor_t, has_been_visited_t, weight_invokable_t>(graph, std::ranges::single_view(start), visitor, has_been_visited, weight_invokable);
	}
	template<typename graph_t, typename visitor_t, visited_predicate has_been_visited_t>
	bool dijkstra(graph_t& graph, std::ranges::input_range auto const& start, visitor_t visitor, has_been_visited_t has_been_visited)
	{
		return dijkstra<graph_t, visitor_t, has_been_visited_t>(graph, start, visitor, has_been_visited, dijkstra_default_weight_invokable<graph_t>());
	}
	template<typename graph_t, typename visitor_t, visited_predicate has_been_visited_t>
	bool dijkstra(graph_t& graph, vertex_index_t start, visitor_t visitor, has_been_visited_t has_been_visited)
	{
		return dijkstra<graph_t, visitor_t, has_been_visited_t>(graph, std::ranges::single_view(start), visitor, has_been_visited);
	}
	template<typename graph_t, typename visitor_t>
	bool dijkstra(graph_t& graph, std::ranges::input_range auto const& start, visitor_t visitor)
	{
		visitation_table_t visitation_table(graph);
		return dijkstra<graph_t, decltype(visitation_table.hook_visitor(visitor)), visitation_table_t&>(graph, start, visitation_table.hook_visitor(visitor), visitation_table);
	}
	template<typename graph_t, typename visitor_t>
	bool dijkstra(graph_t& graph, vertex_index_t start, visitor_t visitor)
	{
		return dijkstra<graph_t, visitor_t>(graph, std::ranges::single_view(start), visitor);
	}

	// visitor shall have the signature bool(vertex_index_t, scalar_t, vertex_index_t).
	// If visitor returns true, exit the search and return true.
	// Returns false if no visitation returned true.
	// has_been_visited shall have the signature bool(vertex_index_t)
	// and shall return true iff the vertex has been visited by visitor.
	// weight_invokable shall have the signature scalar_t(vertex_index_t, out_edge_t).
	template<typename graph_t, typename visitor_t, visited_predicate has_been_visited_t, typename weight_invokable_t>
	bool dijkstra_shortest_path(graph_t& graph, std::ranges::input_range auto const& start, visitor_t visitor, has_been_visited_t has_been_visited, weight_invokable_t weight_invokable)
	{
		return detail::basic_dijkstra<detail::bind_dijkstra_queue_entry_t<detail::dijkstra_predecessor_t>::templ, graph_t, visitor_t, has_been_visited_t, weight_invokable_t>(graph, start, visitor, has_been_visited, weight_invokable);
	}
	template<typename graph_t, typename visitor_t, visited_predicate has_been_visited_t, typename weight_invokable_t>
	bool dijkstra_shortest_path(graph_t& graph, vertex_index_t start, visitor_t visitor, has_been_visited_t has_been_visited, weight_invokable_t weight_invokable)
	{
		return dijkstra_shortest_path<graph_t, visitor_t, has_been_visited_t, weight_invokable_t>(graph, std::ranges::single_view(start), visitor, has_been_visited, weight_invokable);
	}
	// Return type is a map from vertex index to (distance from start, predecessor)
	template<typename graph_t, typename weight_invokable_t>
	[[nodiscard]] auto dijkstra_shortest_path(graph_t& graph, std::ranges::input_range auto const& start, weight_invokable_t weight_invokable)
	{
		using out_edge_t = typename graph_t::out_edge_t;
		using scalar_t = std::invoke_result_t<weight_invokable_t, vertex_index_t, const out_edge_t>;

		constexpr vertex_index_t no_predecessor = ~vertex_index_t{};
		std::vector<std::pair<scalar_t, vertex_index_t>> result(graph.V.size(), { inf<scalar_t>, no_predecessor });
		auto visitor = [&result](vertex_index_t vertex, scalar_t distance, vertex_index_t predecessor) -> bool {
			result[vertex] = { distance, predecessor };
			//	maybe break; if vertex is an end node
			return false;
		};
		auto has_been_visited = [&result](vertex_index_t index){
			return result[index].second != no_predecessor;
		};
		dijkstra_shortest_path<graph_t, decltype(visitor), decltype(has_been_visited), weight_invokable_t>(graph, start, std::move(visitor), std::move(has_been_visited), weight_invokable);
		return result;
	}
	template<typename graph_t, typename weight_invokable_t>
	[[nodiscard]] auto dijkstra_shortest_path(graph_t& graph, vertex_index_t start, weight_invokable_t weight_invokable)
	{
		return dijkstra_shortest_path<graph_t, weight_invokable_t>(graph, std::ranges::single_view(start), weight_invokable);
	}
	template<typename graph_t>
	[[nodiscard]] auto dijkstra_shortest_path(graph_t& graph, std::ranges::input_range auto const& start)
	{
		return dijkstra_shortest_path(graph, start, dijkstra_default_weight_invokable<graph_t>());
	}
	template<typename graph_t>
	[[nodiscard]] auto dijkstra_shortest_path(graph_t& graph, vertex_index_t start)
	{
		return dijkstra_shortest_path<graph_t>(graph, std::ranges::single_view(start));
	}

	// TODO: overloads with end node(s)

	// if implemented, implement them for bfs_shortest_path too
	// TODO: out iterator overloads for the *shortest_path variant
	// TODO: maybe return the same vertex container as the graph. how would the current implementation even work if the vertex data structure were e.g. a hash map?
	// TODO: maybe offer the functionality to store the result within the vertex data
	// TODO: offer possibility to only get predecessors, or only distance
}

#endif // !QUIVER_SEARCH_DIJKSTRA_HPP_INCLUDED
