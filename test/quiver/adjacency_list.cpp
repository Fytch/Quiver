/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch.hpp>
#include <quiver.hpp>
using namespace quiver;

TEST_CASE("adjacency_list", "[quiver]")
{
	SECTION("directed")
	{
		using graph_t = adjacency_list<directed, cmb<wt<int>, cap<float>>, void>;
		CHECK(is_directed_v<graph_t> == true);
		CHECK(is_weighted_v<graph_t> == true);
		CHECK(has_capacities_v<graph_t> == true);

		graph_t graph(3);
		CHECK(graph.V() == 3);
		CHECK(graph.E() == 0);

		graph.add_edge(0, 2, 5, 3.5);
		CHECK(graph.V() == 3);
		CHECK(graph.E() == 1);

		graph.add_edge(2, 0, 7, 1.0);
		CHECK(graph.V() == 3);
		CHECK(graph.E() == 2);

		REQUIRE(graph.get_edge(0, 2) != nullptr);
		CHECK(graph.get_edge(0, 2)->weight == 5);
		CHECK(graph.get_edge(0, 2)->capacity == 3.5);

		REQUIRE(graph.get_edge(2, 0) != nullptr);
		CHECK(graph.get_edge(2, 0)->weight == 7);
		CHECK(graph.get_edge(2, 0)->capacity == 1.0);
	}
	SECTION("undirected")
	{
		using graph_t = adjacency_list<undirected, wt<float>, void>;
		CHECK(is_undirected_v<graph_t> == true);
		CHECK(is_weighted_v<graph_t> == true);
		CHECK(has_capacities_v<graph_t> == false);

		graph_t graph(4);
		CHECK(graph.V() == 4);
		CHECK(graph.E() == 0);

		graph.add_edge(0, 2, 2.5);
		CHECK(graph.V() == 4);
		CHECK(graph.E() == 1);

		graph.add_edge(3, 1, 1.5);
		CHECK(graph.V() == 4);
		CHECK(graph.E() == 2);

		REQUIRE(graph.get_edge(0, 2) != nullptr);
		REQUIRE(graph.get_edge(2, 0) != nullptr);
		CHECK(graph.get_edge(0, 2)->weight == 2.5);
		CHECK(graph.get_edge(2, 0)->weight == 2.5);

		REQUIRE(graph.get_edge(1, 3) != nullptr);
		REQUIRE(graph.get_edge(3, 1) != nullptr);
		CHECK(graph.get_edge(1, 3)->weight == 1.5);
		CHECK(graph.get_edge(3, 1)->weight == 1.5);
	}
}
