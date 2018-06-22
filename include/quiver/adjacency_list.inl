/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_ADJACENCY_LIST_INL_INCLUDED
#define QUIVER_ADJACENCY_LIST_INL_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <quiver/util.hpp>
#include <quiver/properties.hpp>
#include <quiver/container_interface.hpp>
#include <utility>
#include <cassert>
#include <sstream>

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
constexpr void quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::normalize(vertex_index_t& from, vertex_index_t& to) noexcept
{
	if(from > to)
		std::swap(from, to);
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
bool quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::remove_vertex_simple(vertex_index_t index)
{
	assert(index < V());

	const std::size_t out_degree = m_vertices[index].out_degree();
	m_vertices.erase(m_vertices.begin() + index);
	m_e -= out_degree;
	--m_v;
	return true;
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
template<typename... args_t>
bool quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::add_edge_simple(vertex_index_t from, vertex_index_t to, args_t&&... args)
{
	assert(from < V());
	assert(to < V());
	assert(get_edge_simple(from, to) == nullptr);
	assert(from != to); // no loops

	m_vertices[from].out_edges.emplace_back(to, std::forward<args_t>(args)...);
	++m_e;
	return true;
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
bool quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::remove_edge_simple(vertex_index_t from, vertex_index_t to)
{
	assert(from < V());
	assert(to < V());

	// if(from == to) return false;
	auto& edges = m_vertices[from].out_edges;
	for(auto iter = edges.begin(); iter != edges.end(); ++iter)
	{
		if(iter->to == to)
		{
			edges.erase(iter);
			--m_e;
			return true;
		}
	}
	return false;
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::get_edge_simple(vertex_index_t from, vertex_index_t to) const noexcept -> out_edge_t const*
{
	assert(from < V());
	assert(to < V());

	// if(from == to) return nullptr;
	auto& edges = m_vertices[from].out_edges;
	for(auto iter = edges.begin(); iter != edges.end(); ++iter)
	{
		if(iter->to == to)
			return &*iter;
	}
	return nullptr;
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::adjacency_list(std::size_t vertices)
: m_v{ vertices }, m_vertices(vertices)
{
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
constexpr std::size_t quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::V() const noexcept
{
	return m_v;
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
constexpr std::size_t quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::E() const noexcept
{
	if constexpr(directivity == directed)
		return m_e;
	else if constexpr(directivity == undirected)
		return m_e / 2;
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
constexpr std::size_t quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::max_edges() const noexcept
{
	if constexpr(directivity == directed)
		return V() * (V() - 1);
	else if constexpr(directivity == undirected)
		return V() * (V() - 1) / 2;
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
constexpr bool quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::empty() const noexcept
{
	return V() == 0;
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
constexpr bool quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::edgeless() const noexcept
{
	return E() == 0;
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::vertex(vertex_index_t index) const noexcept -> vertex_t const&
{
	assert(index < V());
	return m_vertices[index];
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::vertex(vertex_index_t index) noexcept -> vertex_t&
{
	return const_cast<vertex_t&>(static_cast<adjacency_list const&>(*this).vertex(index));
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
std::size_t quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::in_degree(vertex_index_t index) const noexcept
{
	if constexpr(directivity == directed) {
		std::size_t count = 0;
		for(auto const& vertex : m_vertices)
			count += vertex.has_edge_to(index);
		return count;
	} else if constexpr(directivity == undirected) {
		return out_degree(index);
	}
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
std::size_t quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::out_degree(vertex_index_t index) const noexcept
{
	return vertex(index).out_degree();
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
std::size_t quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::degree(vertex_index_t index) const noexcept
{
	static_assert(directivity == undirected, "degree only defined for undirected graphs");
	return out_degree(index);
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
template<typename... args_t>
quiver::vertex_index_t quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::add_vertex(args_t&&... args)
{
	m_vertices.emplace_back(std::forward<args_t>(args)...);
	// assert(directivity == directed || m_vertices.back().out_edges.empty()); // we mustn't have this check for split_ccs
	m_e += m_vertices.back().out_edges.size();
	return m_v++;
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
bool quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::remove_vertex(vertex_index_t index)
{
	assert(index < V());

	if(remove_vertex_simple(index)) {
		for(auto& vertex : m_vertices)
		{
			for(std::size_t i = vertex.out_edges.size(); i-- > 0;)
			{
				if(vertex.out_edges[i].to == index)
				{
					vertex.out_edges.erase(vertex.out_edges.begin() + i);
					++i;
					--m_e;
				}
				else if(vertex.out_edges[i].to > index)
				{
					--vertex.out_edges[i].to;
				}
			}
		}
		return true;
	} else {
		return false;
	}
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
void quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::reserve(std::size_t vertices)
{
	m_vertices.reserve(vertices);
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
std::size_t quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::capacity() const noexcept
{
	return m_vertices.capacity();
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
template<typename... args_t>
bool quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::add_edge(vertex_index_t from, vertex_index_t to, args_t&&... args)
{
	assert(from < V());
	assert(to < V());
	// assert(the edge (from, to) doesn't already exist);

	if constexpr(directivity == directed) {
		add_edge_simple(from, to, std::forward<args_t>(args)...);
	} else if constexpr(directivity == undirected) {
		add_edge_simple(to, from, std::as_const(args)...);
		add_edge_simple(from, to, std::forward<args_t>(args)...);
		/*
		if(from != to) {
			add_edge_simple(to, from, std::as_const(args)...);
			add_edge_simple(from, to, std::forward<args_t>(args)...);
			// TODO: strong exception safety
		} else {
			add_edge_simple(from, to, std::forward<args_t>(args)...);
		}
		*/
	}
	return true;
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
bool quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::remove_edge(vertex_index_t from, vertex_index_t to)
{
	assert(from < V());
	assert(to < V());

	// if(from == to) return false;
	if constexpr(directivity == directed) {
		return remove_edge_simple(from, to);
	} else if constexpr(directivity == undirected) {
		if(from == to)
			return remove_edge_simple(from, to);

		assert((get_edge_simple(from, to) == nullptr) == (get_edge_simple(to, from) == nullptr));
		const bool removed = remove_edge_simple(from, to);
		if(removed)
			remove_edge_simple(from, to); // TODO: strong exception safety
		return removed;
	}
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::v_begin()
{
	return m_vertices.begin();
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::v_begin() const
{
	return m_vertices.begin();
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::v_cbegin() const
{
	return m_vertices.cbegin();
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::v_end()
{
	return m_vertices.end();
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::v_end() const
{
	return m_vertices.end();
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::v_cend() const
{
	return m_vertices.cend();
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::vertices() const& -> adjacency_list
{
	adjacency_list result;
	result.m_vertices.reserve(V());
	for(auto const& vertex : m_vertices)
		result.add_vertex(vertex.properties());
	return result;
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::vertices() && -> adjacency_list&&
{
	for(auto& vertex : m_vertices) {
		std::size_t diff_e = vertex.out_edges.size();
		vertex.out_edges.clear();
		m_e -= diff_e;
	}
	assert(m_e == 0);
	return std::move(*this);
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::get_edge(vertex_index_t from, vertex_index_t to) const noexcept -> out_edge_t const*
{
	assert(from < V());
	assert(to < V());

	if constexpr(directivity == undirected)
		normalize(from, to); // so that get_edge(1,0) == get_edge(0,1)
	return get_edge_simple(from, to);
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
template<typename invokable_t>
void quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::transform_outs(invokable_t invokable)
{
	for(auto& vertex : m_vertices)
		for(auto& out_edge : vertex.out_edges)
			out_edge.to = invokable(out_edge.to);
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
void quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::swap(adjacency_list& rhs) noexcept
{
	using std::swap;
	swap(m_vertices, rhs.m_vertices);
	swap(m_v, rhs.m_v);
	swap(m_e, rhs.m_e);
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
constexpr bool quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::is_weighted() noexcept
{
	return is_weighted_v<adjacency_list>;
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
constexpr bool quiver::adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>::is_simple() noexcept
{
	return !is_weighted() && directivity == undirected;
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::begin(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>& graph)
{
	return graph.v_begin();
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::begin(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph)
{
	return graph.v_begin();
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::cbegin(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph)
{
	return graph.v_cbegin();
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::end(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>& graph)
{
	return graph.v_end();
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::end(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph)
{
	return graph.v_end();
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
auto quiver::cend(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph)
{
	return graph.v_cend();
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
void quiver::swap(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& lhs, adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& rhs) noexcept
{
	lhs.swap(rhs);
}

template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
std::ostream& quiver::to_dot(std::ostream& stream, adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph)
{
	if constexpr(dir == directed)
		stream << "di";
	stream << "graph\n";
	stream << "{\n";

	// vertices
	for(vertex_index_t vert_index = 0; vert_index < graph.V(); ++vert_index)
		stream << '\t' << vert_index << ";\n";

	// edges
	{
		vertex_index_t vert_index = 0;
		for(auto const& vert : graph) {
			for(auto const& out_edge : vert.out_edges)
				if constexpr(dir == directed) {
					stream << '\t' << vert_index << "->" << out_edge.to << ";\n";
				} else if constexpr(dir == undirected) {
					if(vert_index <= out_edge.to)
						stream << '\t' << vert_index << "--" << out_edge.to << ";\n";
				}
			++vert_index;
		}
	}

	stream << "}\n";
	return stream;
}
template<quiver::directivity_t dir, typename edge_properties_t, typename vertex_properties_t, template<typename> class out_edge_container, template<typename> class vertex_container>
std::string quiver::to_dot(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph)
{
	std::stringstream strstr;
	to_dot(strstr, graph);
	return strstr.str();
}

#endif // !QUIVER_ADJACENCY_LIST_INL_INCLUDED
