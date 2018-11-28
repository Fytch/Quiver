/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#include <quiver.hpp>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>

int main()
{
	char style[] = "node [shape=point, margin=0.1, width=0.1, style=filled, fillcolor=red]";
	quiver::adjacency_list<quiver::undirected> graph = quiver::complete(5);
	std::string dot = to_dot(graph);
	dot.insert(dot.find('{') + 1, style);

	FILE* graphviz = popen("neato -Tpng -ograph.png", "w");
	if(!graphviz) {
		std::cerr << "error: " << strerror(errno) << '\n';
		return -1;
	}
	fprintf(graphviz, "%s", dot.c_str());
	pclose(graphviz);
}
