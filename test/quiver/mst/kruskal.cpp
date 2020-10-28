/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch2/catch.hpp>
#include <quiver.hpp>
using namespace quiver;

TEST_CASE("kruskal", "[quiver][mst]")
{
	SECTION("weighted")
	{
		using graph_t = adjacency_list<undirected, wt<int>, void>;
		SECTION("empty")
		{
			graph_t empty(0);
			auto mst = kruskal(empty);
			CHECK(mst.empty());
		}
		SECTION("trivial")
		{
			graph_t trivial(1);
			auto mst = kruskal(trivial);
			CHECK(mst.V.size() == 1);
			CHECK(mst.E() == 0);
		}
		SECTION("1 cc")
		{
			graph_t graph(6);
			graph.add_edge(0, 1, 1);
			graph.add_edge(0, 2, 3);
			graph.add_edge(0, 3, 3);
			graph.add_edge(1, 2, 2);
			graph.add_edge(2, 3, 4);
			graph.add_edge(2, 5, 7);
			graph.add_edge(3, 4, 1);
			graph.add_edge(4, 5, 9);
			REQUIRE(ccs(graph) == 1);

			auto mst = kruskal(graph);
			REQUIRE(mst.V.size() == graph.V.size());
			REQUIRE(mst.E() == graph.V.size() - 1); // because we have 1 CC
			CHECK(mst.get_edge(0, 1) != nullptr);
			CHECK(mst.get_edge(0, 2) == nullptr);
			CHECK(mst.get_edge(0, 3) != nullptr);
			CHECK(mst.get_edge(1, 2) != nullptr);
			CHECK(mst.get_edge(2, 3) == nullptr);
			CHECK(mst.get_edge(2, 5) != nullptr);
			CHECK(mst.get_edge(3, 4) != nullptr);
			CHECK(mst.get_edge(4, 5) == nullptr);
		}
		SECTION("2 cc")
		{
			graph_t graph(6);
			graph.add_edge(0, 1, 1);
			graph.add_edge(0, 2, 3);
			graph.add_edge(0, 3, 3);
			graph.add_edge(1, 2, 2);
			graph.add_edge(2, 3, 4);
			graph.add_edge(4, 5, 9);
			REQUIRE(ccs(graph) == 2);

			auto mst = kruskal(graph);
			REQUIRE(mst.V.size() == graph.V.size());
			REQUIRE(mst.E() == graph.V.size() - 2); // because we have 2 CCs
			CHECK(mst.get_edge(0, 1) != nullptr);
			CHECK(mst.get_edge(0, 2) == nullptr);
			CHECK(mst.get_edge(0, 3) != nullptr);
			CHECK(mst.get_edge(1, 2) != nullptr);
			CHECK(mst.get_edge(2, 3) == nullptr);
			CHECK(mst.get_edge(4, 5) != nullptr);
		}
	}
	SECTION("unweighted")
	{
		using graph_t = adjacency_list<undirected, void, void>;
		SECTION("empty")
		{
			graph_t empty(0);
			auto mst = kruskal(empty);
			CHECK(mst.empty());
		}
		SECTION("trivial (0 edges)")
		{
			graph_t trivial(1);
			auto mst = kruskal(trivial);
			CHECK(mst.V.size() == 1);
			CHECK(mst.E() == 0);
		}
		SECTION("1 cc")
		{
			graph_t graph(6);
			graph.add_edge(0, 1);
			graph.add_edge(0, 2);
			graph.add_edge(0, 3);
			graph.add_edge(1, 2);
			graph.add_edge(2, 3);
			graph.add_edge(2, 5);
			graph.add_edge(3, 4);
			graph.add_edge(4, 5);
			REQUIRE(ccs(graph) == 1);

			auto mst = kruskal(graph);
			REQUIRE(mst.V.size() == graph.V.size());
			REQUIRE(mst.E() == graph.V.size() - 1); // because we have 1 CC
		}
		SECTION("2 cc")
		{
			graph_t graph(6);
			graph.add_edge(0, 1);
			graph.add_edge(0, 2);
			graph.add_edge(0, 3);
			graph.add_edge(1, 2);
			graph.add_edge(2, 3);
			graph.add_edge(4, 5);
			REQUIRE(ccs(graph) == 2);

			auto mst = kruskal(graph);
			REQUIRE(mst.V.size() == graph.V.size());
			REQUIRE(mst.E() == graph.V.size() - 2); // because we have 2 CCs
		}
	}
}
