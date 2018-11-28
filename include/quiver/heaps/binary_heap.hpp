/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_HEAPS_BINARY_HEAP_HPP_INCLUDED
#define QUIVER_HEAPS_BINARY_HEAP_HPP_INCLUDED

#include <functional>
#include <vector>
#include <cassert>
#include <utility>
#include <iterator>

namespace quiver
{
	template<typename T, typename comparator_t = std::less<>, typename container_t = std::vector<T>>
	class binary_heap
	{
	public:
		using value_type = T;
		using comparator_type = comparator_t;
		using container_type = container_t;

	private:
		container_type m_container;
		comparator_type m_comparator;

		static constexpr std::size_t up(std::size_t index) noexcept		{ return (index - 1) / 2; }
		static constexpr std::size_t down(std::size_t index) noexcept	{ return 2 * index + 1; }

		std::size_t sift_up(std::size_t index)
		{
			assert(index < m_container.size());
			while(index != 0) {
				const std::size_t parent = up(index);
				if(m_comparator(m_container[index], m_container[parent])) {
					using std::swap;
					swap(m_container[index], m_container[parent]);
					index = parent;
				} else {
					break;
				}
			}
			return index;
		}
		std::size_t sift_down(std::size_t index)
		{
			assert(index < m_container.size());
			for(const std::size_t size = m_container.size();;) {
				std::size_t child = down(index);
				if(child >= size)
					break;
				if(child + 1 < size && m_comparator(m_container[child + 1], m_container[child]))
					++child;
				if(m_comparator(m_container[child], m_container[index])) {
					using std::swap;
					swap(m_container[child], m_container[index]);
					index = child;
				} else {
					break;
				}
			}
			return index;
		}

		bool is_heap() const
		{
			for(std::size_t i = 0;;) {
				const std::size_t child = down(i);
				if(child > m_container.size())
					break;
				if(m_comparator(m_container[child], m_container[i]))
					return false;
				i = child;
			}
			return true;
		}
		void make_heap()
		{
			for(std::size_t i = size() / 2; i--;)
				sift_down(i);
			assert(is_heap());
		}

	public:
		binary_heap()
		{
		}
		explicit binary_heap(comparator_t comparator)
		: m_comparator(std::move(comparator))
		{
		}
		// TODO: ctors
		// TODO: assign or make_heap

		void reserve(std::size_t capacity)
		{
			m_container.reserve(capacity);
		}
		std::size_t capacity() const noexcept
		{
			return m_container.capacity();
		}

		// void resize(std::size_t size)
		std::size_t size() const noexcept
		{
			return m_container.size();
		}
		bool empty() const noexcept
		{
			return size() == 0;
		}
		void clear()
		{
			m_container.clear();
		}

		void merge(binary_heap const& rhs)
		{
			reserve(size() + rhs.size());
			m_container.insert(m_container.end(), rhs.m_container.begin(), rhs.m_container.end());
			make_heap();
		}
		// a.k.a. 'meld'
		void merge(binary_heap&& rhs)
		{
			if(capacity() < rhs.capacity()) {
				using std::swap;
				swap(m_container, rhs.m_container);
			}
			reserve(size() + rhs.size());
			m_container.insert(m_container.end(), std::make_move_iterator(rhs.m_container.begin()), std::make_move_iterator(rhs.m_container.end()));
			make_heap();
		}

		value_type const& get(std::size_t index) const noexcept
		{
			assert(index < m_container.size());
			return m_container[index];
		}

		// a.k.a. 'find_min'
		value_type const& top() const noexcept
		{
			assert(!empty());
			return m_container.front();
		}
		// a.k.a. 'insert' or 'emplace'
		template<typename... args_t>
		std::size_t push(args_t&&... args)
		{
			m_container.emplace_back(std::forward<args_t>(args)...);
			return sift_up(m_container.size() - 1);
		}
		void erase(std::size_t index)
		{
			assert(index < m_container.size());
			value_type replacement(std::move(m_container.back()));
			m_container.pop_back();
			update(index, std::move(replacement));
		}
		// a.k.a. 'remove_min'
		void pop()
		{
			assert(!empty());
			if(size() > 1) {
				using std::swap;
				swap(m_container.front(), m_container.back());
				m_container.pop_back();
				sift_down(0);
			} else {
				m_container.pop_back();
			}
		}
		// a.k.a. 'extract_min'
		value_type extract_top()
		{
			assert(!empty());
			value_type result(std::move(m_container.front()));
			pop();
			return result;
		}
		// a.k.a. 'replace_min'
		std::size_t exchange_top(value_type replacement)
		{
			assert(!empty());
			m_container.front() = std::move(replacement);
			return sift_down(0);
		}

		std::size_t update(std::size_t index, value_type replacement)
		{
			assert(index < m_container.size());
			const bool cmp = m_comparator(m_container[index], replacement);
			m_container[index] = std::move(replacement);
			if(cmp)
				return sift_down(index);
			else
				return sift_up(index);
		}
		// move down in the heap
		std::size_t increase(std::size_t index, value_type replacement)
		{
			assert(index < m_container.size());
			assert(!m_comparator(replacement, m_container[index]));
			m_container[index] = std::move(replacement);
			return sift_down(index);
		}
		// a.k.a. 'decrease_key'; move up in the heap
		std::size_t decrease(std::size_t index, value_type replacement)
		{
			assert(index < m_container.size());
			assert(!m_comparator(m_container[index], replacement));
			m_container[index] = std::move(replacement);
			return sift_up(index);
		}

		comparator_type const& comparator() const noexcept
		{
			return m_comparator;
		}

		void swap(binary_heap& rhs) noexcept
		{
			using std::swap;
			swap(m_comparator, rhs.m_comparator);
			swap(m_container, rhs.m_container);
		}
	};

	template<typename T, typename comparator_t, typename container_t>
	void swap(binary_heap<T, comparator_t, container_t>& lhs, binary_heap<T, comparator_t, container_t>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	template<typename T, typename container_t = std::vector<T>>
	using min_heap = binary_heap<T, std::less<>, container_t>;
	template<typename T, typename container_t = std::vector<T>>
	using max_heap = binary_heap<T, std::greater<>, container_t>;
}

#endif // !QUIVER_HEAPS_BINARY_HEAP_HPP_INCLUDED
