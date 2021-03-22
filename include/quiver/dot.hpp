/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_DOT_HPP_INCLUDED
#define QUIVER_DOT_HPP_INCLUDED

#include <quiver/util.hpp>
#include <cstdint>
#include <cassert>
#include <variant>
#include <optional>
#include <string>
#include <stdexcept>
#include <cmath>
#include <ostream>
#include <iomanip>

namespace quiver
{
	namespace dot
	{
		struct color_hex
		{
			std::uint8_t r, g, b, a = 0xFF;

			constexpr color_hex(std::uint8_t r, std::uint8_t g, std::uint8_t b) noexcept
				: r(r), g(g), b(b)
			{
			}
			constexpr color_hex(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) noexcept
				: r(r), g(g), b(b), a(a)
			{
			}
		};
		inline std::ostream& operator<<(std::ostream& stream, color_hex const& color)
		{
			constexpr auto int2hex = [](int x) -> char
			{
				assert(0 <= x && x < 16);
				return x < 10 ? '0' + x : 'a' + (x - 10);
			};
			stream << '"';
			stream << '#';
			stream << int2hex(color.r / 16) << int2hex(color.r % 16);
			stream << int2hex(color.g / 16) << int2hex(color.g % 16);
			stream << int2hex(color.b / 16) << int2hex(color.b % 16);
			if(color.a != 0xFF)
				stream << int2hex(color.a / 16) << int2hex(color.a % 16);
			stream << '"';
			return stream;
		}

		enum color_enum
		{
			// cba copying the entirety of the X11 and SVG tables in here; I'm just giving a sane subset
			aqua, black, blue, dark_blue, brown, crimson, cyan, fuchsia, gold, gray,
			green, dark_green, lime, olive, orange, pink, purple, red, teal, white, yellow,
		};
		inline std::ostream& operator<<(std::ostream& stream, color_enum color)
		{
			switch(color) {
			case aqua:
				stream << "Aqua";
				break;
			case black:
				stream << "Black";
				break;
			case blue:
				stream << "Blue";
				break;
			case dark_blue:
				stream << "Dark Blue";
				break;
			case brown:
				stream << "Brown";
				break;
			case crimson:
				stream << "Crimson";
				break;
			case cyan:
				stream << "Cyan";
				break;
			case fuchsia:
				stream << "Fuchsia";
				break;
			case gold:
				stream << "Gold";
				break;
			case gray:
				stream << "Gray";
				break;
			case green:
				stream << "Green";
				break;
			case dark_green:
				stream << "Dark Green";
				break;
			case lime:
				stream << "Lime";
				break;
			case olive:
				stream << "Olive";
				break;
			case orange:
				stream << "Orange";
				break;
			case pink:
				stream << "Pink";
				break;
			case purple:
				stream << "Purple";
				break;
			case red:
				stream << "Red";
				break;
			case teal:
				stream << "Teal";
				break;
			case white:
				stream << "White";
				break;
			case yellow:
				stream << "Yellow";
				break;
			default:
				assert(false);
			}
			return stream;
		}

		// deliberately not supporting HSV colors
		using color_t = std::variant<color_hex, color_enum>;
		inline std::ostream& operator<<(std::ostream& stream, color_t const& color)
		{
			std::visit([&](auto const& x) { stream << x; }, color);
			return stream;
		}

		class penwidth_t
		{
		public:
			using value_type = double;

		private:
			value_type m_width;

		public:
			penwidth_t(value_type width = 1.0)
				: m_width(width)
			{
				if(!std::isnormal(width) || width < 0.0)
					throw std::invalid_argument("quiver::dot::penwidth_t::penwidth_t: invalid pen width");
			}

			constexpr value_type get() const noexcept
			{
				return m_width;
			}
		};
		inline std::ostream& operator<<(std::ostream& stream, penwidth_t width)
		{
			return stream << width.get();
		}

		class fontsize_t
		{
		public:
			using value_type = double;

