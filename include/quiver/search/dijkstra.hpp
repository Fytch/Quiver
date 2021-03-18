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
#include <tuple>
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
		template<typename scalar_t, template<typename> typename... additional_t>
		class queue_entry_t : public std::tuple<vertex_index_t, scalar_t, additional_t<scalar_t>...>
		{
			using base_t = std::tuple<vertex_index_t, scalar_t, additional_t<scalar_t>...>;

		public:
			template<typename... discard_t>
			[[nodiscard]] constexpr queue_entry_t(vertex_index_t index, scalar_t distance, additional_t<scalar_t>... additional, discard_t&&...)
			: base_t(index, distance, additional...)
			{
			}

			[[nodiscard]] constexpr base_t const& as_tuple() const noexcept { return *this; }
			[[nodiscard]] constexpr base_t      & as_tuple()       noexcept { return *this; }

			[[nodiscard]] constexpr auto   const& index  () const noexcept { return std::get<0>(as_tuple()); }
			[[nodiscard]] constexpr auto        & index  ()       noexcept { return std::get<0>(as_tuple()); }

			[[nodiscard]] constexpr auto   const& distance() const noexcept { return std::get<1>(as_tuple()); }
			[[nodiscard]] constexpr auto        & distance()       noexcept { return std::get<1>(as_tuple()); }
		};
		template<typename scalar_t, template<typename> typename... additional_t>
		[[nodiscard]] constexpr bool operator<(queue_entry_t<scalar_t, additional_t...> const& lhs, queue_entry_t<scalar_t, additional_t...> const& rhs)
		{
			return lhs.distance() < rhs.distance();
		}

		template<template<typename> typename... additional_t>
		struct bind_queue_entry_t
		{
			template<typename scalar_t>
			using templ = queue_entry_t<scalar_t, additional_t...>;
		};

		template<typename T>
		struct templatize
		{
			template<typename scalar_t>
			using templ = T;
		};

		// TODO: let the caller choose the heap data structure

		template<template<typename> typename basic_queue_entry_t, typename graph_t, typename visitor_t, visited_predicate has_been_visited_t, typename weight_invokable_t>
		bool basic_dijkstra(graph_t& graph, std::ranges::input_range auto const& start, visitor_t visitor, has_been_visited_t has_been_visited, weight_invokable_t weight_invokable)
		{
			using out_edge_t = typename graph_t::out_edge_t;
			using scalar_t = std::invoke_result_t<weight_invokable_t, vertex_index_t, const out_edge_t>;
			using queue_entry_t = basic_queue_entry_t<scalar_t>;

			binary_heap<queue_entry_t> queue;
			for(vertex_index_t index : start) {
				assert(index < graph.V.size());
				queue.push(index, scalar_t{}, index);
			}

			do {
				// The reason we don't already .extract_top here is because it
				// causes unnecessary workload in case visitor returns true.
				queue_entry_t const& top = queue.top();
				if(has_been_visited(top.index())) {
					queue.pop();
					continue;
				}
				if(std::apply(visitor, top.as_tuple()))
					return true;

				queue_entry_t extracted = queue.extract_top();
				for(out_edge_t const& edge : graph.V[extracted.index()].out_edges)
					if(!has_been_visited(edge.to)) {
						scalar_t&& edge_weight = weight_invokable(extracted.index(), edge);
						assert(std::as_const(extracted.distance()) + std::as_const(edge_weight) >= std::as_const(extracted.distance()));
						queue.push(edge.to, std::move(extracted.distance()) + std::move(edge_weight), extracted.index());
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
		return detail::basic_dijkstra<detail::bind_queue_entry_t<>::templ, graph_t, visitor_t, has_been_visited_t, weight_invokable_t>(graph, start, visitor, has_been_visited, weight_invokable);
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
		return detail::basic_dijkstra<detail::bind_queue_entry_t<detail::templatize<vertex_index_t>::templ>::templ, graph_t, visitor_t, has_been_visited_t, weight_invokable_t>(graph, start, visitor, has_been_visited, weight_invokable);
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

	// TODO: generalize the *shortest_path overloads s.t. we can use it for BFS too
	// TODO: out iterator overloads for the *shortest_path variant
	// TODO: maybe return the same vertex container as the graph. how would the current implementation even work if the vertex data structure were e.g. a hash map?
	// TODO: maybe offer the functionality to store the result within the vertex data
}

#endif // !QUIVER_SEARCH_DIJKSTRA_HPP_INCLUDED
