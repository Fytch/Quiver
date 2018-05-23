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

namespace quiver
{
	using vertex_index_t = std::size_t;

	enum directivity_t
	{
		directed,
		// undirected,
		// bidirected,
	};

	// no multiedges, no loops
	template<
		typename edge_properties_t = void,
		typename vertex_properties_t = void,
		directivity_t dir = directed,
		template<typename> class out_edge_container = vector,
		template<typename> class vertex_container = vector
	>
	class adjacency_list
	{
	public:
		struct out_edge_t : public void2empty<edge_properties_t>
		{
			using base_t = void2empty<edge_properties_t>;
			vertex_index_t to;

			template<typename... args_t>
			out_edge_t(vertex_index_t to, args_t&&... args) noexcept(std::is_nothrow_constructible_v<base_t, args_t...>)
			: base_t(std::forward<args_t>(args)...), to{ to }
			{
			}
		};

		using out_edge_list_t = out_edge_container<out_edge_t>;
		static_assert(is_sane_container<out_edge_list_t>, "out_edge_list_t must be sane container");

		struct vertex_t : public void2empty<vertex_properties_t>
		{
			using base_t = void2empty<vertex_properties_t>;
			out_edge_list_t out_edges;

			using base_t::base_t;
		};

		using vertices_t = vertex_container<vertex_t>;
		static_assert(is_sane_container<vertices_t>, "vertices_t must be sane container");

		inline static constexpr directivity_t directivity = dir;

	private:
		std::size_t m_v = 0, m_e = 0;
		vertices_t m_vertices;

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
			return m_e;
		}
		constexpr std::size_t max_edges() const noexcept
		{
			if constexpr(directivity == directed)
				return sq(V());
			// else if constexpr(directivity == undirected)
			// 	return V() * (V() - 1) / 2;
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

		// TODO: should return iter
		template<typename... args_t>
		vertex_index_t add_vertex(args_t&&... args)
		{
			m_vertices.emplace_back(std::forward<args_t>(args)...);
			++m_v;
		}
		bool remove_vertex(vertex_index_t index)
		{
			assert(index < V());

			const std::size_t out_edges = m_vertices[index].out_edges.size();
			m_vertices.erase(m_vertices.begin() + index);
			m_e -= out_edges;
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

		// TODO: should return std::pair<iter, bool>
		template<typename... args_t>
		bool add_edge(vertex_index_t from, vertex_index_t to, args_t&&... args)
		{
			assert(from < V());
			assert(to < V());
			// assert(the edge (from, to) doesn't already exist);

			m_vertices[from].out_edges.emplace_back(to, std::forward<args_t>(args)...);
			++m_e;
			return true;
		}
		bool remove_edge(vertex_index_t from, vertex_index_t to)
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

		out_edge_t const* get_edge(vertex_index_t from, vertex_index_t to) const noexcept
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
