/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	                   Pascal Sommer (p@pascalsommer.ch)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch2/catch.hpp>
#include <quiver.hpp>
using namespace quiver;

TEST_CASE("strongly_connected_components", "[quiver][fundamentals]")
{
	using graph_t = adjacency_list<directed, void, cap<int>>;
	graph_t graph;
	const std::size_t N = 8;
	for(std::size_t i = 0; i < N; ++i)
		graph.V().emplace(i);


	// graph from example on Wikipedia
	// https://en.wikipedia.org/wiki/Tarjan's_strongly_connected_components_algorithm

	// numbering:
	// 0  2  4  6
	// 1  3  5  7

	graph.add_edge(0, 1);
	graph.add_edge(1, 2);
	graph.add_edge(2, 0);

	graph.add_edge(3, 1);
	graph.add_edge(3, 2);
	graph.add_edge(3, 5);
	graph.add_edge(5, 3);
	graph.add_edge(5, 4);

	graph.add_edge(4, 2);
	graph.add_edge(4, 6);
	graph.add_edge(6, 4);

	graph.add_edge(7, 6);
	graph.add_edge(7, 5);

	const auto sccs = tarjan_recursive(graph);

	REQUIRE(sccs.size() == N);

	CHECK(sccs[0] == sccs[1]);
	CHECK(sccs[0] == sccs[2]);

	CHECK(sccs[3] == sccs[5]);

	CHECK(sccs[4] == sccs[6]);

	CHECK(sccs[0] != sccs[3]);
	CHECK(sccs[0] != sccs[4]);
	CHECK(sccs[0] != sccs[7]);

	CHECK(sccs[3] != sccs[4]);
	CHECK(sccs[3] != sccs[7]);

	CHECK(sccs[4] != sccs[7]);

	std::size_t max_scc_index = *std::max_element(sccs.cbegin(), sccs.cend());

	// 4 components
	CHECK(max_scc_index == 3);
}
