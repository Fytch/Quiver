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

TEST_CASE("bfs", "[quiver][search]")
{
	SECTION("directed")
	{
		using graph_t = adjacency_list<directed, void, void>;
		graph_t graph(5);
		graph.E.emplace(0, 1);
		graph.E.emplace(1, 3);
		graph.E.emplace(1, 2);
		graph.E.emplace(2, 0);
		graph.E.emplace(2, 4);
		graph.E.emplace(3, 4);
		graph.E.emplace(4, 1);

		std::vector<vertex_index_t> visited;
		auto visitor = [&visited](vertex_index_t index){
			visited.push_back(index);
			return false;
		};
		CHECK(bfs(graph, 1, visitor) == false);
		REQUIRE(visited.size() == graph.V.size());
		CHECK(visited[0] == 1);
		CHECK(visited[1] == 3);
		CHECK(visited[2] == 2);
		CHECK(visited[3] == 4);
		CHECK(visited[4] == 0);

		const auto shortest_path = bfs_shortest_path(graph, 1);
		using pair_t = decltype(shortest_path)::value_type;
		REQUIRE(shortest_path.size() == graph.V.size());
		CHECK(shortest_path[0] == pair_t(2, 2));
		CHECK(shortest_path[1] == pair_t(0, 1));
		CHECK(shortest_path[2] == pair_t(1, 1));
		CHECK(shortest_path[3] == pair_t(1, 1));
		CHECK((shortest_path[4] == pair_t(2, 2) || shortest_path[4] == pair_t(2, 3)));
	}
}