		private:
			value_type m_size;

		public:
			fontsize_t(value_type size = 14.0)
				: m_size(size)
			{
				if(!std::isnormal(size) || size < 1.0)
					throw std::invalid_argument("quiver::dot::fontsize_t::fontsize_t: invalid font size");
			}

			constexpr value_type get() const noexcept
			{
				return m_size;
			}
		};
		inline std::ostream& operator<<(std::ostream& stream, fontsize_t size)
		{
			return stream << size.get();
		}

		/*struct attribs
		{
			std::optional<std::variant<Enum, Hex>> color;

			std::optional<std::string> custom_attribs;
		};

		[](auto edge) -> attribs
		{
			attribs a;
			switch(edge.color) {
			case Color::red:
				return ColorEnum::red;
			case Color::green:
				return "[color=green]";
			case Color::blue:
				a.color = "blue";
				break;
			default:
				;
			}
			return a;
		}*/
		/*
		digraph G {
			label="My Graph";
			splines="line";

			node [fontcolor=gray fontsize=32 shape=circle]
			edge [color="green"]

			1 [label="xyz",color="red",fillcolor="blue",style="filled,dashed"];
			1->2 [label="Some Label",color="red",penwidth=2.0];
			3->4;
		}
		*/

		// https://graphviz.org/doc/info/attrs.html
		// TODO: style
		// TODO: splines
		// TODO: shape
		// TODO: support html labels. do this by replacing labels from string to variant<string, html> where html is a strong typedef of string

		struct graph_attributes
		{
			std::optional<color_t> bgcolor; // background color

			std::optional<std::string> label;
			std::optional<color_t> fontcolor;
			std::optional<std::string> fontname;
			std::optional<fontsize_t> fontsize;

			// TODO: maybe std::pair<std::string_view, std::string>?
			std::optional<std::string> custom; // custom attributes, should be of the form "a=b; c=d"

			// graph_attributes() // TODO: implement

			constexpr bool any() const noexcept
			{
				return bgcolor
					|| label
					|| fontcolor
					|| fontname
					|| fontsize
					|| custom;
			}
		};
		inline std::ostream& operator<<(std::ostream& stream, graph_attributes const& ga)
		{
			if(ga.bgcolor)
				stream << "\tbgcolor=" << *ga.bgcolor << ";\n";
			if(ga.label)
				stream << "\tlabel=" << std::quoted(*ga.label) << ";\n";
			if(ga.fontcolor)
				stream << "\tfontcolor=" << *ga.fontcolor << ";\n";
			if(ga.fontname)
				stream << "\tfontname=" << std::quoted(*ga.fontname) << ";\n";
			if(ga.fontsize)
				stream << "\tfontsize=" << *ga.fontsize << ";\n";
			if(ga.custom)
				stream << '\t' << *ga.custom << ";\n";
			return stream;
		}

		struct node_attributes
		{
			std::optional<color_t> color; // outline color
			std::optional<penwidth_t> penwidth; // outline thickness
			std::optional<color_t> fillcolor; // interior filling color

			std::optional<std::string> label;
			std::optional<color_t> fontcolor;
			std::optional<std::string> fontname;
			std::optional<fontsize_t> fontsize;

			// TODO: maybe std::pair<std::string_view, std::string>?
			std::optional<std::string> custom; // custom attributes, should be of the form "a=b; c=d"

			// node_attributes() // TODO: implement

			constexpr bool any() const noexcept
			{
				return color
					|| penwidth
					|| fillcolor
					|| label
					|| fontcolor
					|| fontname
					|| fontsize
					|| custom;
			}
		};
		inline std::ostream& operator<<(std::ostream& stream, node_attributes const& na)
		{
			// TODO: no final ", "
			if(na.color)
				stream << "color=" << *na.color << ", ";
			if(na.penwidth)
				stream << "penwidth=" << *na.penwidth << ", ";
			if(na.fillcolor)
				stream << "fillcolor=" << *na.fillcolor << ", " << "style=filled, "; // TODO: hack because we don't have style yet
			if(na.label)
				stream << "label=" << std::quoted(*na.label) << ", ";
			if(na.fontcolor)
				stream << "fontcolor=" << *na.fontcolor << ", ";
			if(na.fontname)
				stream << "fontname=" << std::quoted(*na.fontname) << ", ";
			if(na.fontsize)
				stream << "fontsize=" << *na.fontsize << ", ";
			if(na.custom)
				stream << *na.custom << ", ";
			return stream;
		}

