/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_ADJACENCY_LIST_HPP_INCLUDED
#define QUIVER_ADJACENCY_LIST_HPP_INCLUDED

#include <quiver/typedefs.hpp>
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
	template<typename edge_properties_t>
	struct out_edge : public void2empty<edge_properties_t>
	{
		using base_t = void2empty<edge_properties_t>;
		vertex_index_t to;

		base_t const& properties() const noexcept	{ return *this; }
		base_t      & properties()       noexcept	{ return *this; }

		template<typename... args_t>
		constexpr out_edge(vertex_index_t to, args_t&&... args) noexcept(std::is_nothrow_constructible_v<base_t, args_t...>)
		: base_t(std::forward<args_t>(args)...), to(to)
		{
		}
	};
	template<typename edge_properties_t>
	struct edge : public void2empty<edge_properties_t>
	{
		using base_t = void2empty<edge_properties_t>;
		vertex_index_t from, to;

		base_t const& properties() const noexcept	{ return *this; }
		base_t      & properties()       noexcept	{ return *this; }

		template<typename... args_t>
		constexpr edge(vertex_index_t from, vertex_index_t to, args_t&&... args) noexcept(std::is_nothrow_constructible_v<base_t, args_t...>)
		: base_t(std::forward<args_t>(args)...), from(from), to(to)
		{
		}
		constexpr edge(vertex_index_t from, out_edge<edge_properties_t> out) noexcept(std::is_nothrow_move_constructible_v<base_t>)
		: base_t(std::move(out.properties())), from(from), to(out.to)
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

		constexpr bool has_edge_to(vertex_index_t index) const noexcept
		{
			return std::any_of(out_edges.begin(), out_edges.end(), [index](auto const& edge){ return edge.to == index; });
		}

		constexpr void sort_edges()
		{
			sort(out_edges, [](auto const& x, auto const& y){ return x.to < y.to; });
		}

		using base_t::base_t;
		constexpr vertex(base_t const& properties)
		: base_t(properties)
		{
		}
		constexpr vertex(base_t&& properties)
		: base_t(std::move(properties))
		{
		}
	};

	template<typename graph_t>
	class vertex_span_t
	{
	public:
		using graph_type = graph_t;

	private:
		graph_type* graph;

	public:
		constexpr explicit vertex_span_t(graph_type& graph) noexcept
		: graph(&graph)
		{
		}

		constexpr auto size()     const { return graph->vertex_size(); }
		constexpr auto capacity() const { return graph->vertex_capacity(); }

		constexpr auto begin()    const { return graph->vertex_begin(); }
		constexpr auto cbegin()   const { return graph->vertex_cbegin(); }
		constexpr auto end()      const { return graph->vertex_end(); }
		constexpr auto cend()     const { return graph->vertex_cend(); }

		constexpr auto& operator[](vertex_index_t index) const { return graph->vertex_get(index); }
		constexpr auto& get(vertex_index_t index)        const { return graph->vertex_get(index); }

		template<typename... args_t>
		constexpr auto emplace(args_t&&... args) const { return graph->vertex_emplace(std::forward<args_t>(args)...); }
		constexpr auto erase(vertex_index_t index) const { return graph->vertex_erase(index); }
	};

	template<typename graph_t>
	auto begin(vertex_span_t<graph_t>& graph);
	template<typename graph_t>
	auto begin(vertex_span_t<graph_t> const& graph);
	template<typename graph_t>
	auto cbegin(vertex_span_t<graph_t> const& graph);
	template<typename graph_t>
	auto end(vertex_span_t<graph_t>& graph);
	template<typename graph_t>
	auto end(vertex_span_t<graph_t> const& graph);
	template<typename graph_t>
	auto cend(vertex_span_t<graph_t> const& graph);

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

		using vertex_t = ::quiver::vertex<vertex_properties_t, out_edge_list_t>;
		using vertices_t = vertex_container<vertex_t>;
		static_assert(is_sane_container<vertices_t>, "vertices_t must be sane container");

		inline static constexpr directivity_t directivity = dir;

	private:
		std::size_t m_v = 0, m_e = 0;
		vertices_t m_vertices;

		static constexpr void normalize(vertex_index_t& from, vertex_index_t& to) noexcept;

		bool vertex_erase_simple(vertex_index_t index);

		template<typename... args_t>
		bool add_edge_simple(vertex_index_t from, vertex_index_t to, args_t&&... args);
		bool remove_edge_simple(vertex_index_t from, vertex_index_t to);
		out_edge_t const* get_edge_simple(vertex_index_t from, vertex_index_t to) const noexcept;

		using vertex_span_type = vertex_span_t<adjacency_list>;
		friend class vertex_span_t<adjacency_list>;

		using const_vertex_span_type = vertex_span_t<const adjacency_list>;
		friend class vertex_span_t<const adjacency_list>;

		auto vertex_size() const;
		auto vertex_capacity() const;

		auto vertex_begin();
		auto vertex_begin() const;
		auto vertex_cbegin() const;
		auto vertex_end();
		auto vertex_end() const;
		auto vertex_cend() const;

		vertex_t const& vertex_get(vertex_index_t index) const noexcept;
		vertex_t& vertex_get(vertex_index_t index) noexcept;

		// TODO: should return iter
		template<typename... args_t>
		vertex_index_t vertex_emplace(args_t&&... args);
		bool vertex_erase(vertex_index_t index);

	public:
		adjacency_list() noexcept = default;
		explicit adjacency_list(std::size_t vertices);

		constexpr vertex_span_type V() noexcept;
		constexpr const_vertex_span_type V() const noexcept;

		constexpr std::size_t E() const noexcept;
		constexpr std::size_t max_edges() const noexcept;
		constexpr bool empty() const noexcept;
		constexpr bool edgeless() const noexcept;

		std::size_t in_degree(vertex_index_t index) const noexcept;
		std::size_t out_degree(vertex_index_t index) const noexcept;
		std::size_t degree(vertex_index_t index) const noexcept;

		void reserve(std::size_t vertices);
		std::size_t capacity() const noexcept;

		// TODO: should return std::pair<iter, bool>
		template<typename... args_t>
		bool add_edge(vertex_index_t from, vertex_index_t to, args_t&&... args);
		bool remove_edge(vertex_index_t from, vertex_index_t to);

		adjacency_list strip_edges() const&;
		adjacency_list&& strip_edges() &&;

		out_edge_t const* get_edge(vertex_index_t from, vertex_index_t to) const noexcept;

		template<typename invokable_t>
		void transform_outs(invokable_t invokable);
		void sort_edges();

		// returns true iff there was an edge from u to v or from v to u
		bool contract(vertex_index_t u, vertex_index_t v);
		vertex_index_t cleave(vertex_index_t v);

		void swap(adjacency_list& rhs) noexcept;

		static constexpr bool is_weighted() noexcept;
		static constexpr bool is_simple() noexcept;
	};

	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	void swap(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& lhs, adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& rhs) noexcept;

	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	std::ostream& to_dot(std::ostream& stream, adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph);
	template<directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
	std::string to_dot(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph);
}

#include <quiver/adjacency_list.inl>

#endif // !QUIVER_ADJACENCY_LIST_HPP_INCLUDED
