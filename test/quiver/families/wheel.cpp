/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <catch2/catch.hpp>
#include <quiver.hpp>
using namespace quiver;

TEST_CASE("wheel", "[quiver][families]")
{
	for(std::size_t n = 4; n < 20; ++n) {
		const auto K = wheel(n);
		CHECK(K.V.size() == n);
		CHECK(K.E.size() == 2 * (n - 1));
	}
}