		struct edge_attributes
		{
			std::optional<color_t> color; // outline color
			std::optional<penwidth_t> penwidth; // outline thickness
			std::optional<color_t> fillcolor; // interior filling color

			std::optional<std::string> label;
			std::optional<color_t> fontcolor;
			std::optional<std::string> fontname;
			std::optional<fontsize_t> fontsize;

			// TODO: maybe std::pair<std::string_view, std::string>?
			std::optional<std::string> custom; // custom attributes, should be of the form "a=b; c=d"

			// edge_attributes() // TODO: implement

			constexpr bool any() const noexcept
			{
				return color
					|| penwidth
					|| fillcolor
					|| label
					|| fontcolor
					|| fontname
					|| fontsize
					|| custom;
			}
		};
		inline std::ostream& operator<<(std::ostream& stream, edge_attributes const& ea)
		{
			// TODO: no final ", "
			if(ea.color)
				stream << "color=" << *ea.color << ", ";
			if(ea.penwidth)
				stream << "penwidth=" << *ea.penwidth << ", ";
			if(ea.fillcolor)
				stream << "fillcolor=" << *ea.fillcolor << ", ";
			if(ea.label)
				stream << "label=" << std::quoted(*ea.label) << ", ";
			if(ea.fontcolor)
				stream << "fontcolor=" << *ea.fontcolor << ", ";
			if(ea.fontname)
				stream << "fontname=" << std::quoted(*ea.fontname) << ", ";
			if(ea.fontsize)
				stream << "fontsize=" << *ea.fontsize << ", ";
			if(ea.custom)
				stream << *ea.custom << ", ";
			return stream;
		}
	}

	namespace detail
	{
		template<typename node_invocable_t, typename vertex_t>
		constexpr dot::node_attributes invoke_node_invocable(node_invocable_t&& node_invocable, vertex_index_t index, vertex_t const& vertex)
		{
			if constexpr(std::is_invocable_r_v<dot::node_attributes, node_invocable_t, vertex_index_t, vertex_t const&>)
				return std::forward<node_invocable_t>(node_invocable)(index, vertex);
			else if constexpr(std::is_invocable_r_v<dot::node_attributes, node_invocable_t, vertex_t const&, vertex_index_t>)
				return std::forward<node_invocable_t>(node_invocable)(vertex, index);
			else if constexpr(std::is_invocable_r_v<dot::node_attributes, node_invocable_t, vertex_t const&>)
				return std::forward<node_invocable_t>(node_invocable)(vertex);
			else if constexpr(std::is_invocable_r_v<dot::node_attributes, node_invocable_t, vertex_index_t>)
				return std::forward<node_invocable_t>(node_invocable)(index);
			else
				static_assert(get_false<node_invocable_t>(), "cannot invoke node_invocable");
		}
		template<typename edge_invocable_t, typename out_edge_t>
		constexpr dot::edge_attributes invoke_edge_invocable(edge_invocable_t&& edge_invocable, vertex_index_t from, out_edge_t const& out_edge)
		{
			if constexpr(std::is_invocable_r_v<dot::edge_attributes, edge_invocable_t, vertex_index_t, out_edge_t const&>)
				return std::forward<edge_invocable_t>(edge_invocable)(from, out_edge);
			else if constexpr(std::is_invocable_r_v<dot::edge_attributes, edge_invocable_t, vertex_index_t, vertex_index_t>)
				return std::forward<edge_invocable_t>(edge_invocable)(from, out_edge.to);
			else
				static_assert(get_false<edge_invocable_t>(), "cannot invoke edge_invocable");
		}
	}

