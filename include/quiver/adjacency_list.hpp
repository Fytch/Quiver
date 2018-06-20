/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_ADJACENCY_LIST_HPP_INCLUDED
#define QUIVER_ADJACENCY_LIST_HPP_INCLUDED

#include <quiver/util.hpp>
#include <quiver/properties.hpp>
#include <quiver/container_interface.hpp>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <ostream>
#include <string>

namespace quiver
{
	using vertex_index_t = std::size_t;

	enum directivity_t
	{
		directed,
		undirected
	};

	template<typename graph_t>
	struct is_directed : public std::integral_constant<bool, graph_t::directivity == directed>
	{
	};
	template<typename graph_t>
	inline constexpr bool is_directed_v = is_directed<graph_t>::value;

	template<typename graph_t>
	struct is_undirected : public std::integral_constant<bool, graph_t::directivity == undirected>
	{
	};
	template<typename graph_t>
	inline constexpr bool is_undirected_v = is_undirected<graph_t>::value;

	template<typename edge_properties_t>
	struct edge : public void2empty<edge_properties_t>
	{
		using base_t = void2empty<edge_properties_t>;
		vertex_index_t from, to;

		base_t const& properties() const noexcept	{ return *this; }
		base_t      & properties()       noexcept	{ return *this; }

		template<typename... args_t>
		edge(vertex_index_t from, vertex_index_t to, args_t&&... args) noexcept(std::is_nothrow_constructible_v<base_t, args_t...>)
		: base_t(std::forward<args_t>(args)...), from{ from }, to{ to }
		{
		}
	};
	template<typename edge_properties_t>
	struct out_edge : public void2empty<edge_properties_t>
	{
		using base_t = void2empty<edge_properties_t>;
		vertex_index_t to;

		base_t const& properties() const noexcept	{ return *this; }
		base_t      & properties()       noexcept	{ return *this; }

		template<typename... args_t>
		out_edge(vertex_index_t to, args_t&&... args) noexcept(std::is_nothrow_constructible_v<base_t, args_t...>)
		: base_t(std::forward<args_t>(args)...), to{ to }
		{
		}
	};

	template<typename vertex_properties_t, typename out_edge_list_t>
	struct vertex : public void2empty<vertex_properties_t>
	{
		using base_t = void2empty<vertex_properties_t>;
		out_edge_list_t out_edges;

		base_t const& properties() const noexcept	{ return *this; }
		base_t      & properties()       noexcept	{ return *this; }

		std::size_t out_degree() const noexcept		{ return out_edges.size(); }

		bool has_edge_to(vertex_index_t index) const noexcept
		{
			return std::any_of(out_edges.begin(), out_edges.end(), [index](auto const& edge){ return edge.to == index; });
		}

		using base_t::base_t;
		vertex(base_t const& properties)
		: base_t(properties)
		{
		}
		vertex(base_t&& properties)
		: base_t(std::move(properties))
		{
		}
	};

	// no loops, no multiedges
	template<
		directivity_t dir = directed,
		typename edge_properties_t = void,
		typename vertex_properties_t = void,
		template<typename> class out_edge_container = vector,
		template<typename> class vertex_container = vector
	>
	class adjacency_list
	{
	public:
		using edge_t = edge<edge_properties_t>;
		using out_edge_t = out_edge<edge_properties_t>;
		using out_edge_list_t = out_edge_container<out_edge_t>;
		static_assert(is_sane_container<out_edge_list_t>, "out_edge_list_t must be sane container");

		using vertex_t = vertex<vertex_properties_t, out_edge_list_t>;
		using vertices_t = vertex_container<vertex_t>;
		static_assert(is_sane_container<vertices_t>, "vertices_t must be sane container");

		inline static constexpr directivity_t directivity = dir;

	private:
		std::size_t m_v = 0, m_e = 0;
		vertices_t m_vertices;

		static constexpr void normalize(vertex_index_t& from, vertex_index_t& to) noexcept;

		template<typename... args_t>
		bool add_edge_simple(vertex_index_t from, vertex_index_t to, args_t&&... args);
		bool remove_edge_simple(vertex_index_t from, vertex_index_t to);
		out_edge_t const* get_edge_simple(vertex_index_t from, vertex_index_t to) const noexcept;

	public:
		adjacency_list() noexcept = default;
		explicit adjacency_list(std::size_t vertices);

		constexpr std::size_t V() const noexcept;
		constexpr std::size_t E() const noexcept;
		constexpr std::size_t max_edges() const noexcept;
		constexpr bool empty() const noexcept;
		constexpr bool edgeless() const noexcept;

		vertex_t const& vertex(vertex_index_t index) const noexcept;
		vertex_t& vertex(vertex_index_t index) noexcept;

		std::size_t in_degree(vertex_index_t index) const noexcept;
		std::size_t out_degree(vertex_index_t index) const noexcept;
		std::size_t degree(vertex_index_t index) const noexcept;

		// TODO: should return iter
		template<typename... args_t>
		vertex_index_t add_vertex(args_t&&... args);
		bool remove_vertex(vertex_index_t index);

		void reserve(std::size_t vertices);
		std::size_t capacity() const noexcept;

		// TODO: should return std::pair<iter, bool>
		template<typename... args_t>
		bool add_edge(vertex_index_t from, vertex_index_t to, args_t&&... args);
		bool remove_edge(vertex_index_t from, vertex_index_t to);

		auto v_begin();
		auto v_begin() const;
		auto v_cbegin() const;
		auto v_end();
		auto v_end() const;
		auto v_cend() const;

		// returns a graph with just the vertices
		adjacency_list vertices() const&;
		adjacency_list&& vertices() &&;

		out_edge_t const* get_edge(vertex_index_t from, vertex_index_t to) const noexcept;

		template<typename invokable_t>
		void transform_outs(invokable_t invokable);

		void swap(adjacency_list& rhs) noexcept;

		static constexpr bool is_weighted() noexcept;
	};

	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	auto begin(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>& graph);
	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	auto begin(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph);
	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	auto cbegin(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph);
	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	auto end(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>& graph);
	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	auto end(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph);
	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	auto cend(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph);

	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	void swap(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& lhs, adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& rhs) noexcept;

	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	std::ostream& to_dot(std::ostream& stream, adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph);
	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	std::string to_dot(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph);
}

#include <quiver/adjacency_list.inl>

#endif // !QUIVER_ADJACENCY_LIST_HPP_INCLUDED
