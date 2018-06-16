/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_ADJACENCYLIST_HPP_INCLUDED
#define QUIVER_ADJACENCYLIST_HPP_INCLUDED

#include <quiver/util.hpp>
#include <quiver/properties.hpp>
#include <quiver/container_interface.hpp>
#include <utility>
#include <vector>
#include <cassert>
#include <algorithm>

namespace quiver
{
	using vertex_index_t = std::size_t;

	enum directivity_t
	{
		directed,
		undirected,
		// bidirected,
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

	// no multiedges
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

		static constexpr void normalize(vertex_index_t& from, vertex_index_t& to) noexcept
		{
			if(from > to)
				std::swap(from, to);
		}

		template<typename... args_t>
		bool add_edge_simple(vertex_index_t from, vertex_index_t to, args_t&&... args)
		{
			assert(from < V());
			assert(to < V());
			assert(get_edge_simple(from, to) == nullptr);

			m_vertices[from].out_edges.emplace_back(to, std::forward<args_t>(args)...);
			++m_e;
			return true;
		}
		bool remove_edge_simple(vertex_index_t from, vertex_index_t to)
		{
			assert(from < V());
			assert(to < V());

			auto& edges = m_vertices[from].out_edges;
			for(auto iter = edges.begin(); iter != edges.end(); ++iter)
			{
				if(iter->to == to)
				{
					edges.erase(iter);
					--m_e;
					return true;
				}
			}
			return false;
		}
		out_edge_t const* get_edge_simple(vertex_index_t from, vertex_index_t to) const noexcept
		{
			assert(from < V());
			assert(to < V());

			auto& edges = m_vertices[from].out_edges;
			for(auto iter = edges.begin(); iter != edges.end(); ++iter)
			{
				if(iter->to == to)
					return &*iter;
			}
			return nullptr;
		}

	public:
		adjacency_list() noexcept
		{
		}
		explicit adjacency_list(std::size_t vertices)
		: m_v{ vertices }, m_vertices(vertices)
		{
		}

		constexpr std::size_t V() const noexcept
		{
			return m_v;
		}
		constexpr std::size_t E() const noexcept
		{
			if constexpr(directivity == directed)
				return m_e;
			else if constexpr(directivity == undirected)
				return m_e / 2;
		}
		constexpr std::size_t max_edges() const noexcept
		{
			if constexpr(directivity == directed)
				return sq(V());
			else if constexpr(directivity == undirected)
				return V() * (V() - 1) / 2;
		}
		constexpr bool empty() const noexcept
		{
			return V() == 0;
		}
		constexpr bool edgeless() const noexcept
		{
			return E() == 0;
		}

		vertex_t const& vertex(vertex_index_t index) const noexcept
		{
			assert(index < V());
			return m_vertices[index];
		}
		vertex_t& vertex(vertex_index_t index) noexcept
		{
			return const_cast<vertex_t&>(static_cast<adjacency_list const&>(*this).vertex(index));
		}

		std::size_t in_degree(vertex_index_t index) const noexcept
		{
			if constexpr(directivity == directed) {
				std::size_t count = 0;
				for(auto const& vertex : m_vertices)
					count += vertex.has_edge_to(index);
				return count;
			} else if constexpr(directivity == undirected) {
				return out_degree(index);
			}
		}
		std::size_t out_degree(vertex_index_t index) const noexcept
		{
			return vertex(index).out_degree();
		}
		std::size_t degree(vertex_index_t index) const noexcept
		{
			static_assert(directivity == undirected, "degree only defined for undirected graphs");
			return out_degree(index);
		}

		// TODO: should return iter
		template<typename... args_t>
		vertex_index_t add_vertex(args_t&&... args)
		{
			m_vertices.emplace_back(std::forward<args_t>(args)...);
			++m_v;
			return m_v - 1;
		}
		bool remove_vertex(vertex_index_t index)
		{
			assert(index < V());

			const std::size_t out_degree = m_vertices[index].out_degree();
			m_vertices.erase(m_vertices.begin() + index);
			m_e -= out_degree;
			--m_v;
			for(auto& vertex : m_vertices)
			{
				for(std::size_t i = vertex.out_edges.size(); i-- > 0;)
				{
					if(vertex.out_edges[i].to == index)
					{
						vertex.out_edges.erase(vertex.out_edges.begin() + i);
						++i;
						--m_e;
					}
					else if(vertex.out_edges[i].to > index)
					{
						--vertex.out_edges[i].to;
					}
				}
			}
			return true;
		}

		void reserve(std::size_t vertices)
		{
			m_vertices.reserve(vertices);
		}
		std::size_t capacity() const noexcept(noexcept(m_vertices.capacity()))
		{
			return m_vertices.capacity();
		}

		// TODO: should return std::pair<iter, bool>
		template<typename... args_t>
		bool add_edge(vertex_index_t from, vertex_index_t to, args_t&&... args)
		{
			assert(from < V());
			assert(to < V());
			// assert(the edge (from, to) doesn't already exist);

			if constexpr(directivity == directed) {
				add_edge_simple(from, to, std::forward<args_t>(args)...);
			} else if constexpr(directivity == undirected) {
				if(from != to) {
					add_edge_simple(to, from, std::as_const(args)...);
					add_edge_simple(from, to, std::forward<args_t>(args)...);
					// TODO: strong exception safety
				} else {
					add_edge_simple(from, to, std::forward<args_t>(args)...);
				}
			}
			return true;
		}
		bool remove_edge(vertex_index_t from, vertex_index_t to)
		{
			assert(from < V());
			assert(to < V());

			if constexpr(directivity == directed) {
				return remove_edge_simple(from, to);
			} else if constexpr(directivity == undirected) {
				if(from == to)
					return remove_edge_simple(from, to);

				assert((get_edge_simple(from, to) == nullptr) == (get_edge_simple(to, from) == nullptr));
				const bool removed = remove_edge_simple(from, to);
				if(removed)
					remove_edge_simple(from, to); // TODO: strong exception safety
				return removed;
			}
		}

		auto v_begin()
		{
			return m_vertices.begin();
		}
		auto v_begin() const
		{
			return m_vertices.begin();
		}
		auto v_cbegin() const
		{
			return m_vertices.cbegin();
		}
		auto v_end()
		{
			return m_vertices.end();
		}
		auto v_end() const
		{
			return m_vertices.end();
		}
		auto v_cend() const
		{
			return m_vertices.cend();
		}

		// returns a graph with just the vertices
		adjacency_list vertices() const
		{
			adjacency_list result;
			result.m_vertices.reserve(V());
			for(auto const& vertex : m_vertices)
				result.add_vertex(vertex.properties());
			return result;
		}

		out_edge_t const* get_edge(vertex_index_t from, vertex_index_t to) const noexcept
		{
			assert(from < V());
			assert(to < V());

			if constexpr(directivity == undirected)
				normalize(from, to); // so that get_edge(1,0) == get_edge(0,1)
			return get_edge_simple(from, to);
		}

		template<typename invokable_t>
		void transform_outs(invokable_t invokable)
		{
			for(auto& vertex : m_vertices)
				for(auto& out_edge : vertex.out_edges)
					out_edge.to = invokable(out_edge.to);
		}

		void swap(adjacency_list& rhs) noexcept
		{
			using std::swap;
			swap(m_vertices, rhs.m_vertices);
			swap(m_v, rhs.m_v);
			swap(m_e, rhs.m_e);
		}

		static constexpr bool is_weighted() noexcept
		{
			return is_weighted_v<adjacency_list>;
		}
	};
}

#endif // !QUIVER_ADJACENCYLIST_HPP_INCLUDED