	template<typename graph_t, typename node_invocable_t, typename edge_invocable_t>
	std::ostream& to_dot(
		std::ostream& stream,
		graph_t const& graph,
		dot::graph_attributes const& global_graph_attributes,
		dot::node_attributes const& global_node_attributes,
		dot::edge_attributes const& global_edge_attributes,
		node_invocable_t const& node_invocable, // maps (vertex_index_t) or (vertex_t const&) or (vertex_index_t, graph_t::vertex_t const&) or (graph_t::vertex_t const&, vertex_index_t) -> dot::node_attributes
		edge_invocable_t const& edge_invocable  // maps (vertex_index_t, vertex_index_t) or (vertex_index_t, graph_t::out_edge_t const&) -> dot::edge_attributes
	)
	{
		if constexpr(graph.directivity == directed)
			stream << "di";
		stream << "graph\n";
		stream << "{\n";

		stream << global_graph_attributes;
		if(global_node_attributes.any())
			stream << "\tnode [" << global_node_attributes << "]\n";
		if(global_edge_attributes.any())
			stream << "\tedge [" << global_edge_attributes << "]\n";

		// vertices
		// TODO: dont emit vertices if they don't have special attribs and they are part of at least one edge
		{
			vertex_index_t vert_index = 0;
			for(auto const& vert : graph.V) {
				const dot::node_attributes na = detail::invoke_node_invocable(node_invocable, vert_index, vert);
				const bool any = na.any();
				if(vert.out_edges.empty() || any) {
					stream << '\t' << vert_index;
					if(any)
						stream << " [" << na << ']';
					stream << ";\n";
				}

				++vert_index;
			}
		}

		// edges
		{
			vertex_index_t vert_index = 0;
			for(auto const& vert : graph.V) {
				for(auto const& out_edge : vert.out_edges) {
					if(graph.directivity == undirected && vert_index > out_edge.to)
						continue;

					stream << '\t' << vert_index;
					if constexpr(graph.directivity == directed)
						stream << "->";
					else
						stream << "--";
					stream << out_edge.to;
					dot::edge_attributes ea = detail::invoke_edge_invocable(edge_invocable, vert_index, out_edge);
					if(ea.any())
						stream << " [" << ea << ']';
					stream << ";\n";
				}
				++vert_index;
			}
		}

		stream << "}\n";
		return stream;
	}
	template<typename graph_t>
	std::ostream& to_dot(std::ostream& stream, graph_t const& graph)
	{
		if constexpr(graph.directivity == directed)
			stream << "di";
		stream << "graph\n";
		stream << "{\n";

		// vertices
		// TODO: dont emit vertices if they don't have special attribs and they are part of at least one edge. WE ALREADY DO IT HALFWAYS IN THE ABOVE OVERLOAD
		for(vertex_index_t vert_index = 0; vert_index < graph.V.size(); ++vert_index)
			stream << '\t' << vert_index << ";\n";

		// edges
		{
			vertex_index_t vert_index = 0;
			for(auto const& vert : graph.V) {
				for(auto const& out_edge : vert.out_edges)
					if constexpr(graph.directivity == directed) {
						stream << '\t' << vert_index << "->" << out_edge.to << ";\n";
					} else if constexpr(graph.directivity == undirected) {
						if(vert_index <= out_edge.to)
							stream << '\t' << vert_index << "--" << out_edge.to << ";\n";
					}
				++vert_index;
			}
		}

		stream << "}\n";
		return stream;
	}
	/*
	template<typename graph_t>
	std::string to_dot(graph_t const& graph)
	{
		std::stringstream strstr;
		to_dot(strstr, graph);
		return strstr.str();
	}
	*/
}

#endif // !QUIVER_DOT_HPP_INCLUDED
