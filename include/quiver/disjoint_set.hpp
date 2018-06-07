/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_DISJOINT_SET_HPP_INCLUDED
#define QUIVER_DISJOINT_SET_HPP_INCLUDED

// #include <quiver/adjacency_list.hpp>
#include <vector>
#include <numeric>
#include <cassert>

namespace quiver
{
	enum disjoint_set_find_strategy_t
	{
		naive,				// O(n) time
		path_compression,	// O(a(n)) time
		path_halving,		// O(a(n)) time
		path_splitting		// O(a(n)) time
	};

	template<typename derived_t, disjoint_set_find_strategy_t>
	class disjoint_set_find_injector;
	template<typename derived_t>
	class disjoint_set_find_injector<derived_t, naive>
	{
		auto&& derived() const noexcept	{ return static_cast<derived_t const&>(*this); }
		auto&& derived()       noexcept	{ return static_cast<derived_t      &>(*this); }

	public:
		std::size_t find(std::size_t index) const noexcept
		{
			std::size_t next;
			while(index != (next = derived().m_parent[index]))
				index = next;
			return index;
		}
	};
	template<typename derived_t>
	class disjoint_set_find_injector<derived_t, path_compression>
	{
		auto&& derived() const noexcept	{ return static_cast<derived_t const&>(*this); }
		auto&& derived()       noexcept	{ return static_cast<derived_t      &>(*this); }

	public:
		std::size_t find(std::size_t index) noexcept
		{
			const std::size_t start = index;
			for(std::size_t next; index != (next = derived().m_parent[index]); index = next);
			const std::size_t root = index;
			index = start;
			for(std::size_t next; index != (next = derived().m_parent[index]); index = next)
				derived().m_parent[index] = root;
			return root;
		}
	};
	template<typename derived_t>
	class disjoint_set_find_injector<derived_t, path_halving>
	{
		auto&& derived() const noexcept	{ return static_cast<derived_t const&>(*this); }
		auto&& derived()       noexcept	{ return static_cast<derived_t      &>(*this); }

	public:
		std::size_t find(std::size_t index) noexcept
		{
			while(index != derived().m_parent[index])
				index = derived().m_parent[index] = derived().m_parent[derived().m_parent[index]];
			return index;
		}
	};
	template<typename derived_t>
	class disjoint_set_find_injector<derived_t, path_splitting>
	{
		auto&& derived() const noexcept	{ return static_cast<derived_t const&>(*this); }
		auto&& derived()       noexcept	{ return static_cast<derived_t      &>(*this); }

	public:
		std::size_t find(std::size_t index) noexcept
		{
			for(std::size_t next; index != (next = derived().m_parent[index]); index = next)
				derived().m_parent[index] = derived().m_parent[next];
			return index;
		}
	};

	template<
		disjoint_set_find_strategy_t find_strategy_ = path_splitting
	>
	class disjoint_set : public disjoint_set_find_injector<disjoint_set<find_strategy_>, find_strategy_>
	{
		using base_t = disjoint_set_find_injector<disjoint_set<find_strategy_>, find_strategy_>;
		friend base_t;

		std::vector<std::size_t> m_parent;
		std::vector<std::size_t> m_card;
		std::size_t m_sets = 0;

		bool check_size_invariant(std::size_t index)
		{
			const std::size_t card = cardinality(index);
			std::size_t N = 0;
			for(std::size_t i = 0; i < size(); ++i)
				N += same_set(i, index);
			return card == N;
		}

	public:
		static constexpr disjoint_set_find_strategy_t find_strategy = find_strategy_;

		disjoint_set() noexcept = default;
		explicit disjoint_set(std::size_t size)
		{
			reset(size);
		}

		void reset(std::size_t size)
		{
			m_card.resize(size, 1);
			m_parent.resize(size);
			std::iota(m_parent.begin(), m_parent.end(), std::size_t{});
			m_sets = size;
		}

		std::size_t size() const noexcept
		{
			assert(m_parent.size() == m_card.size());
			return m_parent.size();
		}
		std::size_t sets() const noexcept
		{
			return m_sets;
		}

		// std::size_t find(std::size_t index) /* const */ noexcept;
		using base_t::find;

		bool unite(std::size_t a, std::size_t b) noexcept
		{
			// find a's and b's roots
			const std::size_t a_root = find(a);
			const std::size_t b_root = find(b);

			// a and b already belong to the same set
			if(a_root == b_root)
				return false;

			// sort a and b by the cardinality of their respective sets
			const std::size_t min_root = m_card[a_root] < m_card[b_root] ? a_root : b_root;
			const std::size_t max_root = m_card[a_root] < m_card[b_root] ? b_root : a_root;

			// attach the small tree to the large tree
			m_parent[min_root] = max_root;
			m_card[max_root] = m_card[max_root] + m_card[min_root];

			// update the number of sets
			--m_sets;

			// check invariants
			assert(check_size_invariant(a));
			assert(check_size_invariant(b));

			return true;
		}

		std::size_t cardinality(std::size_t index) /* const */ noexcept
		{
			return m_card[find(index)];
		}

		bool same_set(std::size_t a, std::size_t b) /* const */ noexcept
		{
			return a == b || find(a) == find(b);
		}
	};
}

#endif // !QUIVER_DISJOINT_SET_HPP_INCLUDED
