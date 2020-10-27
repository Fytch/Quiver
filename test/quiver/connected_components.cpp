/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch2/catch.hpp>
#include <quiver.hpp>
using namespace quiver;

TEST_CASE("connected_components", "[quiver][fundamentals]")
{
	using graph_t = adjacency_list<undirected, void, cap<int>>;
	graph_t graph;
	const std::size_t N = 10;
	for(std::size_t i = 0; i < N; ++i)
		graph.add_vertex(i);

	graph.add_edge(0, 2);
	graph.add_edge(0, 9);
	graph.add_edge(2, 5);

	graph.add_edge(1, 3);
	graph.add_edge(3, 6);
	graph.add_edge(6, 1);

	graph.add_edge(7, 8);

	CHECK(ccs(graph) == 4);

	auto list = split_ccs(graph);
	REQUIRE(list.size() == 4);

	REQUIRE(list[0].V() == 4);
	CHECK(list[0].get_edge(0, 1));
	CHECK(list[0].get_edge(0, 3));
	CHECK(list[0].get_edge(1, 2));
	CHECK(list[0].vertex(0).capacity == 0);
	CHECK(list[0].vertex(1).capacity == 2);
	CHECK(list[0].vertex(2).capacity == 5);
	CHECK(list[0].vertex(3).capacity == 9);

	REQUIRE(list[1].V() == 3);
	CHECK(list[1].get_edge(0, 1));
	CHECK(list[1].get_edge(0, 2));
	CHECK(list[1].get_edge(1, 2));
	CHECK(list[1].vertex(0).capacity == 1);
	CHECK(list[1].vertex(1).capacity == 3);
	CHECK(list[1].vertex(2).capacity == 6);

	REQUIRE(list[2].V() == 1);
	CHECK(list[2].vertex(0).capacity == 4);

	REQUIRE(list[3].V() == 2);
	CHECK(list[3].get_edge(0, 1));
	CHECK(list[3].vertex(0).capacity == 7);
	CHECK(list[3].vertex(1).capacity == 8);
}
