/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch.hpp>
#include <quiver.hpp>
using namespace quiver;

TEST_CASE("disjoint_set", "[quiver][fundamentals]")
{
	const std::size_t N = 10;
	disjoint_set<> set(N);

	CHECK(set.sets() == N);
	for(std::size_t i = 0; i < N; ++i) {
		CHECK(set.find(i) == i);
		CHECK(set.cardinality(i) == 1);
	}

	set.unite(1, 2);
	set.unite(2, 3);
	set.unite(7, 6);
	set.unite(5, 6);

	CHECK(set.sets() == N - 4);

	CHECK(set.cardinality(0) == 1);

	CHECK(set.cardinality(1) == 3);
	CHECK(set.cardinality(2) == 3);
	CHECK(set.cardinality(3) == 3);
	CHECK(set.find(1) == set.find(2));
	CHECK(set.find(1) == set.find(3));

	CHECK(set.cardinality(4) == 1);

	CHECK(set.cardinality(5) == 3);
	CHECK(set.cardinality(6) == 3);
	CHECK(set.cardinality(7) == 3);
	CHECK(set.find(5) == set.find(6));
	CHECK(set.find(5) == set.find(7));

	CHECK(set.cardinality(8) == 1);
	CHECK(set.cardinality(9) == 1);

	set.unite(7, 2);
	set.unite(3, 9);

	CHECK(set.sets() == N - 6);

	CHECK(set.cardinality(0) == 1);
	CHECK(set.cardinality(4) == 1);
	CHECK(set.cardinality(8) == 1);

	CHECK(set.cardinality(1) == 7);
	CHECK(set.cardinality(2) == 7);
	CHECK(set.cardinality(3) == 7);
	CHECK(set.cardinality(5) == 7);
	CHECK(set.cardinality(6) == 7);
	CHECK(set.cardinality(7) == 7);
	CHECK(set.cardinality(9) == 7);
	CHECK(set.find(1) == set.find(2));
	CHECK(set.find(1) == set.find(3));
	CHECK(set.find(1) == set.find(5));
	CHECK(set.find(1) == set.find(6));
	CHECK(set.find(1) == set.find(7));
	CHECK(set.find(1) == set.find(9));
}
