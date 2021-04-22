/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch2/catch.hpp>
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
		CHECK(graph.V.size() == 3);
		CHECK(graph.E.size() == 0);

		CHECK(graph.E.emplace(0, 2, 5, 3.5));
		CHECK(!graph.E.emplace(0, 2, 0, 0.0));
		CHECK(graph.V.size() == 3);
		CHECK(graph.E.size() == 1);

		const auto undirected = graph.to_undirected();

		CHECK(undirected.V.size() == 3);
		CHECK(undirected.E.size() == 1);

		REQUIRE(undirected.E(0, 2) != nullptr);
		CHECK(undirected.E(0, 2)->weight == 5);
		CHECK(undirected.E(0, 2)->capacity == 3.5);

		REQUIRE(undirected.E(2, 0) != nullptr);
		CHECK(undirected.E(2, 0)->weight == 5);
		CHECK(undirected.E(2, 0)->capacity == 3.5);

		CHECK(graph.E.emplace(2, 0, 7, 1.0));
		CHECK(!graph.E.emplace(2, 0, 0, 0.0));
		CHECK(graph.V.size() == 3);
		CHECK(graph.E.size() == 2);

		REQUIRE(graph.E(0, 2) != nullptr);
		CHECK(graph.E(0, 2)->weight == 5);
		CHECK(graph.E(0, 2)->capacity == 3.5);

		REQUIRE(graph.E(2, 0) != nullptr);
		CHECK(graph.E(2, 0)->weight == 7);
		CHECK(graph.E(2, 0)->capacity == 1.0);
	}
	SECTION("undirected")
	{
		using graph_t = adjacency_list<undirected, wt<float>, void>;
		CHECK(is_undirected_v<graph_t> == true);
		CHECK(is_weighted_v<graph_t> == true);
		CHECK(has_capacities_v<graph_t> == false);

		graph_t graph(4);
		CHECK(graph.V.size() == 4);
		CHECK(graph.E.size() == 0);

		CHECK(graph.E.emplace(0, 2, 2.5));
		CHECK(!graph.E.emplace(0, 2, 0.0));
		CHECK(graph.V.size() == 4);
		CHECK(graph.E.size() == 1);

		const auto directed = graph.to_directed();

		CHECK(directed.V.size() == 4);
		CHECK(directed.E.size() == 2);

		REQUIRE(directed.E(0, 2) != nullptr);
		CHECK(directed.E(0, 2)->weight == 2.5);

		REQUIRE(directed.E(2, 0) != nullptr);
		CHECK(directed.E(2, 0)->weight == 2.5);

		CHECK(graph.E.emplace(3, 1, 1.5));
		CHECK(!graph.E.emplace(3, 1, 0.0));
		CHECK(graph.V.size() == 4);
		CHECK(graph.E.size() == 2);

		REQUIRE(graph.E(0, 2) != nullptr);
		REQUIRE(graph.E(2, 0) != nullptr);
		CHECK(graph.E(0, 2)->weight == 2.5);
		CHECK(graph.E(2, 0)->weight == 2.5);

		REQUIRE(graph.E(1, 3) != nullptr);
		REQUIRE(graph.E(3, 1) != nullptr);
		CHECK(graph.E(1, 3)->weight == 1.5);
		CHECK(graph.E(3, 1)->weight == 1.5);

		SECTION("contract")
		{
			graph_t graph(8);
			CHECK(graph.E.emplace(0, 2));
			CHECK(graph.E.emplace(0, 3));
			CHECK(graph.E.emplace(1, 2));
			CHECK(graph.E.emplace(2, 3));
			CHECK(graph.E.emplace(2, 4));
			CHECK(graph.E.emplace(2, 5));
			CHECK(graph.E.emplace(3, 5));
			CHECK(graph.E.emplace(4, 5));
			CHECK(graph.E.emplace(5, 6));
			CHECK(graph.E.emplace(5, 7));

			REQUIRE(graph.V.size() == 8);
			CHECK(graph.E.size() == 10);
			CHECK(graph.contract(2, 5) == true);
			REQUIRE(graph.V.size() == 7);
			CHECK(graph.E.size() == 7);

			CHECK(graph.E(0, 2) != nullptr);
			CHECK(graph.E(0, 3) != nullptr);
			CHECK(graph.E(1, 2) != nullptr);
			CHECK(graph.E(2, 3) != nullptr);
			CHECK(graph.E(2, 4) != nullptr);
			CHECK(graph.E(2, 5) != nullptr);
			CHECK(graph.E(2, 6) != nullptr);
		}
	}
}
