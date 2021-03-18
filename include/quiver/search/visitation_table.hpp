/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_SEARCH_VISITATION_TABLE_HPP_INCLUDED
#define QUIVER_SEARCH_VISITATION_TABLE_HPP_INCLUDED

#include <quiver/typedefs.hpp>
#include <vector>
#include <cassert>

namespace quiver
{
	class visitation_table_t
	{
		std::vector<bool> m_visited;

	public:
		[[nodiscard]] explicit visitation_table_t(std::size_t n)
		: m_visited(n, false)
		{
		}
		template<typename graph_t>
		[[nodiscard]] explicit visitation_table_t(graph_t const& graph)
		: visitation_table_t(graph.V.size())
		{
		}

		visitation_table_t(visitation_table_t const& rhs) = delete;
		visitation_table_t(visitation_table_t&& rhs) = delete;

		void set(vertex_index_t index) noexcept
		{
			assert(index < m_visited.size());
			m_visited[index] = true;
		}
		[[nodiscard]] bool operator()(vertex_index_t index) const noexcept
		{
			assert(index < m_visited.size());
			return m_visited[index];
		}

		template<typename visitor_t>
		[[nodiscard]] auto hook_visitor(visitor_t& visitor) noexcept
		{
			return [this, &visitor](vertex_index_t index, auto&& weight) -> bool {
				this->set(index);
				return visitor(index, std::forward<decltype(weight)>(weight));
			};
		}
	};
}

#endif // !QUIVER_SEARCH_VISITATION_TABLE_HPP_INCLUDED
