/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch2/catch.hpp>
#include <quiver.hpp>
using namespace quiver;
#include <vector>

TEST_CASE("dfs", "[quiver][search]")
{
	SECTION("directed")
	{
		using graph_t = adjacency_list<directed, void, void>;
		graph_t graph(5);
		graph.add_edge(0, 1);
		graph.add_edge(1, 3);
		graph.add_edge(1, 2);
		graph.add_edge(2, 0);
		graph.add_edge(2, 4);
		graph.add_edge(3, 4);
		graph.add_edge(4, 1);

		std::vector<vertex_index_t> visited;
		auto visitor = [&visited](auto const& vertex){ visited.push_back(vertex); return false; };
		CHECK(dfs(graph, 1, visitor) == false);
		REQUIRE(visited.size() == graph.V());
		CHECK(visited[0] == 1);
		CHECK(visited[1] == 3);
		CHECK(visited[2] == 4);
		CHECK(visited[3] == 2);
		CHECK(visited[4] == 0);
	}
}
