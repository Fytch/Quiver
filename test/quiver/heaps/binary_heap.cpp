/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch2/catch.hpp>
#include <quiver.hpp>
using namespace quiver;

TEST_CASE("binary_heap", "[quiver][heaps]")
{
	min_heap<int> heap;
	heap.reserve(3);
	heap.push(2);
	heap.push(0);
	heap.push(4);
	CHECK(heap.capacity() == 3);
	CHECK(heap.size() == 3);
	CHECK(heap.top() == 0);
	CHECK(heap.extract_top() == 0);
	CHECK(heap.size() == 2);
	CHECK(heap.top() == 2);

	min_heap<int> heap2;
	heap2.push(1);
	heap2.push(2);
	heap2.push(5);
	REQUIRE(heap2.size() == 3);

	SECTION("merge")
	{
		SECTION("copy 1")
		{
			heap.merge(heap2);
			CHECK(heap.extract_top() == 1);
			CHECK(heap.extract_top() == 2);
			CHECK(heap.extract_top() == 2);
			CHECK(heap.extract_top() == 4);
			CHECK(heap.extract_top() == 5);
			CHECK(heap.empty());
		}
		SECTION("copy 2")
		{
			heap2.merge(heap);
			CHECK(heap2.extract_top() == 1);
			CHECK(heap2.extract_top() == 2);
			CHECK(heap2.extract_top() == 2);
			CHECK(heap2.extract_top() == 4);
			CHECK(heap2.extract_top() == 5);
			CHECK(heap2.empty());
		}
		SECTION("move 1")
		{
			heap.merge(std::move(heap2));
			CHECK(heap.extract_top() == 1);
			CHECK(heap.extract_top() == 2);
			CHECK(heap.extract_top() == 2);
			CHECK(heap.extract_top() == 4);
			CHECK(heap.extract_top() == 5);
			CHECK(heap.empty());
		}
		SECTION("move 2")
		{
			heap2.merge(std::move(heap));
			CHECK(heap2.extract_top() == 1);
			CHECK(heap2.extract_top() == 2);
			CHECK(heap2.extract_top() == 2);
			CHECK(heap2.extract_top() == 4);
			CHECK(heap2.extract_top() == 5);
			CHECK(heap2.empty());
		}
	}

	SECTION("exchange_top")
	{
		SECTION("case 1")
		{
			heap2.exchange_top(1);
			CHECK(heap2.extract_top() == 1);
			CHECK(heap2.extract_top() == 2);
			CHECK(heap2.extract_top() == 5);
			CHECK(heap2.empty());
		}
		SECTION("case 2")
		{
			heap2.exchange_top(3);
			CHECK(heap2.extract_top() == 2);
			CHECK(heap2.extract_top() == 3);
			CHECK(heap2.extract_top() == 5);
			CHECK(heap2.empty());
		}
		SECTION("case 3")
		{
			heap2.exchange_top(7);
			CHECK(heap2.extract_top() == 2);
			CHECK(heap2.extract_top() == 5);
			CHECK(heap2.extract_top() == 7);
			CHECK(heap2.empty());
		}
	}

	SECTION("update")
	{
		REQUIRE(heap2.get(1) == 2);
		heap2.update(1, -1);
		CHECK(heap2.extract_top() == -1);
		CHECK(heap2.extract_top() == 1);
		CHECK(heap2.extract_top() == 5);
		CHECK(heap2.empty());
	}
}
