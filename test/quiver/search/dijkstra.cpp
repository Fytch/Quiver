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
#include <unordered_map>

TEST_CASE("dijkstra", "[quiver][search]")
{
	SECTION("directed")
	{
		using distance_t = int;

		using graph_t = adjacency_list<directed, wt<distance_t>, void>;
		graph_t graph(5);
		graph.E.emplace(0, 3, distance_t(3));
		graph.E.emplace(1, 2, distance_t(5));
		graph.E.emplace(1, 4, distance_t(1));
		graph.E.emplace(2, 0, distance_t(0));
		graph.E.emplace(3, 2, distance_t(2));
		graph.E.emplace(4, 0, distance_t(6));
		graph.E.emplace(4, 3, distance_t(1));

		std::vector<vertex_index_t> visited;
		std::unordered_map<vertex_index_t, distance_t> distances;
		auto visitor = [&](vertex_index_t index, distance_t distance){
			visited.push_back(index);
			distances[index] = distance;
			return false;
		};
		CHECK(dijkstra(graph, 1, visitor) == false);
		REQUIRE(visited.size() == graph.V.size());
		REQUIRE(visited.size() == distances.size());
		CHECK(visited[0] == 1);
		CHECK(visited[1] == 4);
		CHECK(visited[2] == 3);
		CHECK(visited[3] == 2);
		CHECK(visited[4] == 0);
		CHECK(distances[1] == 0);
		CHECK(distances[4] == 1);
		CHECK(distances[3] == 2);
		CHECK(distances[2] == 4);
		CHECK(distances[0] == 4);

		const auto shortest_path = dijkstra_shortest_path(graph, 1);
		using pair_t = decltype(shortest_path)::value_type;
		REQUIRE(shortest_path.size() == graph.V.size());
		CHECK(shortest_path[0] == pair_t(4, 2));
		CHECK(shortest_path[1] == pair_t(0, 1));
		CHECK(shortest_path[2] == pair_t(4, 3));
		CHECK(shortest_path[3] == pair_t(2, 4));
		CHECK(shortest_path[4] == pair_t(1, 1));
	}
}
