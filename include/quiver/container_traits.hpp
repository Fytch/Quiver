/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_CONTAINER_TRAITS_HPP_INCLUDED
#define QUIVER_CONTAINER_TRAITS_HPP_INCLUDED

#include <vector>
#include <list>
#include <type_traits>
#include <memory>
#include <iterator>
#include <algorithm>

namespace quiver
{
	template<typename T>
	using vector = std::vector<T>;
	template<typename T>
	using list = std::list<T>;

	template<typename T>
	inline constexpr bool is_sane_container =
		std::is_nothrow_constructible_v<T> &&
		std::is_nothrow_move_constructible_v<T> &&
		std::is_nothrow_move_assignable_v<T> &&
		std::is_nothrow_swappable_v<T>;

	template<typename T>
	struct container_traits
	{
		static constexpr bool is_specialized = false;
	};
	template<typename T, typename allocator_t>
	struct container_traits<std::vector<T, allocator_t>>
	{
		static constexpr bool is_specialized = true;
		static constexpr bool is_ordered = true;

		using container_type = std::vector<T, allocator_t>;
		using value_type = T;
		template<typename U>
		using rebind = std::vector<U, typename std::allocator_traits<allocator_t>::template rebind_alloc<U>>;

		template<typename comparator_t>
		void sort(container_type& container, comparator_t comparator)
		{
			using std::begin, std::end;
			std::sort(begin(container), end(container), comparator);
		}

		// TODO: uniform interface for more operation
	};
	template<typename T, typename allocator_t>
	struct container_traits<std::list<T, allocator_t>>
	{
		static constexpr bool is_specialized = true;
		static constexpr bool is_ordered = true;

		using container_type = std::list<T, allocator_t>;
		using value_type = T;
		template<typename U>
		using rebind = std::list<U, typename std::allocator_traits<allocator_t>::template rebind_alloc<U>>;

		template<typename comparator_t>
		void sort(container_type& container, comparator_t comparator)
		{
			container.sort(comparator);
		}
	};
}

#endif // !QUIVER_CONTAINER_TRAITS_HPP_INCLUDED
