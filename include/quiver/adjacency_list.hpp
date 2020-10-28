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

	namespace detail
	{
		template<typename adjacency_list_t, typename intermediate_t>
		class vertex_span_t final
		{
			// static_assert(std::is_standard_layout_v<intermediate_t>, "could not establish standard layout-ness");

		public:
			friend intermediate_t;
			using adjacency_list_type = adjacency_list_t;

		private:
			constexpr adjacency_list_type const* get_parent() const noexcept
			{
				return static_cast<adjacency_list_type const*>(reinterpret_cast<intermediate_t const*>(reinterpret_cast<char const*>(this) - offsetof(intermediate_t, V)));
			}
			constexpr adjacency_list_type* get_parent() noexcept
			{
				return const_cast<adjacency_list_type*>(std::as_const(*this).get_parent());
			}

			constexpr vertex_span_t() noexcept = default;
			constexpr vertex_span_t(vertex_span_t const&) noexcept = delete;
			constexpr vertex_span_t(vertex_span_t&&) noexcept = delete;
			constexpr vertex_span_t& operator=(vertex_span_t const&) noexcept = delete;
			constexpr vertex_span_t& operator=(vertex_span_t&&) noexcept = delete;

		public:
			constexpr auto size()     const { return get_parent()->vertex_size(); }
			constexpr auto capacity() const { return get_parent()->vertex_capacity(); }

			constexpr auto begin()          { return get_parent()->vertex_begin(); }
			constexpr auto begin()    const { return get_parent()->vertex_begin(); }
			constexpr auto cbegin()   const { return get_parent()->vertex_cbegin(); }
			constexpr auto end()            { return get_parent()->vertex_end(); }
			constexpr auto end()      const { return get_parent()->vertex_end(); }
			constexpr auto cend()     const { return get_parent()->vertex_cend(); }

			constexpr auto& operator[](vertex_index_t index)       { return get_parent()->vertex_get(index); }
			constexpr auto& operator[](vertex_index_t index) const { return get_parent()->vertex_get(index); }
			constexpr auto& get(vertex_index_t index)              { return get_parent()->vertex_get(index); }
			constexpr auto& get(vertex_index_t index)        const { return get_parent()->vertex_get(index); }

			template<typename... args_t>
			constexpr auto emplace(args_t&&... args) { return get_parent()->vertex_emplace(std::forward<args_t>(args)...); }
			constexpr auto erase(vertex_index_t index) { return get_parent()->vertex_erase(index); }
		};

		template<typename adjacency_list_t, typename intermediate_t>
		auto begin(vertex_span_t<adjacency_list_t, intermediate_t>& graph);
		template<typename adjacency_list_t, typename intermediate_t>
		auto begin(vertex_span_t<adjacency_list_t, intermediate_t> const& graph);
		template<typename adjacency_list_t, typename intermediate_t>
		auto cbegin(vertex_span_t<adjacency_list_t, intermediate_t> const& graph);
		template<typename adjacency_list_t, typename intermediate_t>
		auto end(vertex_span_t<adjacency_list_t, intermediate_t>& graph);
		template<typename adjacency_list_t, typename intermediate_t>
		auto end(vertex_span_t<adjacency_list_t, intermediate_t> const& graph);
		template<typename adjacency_list_t, typename intermediate_t>
		auto cend(vertex_span_t<adjacency_list_t, intermediate_t> const& graph);

		template<typename adjacency_list_t, typename intermediate_t>
		class edge_span_t final
		{
			// static_assert(std::is_standard_layout_v<intermediate_t>, "could not establish standard layout-ness");

		public:
			friend intermediate_t;
			using adjacency_list_type = adjacency_list_t;

		private:
			constexpr adjacency_list_type const* get_parent() const noexcept
			{
				return static_cast<adjacency_list_type const*>(reinterpret_cast<intermediate_t const*>(reinterpret_cast<char const*>(this) - offsetof(intermediate_t, E)));
			}
			constexpr adjacency_list_type* get_parent() noexcept
			{
				return const_cast<adjacency_list_type*>(std::as_const(*this).get_parent());
			}

			constexpr edge_span_t() noexcept = default;
			constexpr edge_span_t(edge_span_t const&) noexcept = delete;
			constexpr edge_span_t(edge_span_t&&) noexcept = delete;
			constexpr edge_span_t& operator=(edge_span_t const&) noexcept = delete;
			constexpr edge_span_t& operator=(edge_span_t&&) noexcept = delete;

		public:
			constexpr auto size()     const { return get_parent()->edge_size(); }
			constexpr auto empty()    const { return get_parent()->edge_empty(); }
			constexpr auto max_size() const { return get_parent()->edge_max_size(); }

			constexpr auto operator()(vertex_index_t from, vertex_index_t to)       { return get_parent()->edge_get(from, to); }
			constexpr auto operator()(vertex_index_t from, vertex_index_t to) const { return get_parent()->edge_get(from, to); }
			constexpr auto get(vertex_index_t from, vertex_index_t to)              { return get_parent()->edge_get(from, to); }
			constexpr auto get(vertex_index_t from, vertex_index_t to)        const { return get_parent()->edge_get(from, to); }

			template<typename... args_t>
			constexpr auto emplace(vertex_index_t from, vertex_index_t to, args_t&&... args) { return get_parent()->edge_emplace(from, to, std::forward<args_t>(args)...); }
			constexpr auto erase(vertex_index_t from, vertex_index_t to) { return get_parent()->edge_erase(from, to); }
		};

		// We need this base class because we need a standard layout type for offsetof.
		// adjacency_list isn't standard layout because it has different access specifiers.
		template<
			typename edge_properties_t,
			typename vertex_properties_t,
			template<typename> class out_edge_container,
			template<typename> class vertex_container,
			typename derived_t
		>
		class adjacency_list_base
		{
			using vertices_t = vertex_container<::quiver::vertex<vertex_properties_t, out_edge_container<out_edge<edge_properties_t>>>>;
			using adjacency_list = derived_t;
			using vertex_span_type = vertex_span_t<adjacency_list, adjacency_list_base>;
			friend vertex_span_type;
			using edge_span_type = edge_span_t<adjacency_list, adjacency_list_base>;
			friend edge_span_type;

		protected:
			// private:
			std::size_t m_v = 0, m_e = 0;
			vertices_t m_vertices;

			// public:
			vertex_span_type V; // when renaming, the corresponding offsetof invocation must be change too
			edge_span_type E; // when renaming, the corresponding offsetof invocation must be change too

			adjacency_list_base() noexcept
			{
			}
			explicit adjacency_list_base(std::size_t vertices)
			: m_v(vertices), m_vertices(vertices)
			{
			}
			adjacency_list_base(adjacency_list_base const& rhs)
			: m_v(rhs.m_v), m_e(rhs.m_e), m_vertices(rhs.m_vertices)
			{
			}
			adjacency_list_base(adjacency_list_base&& rhs) noexcept
			: m_v(std::move(rhs.m_v)), m_e(std::move(rhs.m_e)), m_vertices(std::move(rhs.m_vertices))
			{
			}
			adjacency_list_base& operator=(adjacency_list_base const& rhs)
			{
				return *this = adjacency_list_base(rhs);
			}
			adjacency_list_base& operator=(adjacency_list_base&& rhs) noexcept
			{
				m_v = std::move(rhs.m_v);
				m_e = std::move(rhs.m_e);
				m_vertices = std::move(rhs.m_vertices);
				return *this;
			}
		};
	}

	// no loops, no multiedges
	template<
		directivity_t dir = directed,
		typename edge_properties_t = void,
		typename vertex_properties_t = void,
		template<typename> class out_edge_container = vector,
		template<typename> class vertex_container = vector
	>
	class adjacency_list : private detail::adjacency_list_base<edge_properties_t, vertex_properties_t, out_edge_container, vertex_container, adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>>
	{
		using base_t = detail::adjacency_list_base<edge_properties_t, vertex_properties_t, out_edge_container, vertex_container, adjacency_list>;

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
		using base_t::m_v;
		static_assert(std::is_same_v<decltype(m_v), std::size_t>);
		using base_t::m_e;
		static_assert(std::is_same_v<decltype(m_e), std::size_t>);
		using base_t::m_vertices;
		static_assert(std::is_same_v<decltype(m_vertices), vertices_t>);

		static constexpr void normalize(vertex_index_t& from, vertex_index_t& to) noexcept;

		// accessible through .V
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

		template<typename... args_t>
		vertex_index_t vertex_emplace(args_t&&... args); // TODO: should return iter
		bool vertex_erase(vertex_index_t index);
		bool vertex_erase_simple(vertex_index_t index);

		// accessible through .E
		constexpr std::size_t edge_size() const noexcept;
		constexpr bool edge_empty() const noexcept;
		constexpr std::size_t edge_max_size() const noexcept;

		out_edge_t const* edge_get(vertex_index_t from, vertex_index_t to) const noexcept;
		out_edge_t const* edge_get_simple(vertex_index_t from, vertex_index_t to) const noexcept;

		template<typename... args_t>
		bool edge_emplace(vertex_index_t from, vertex_index_t to, args_t&&... args); // TODO: should return std::pair<iter, bool>
		template<typename... args_t>
		bool edge_emplace_simple(vertex_index_t from, vertex_index_t to, args_t&&... args);
		bool edge_erase(vertex_index_t from, vertex_index_t to);
		bool edge_erase_simple(vertex_index_t from, vertex_index_t to);

	public:
		adjacency_list() noexcept = default;
		explicit adjacency_list(std::size_t vertices);

		using vertex_span_type = detail::vertex_span_t<adjacency_list, base_t>;
		friend vertex_span_type;
		using base_t::V;
		static_assert(std::is_same_v<decltype(V), vertex_span_type>);

		using edge_span_type = detail::edge_span_t<adjacency_list, base_t>;
		friend edge_span_type;
		using base_t::E;
		static_assert(std::is_same_v<decltype(E), edge_span_type>);

		constexpr bool empty() const noexcept;
		constexpr bool edgeless() const noexcept;

		std::size_t in_degree(vertex_index_t index) const noexcept;
		std::size_t out_degree(vertex_index_t index) const noexcept;
		std::size_t degree(vertex_index_t index) const noexcept;

		void reserve(std::size_t vertices);
		std::size_t capacity() const noexcept;

		adjacency_list strip_edges() const&;
		adjacency_list&& strip_edges() &&;

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
