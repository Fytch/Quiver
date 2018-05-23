/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch.hpp>
#include <quiver.hpp>
using namespace quiver;

TEST_CASE("adjacency_list", "[quiver][fundamentals]")
{
	SECTION("directed")
	{
		using graph_t = adjacency_list<cmb<wt<int>, cap<float>>, void, directed>;
		CHECK(is_weighted_v<graph_t> == true);
		CHECK(has_capacities_v<graph_t> == true);

		graph_t graph(3);
		CHECK(graph.V() == 3);
		CHECK(graph.E() == 0);

		graph.add_edge(0, 2, 5, 3.5);
		CHECK(graph.V() == 3);
		CHECK(graph.E() == 1);

		REQUIRE(graph.get_edge(0, 2) != nullptr);
		CHECK(graph.get_edge(0, 2)->weight == 5);
		CHECK(graph.get_edge(0, 2)->capacity == 3.5);
	}
	/*
	SECTION("undirected")
	{
		using graph_t = adjacency_list<wt<float>, void, undirected>;
		CHECK(is_weighted_v<graph_t> == true);
		CHECK(has_capacities_v<graph_t> == false);

		graph_t graph(4);
		CHECK(graph.V() == 4);
		CHECK(graph.E() == 0);

		graph.add_edge(0, 2, 2.5);
		CHECK(graph.V() == 4);
		CHECK(graph.E() == 1);

		REQUIRE(graph.get_edge(0, 2) != nullptr);
		REQUIRE(graph.get_edge(2, 0) != nullptr);
		CHECK(graph.get_edge(0, 2)->weight == 2.5);
		CHECK(graph.get_edge(2, 0)->weight == 2.5);
	}
	*/
}
