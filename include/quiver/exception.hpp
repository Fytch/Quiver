/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_EXCEPTION_HPP_INCLUDED
#define QUIVER_EXCEPTION_HPP_INCLUDED

#include <stdexcept>

namespace quiver
{
	struct cyclic_graph : public std::invalid_argument
	{
		using std::invalid_argument::invalid_argument;
	};
}

#endif // !QUIVER_EXCEPTION_HPP_INCLUDED
