/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch2/catch.hpp>
#include <quiver.hpp>
using namespace quiver;

TEST_CASE("complete", "[quiver][families]")
{
	for(std::size_t n = 0; n < 20; ++n) {
		const auto K = complete(n);
		CHECK(K.V.size() == n);
		CHECK(K.E() == n * (n - 1) / 2);
	}
}
