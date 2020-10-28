/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch2/catch.hpp>
#include <quiver.hpp>
using namespace quiver;
#include <algorithm>

TEST_CASE("toposort", "[quiver]")
{
	SECTION("0  1  2")
	{
		adjacency_list<directed> graph(3);

		const auto sorting = toposort(graph);
		REQUIRE(sorting.size() == graph.V.size());
		{
			auto sorted_sorting = sorting;
			std::sort(sorted_sorting.begin(), sorted_sorting.end());
			for(std::size_t i = 0; i < graph.V.size(); ++i)
				CHECK(sorted_sorting[i] == i);
		}
	}
	SECTION("0->1  2")
	{
		adjacency_list<directed> graph(3);
		graph.add_edge(0, 1);

		const auto sorting = toposort(graph);
		REQUIRE(sorting.size() == graph.V.size());
		{
			auto sorted_sorting = sorting;
			std::sort(sorted_sorting.begin(), sorted_sorting.end());
			for(std::size_t i = 0; i < graph.V.size(); ++i)
				CHECK(sorted_sorting[i] == i);
		}
		CHECK(std::find(sorting.begin(), sorting.end(), 0) < std::find(sorting.begin(), sorting.end(), 1));
	}
	SECTION("0->1->2")
	{
		adjacency_list<directed> graph(3);
		graph.add_edge(0, 1);
		graph.add_edge(1, 2);

		const auto sorting = toposort(graph);
		REQUIRE(sorting.size() == graph.V.size());
		{
			auto sorted_sorting = sorting;
			std::sort(sorted_sorting.begin(), sorted_sorting.end());
			for(std::size_t i = 0; i < graph.V.size(); ++i)
				CHECK(sorted_sorting[i] == i);
		}
		CHECK(std::find(sorting.begin(), sorting.end(), 0) < std::find(sorting.begin(), sorting.end(), 1));
		CHECK(std::find(sorting.begin(), sorting.end(), 1) < std::find(sorting.begin(), sorting.end(), 2));
	}
	SECTION("0->1->2, 0->2")
	{
		adjacency_list<directed> graph(3);
		graph.add_edge(0, 1);
		graph.add_edge(0, 2);
		graph.add_edge(1, 2);

		const auto sorting = toposort(graph);
		REQUIRE(sorting.size() == graph.V.size());
		{
			auto sorted_sorting = sorting;
			std::sort(sorted_sorting.begin(), sorted_sorting.end());
			for(std::size_t i = 0; i < graph.V.size(); ++i)
				CHECK(sorted_sorting[i] == i);
		}
		CHECK(std::find(sorting.begin(), sorting.end(), 0) < std::find(sorting.begin(), sorting.end(), 1));
		CHECK(std::find(sorting.begin(), sorting.end(), 1) < std::find(sorting.begin(), sorting.end(), 2));
	}
	SECTION("cyclic")
	{
		adjacency_list<directed> graph(3);
		graph.add_edge(0, 1);
		graph.add_edge(1, 2);
		graph.add_edge(2, 0);

		CHECK_THROWS_AS(toposort(graph), cyclic_graph);
	}
}
