/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_PROPERTIES_HPP_INCLUDED
#define QUIVER_PROPERTIES_HPP_INCLUDED

#include <quiver/util.hpp>
#include <type_traits>
#include <utility>

#define QUIVER_DEFINE_PROPERTY_CHECKER(property)						\
template<typename T>													\
class has_##property													\
{																		\
    template<typename U>												\
	static std::true_type check(decltype(&U::property)*);				\
    template<typename U>												\
	static std::false_type check(...);									\
public:																	\
    using type = decltype(check<T>(0));									\
    using value_type = typename type::value_type;						\
    inline static constexpr value_type value = type::value;				\
};																		\
template<typename T>													\
inline constexpr bool has_##property##_v = has_##property <T>::value;

namespace quiver
{
	// weight
	template<typename T>
	struct wt
	{
		T weight;

		template<typename... args_t>
		constexpr wt(args_t&&... args) noexcept(std::is_nothrow_constructible_v<T, args_t...>)
		: weight(std::forward<args_t>(args)...)
		{
		}
	};
	QUIVER_DEFINE_PROPERTY_CHECKER(weight)
	template<typename graph_t>
	struct is_weighted : public std::integral_constant<bool, has_weight_v<typename graph_t::out_edge_t>>
	{
	};
	template<typename graph_t>
	inline constexpr bool is_weighted_v = is_weighted<graph_t>::value;
	template<typename graph_t>
	using weight_type = decltype(graph_t::out_edge_t::weight);

	// capacity
	template<typename T>
	struct cap
	{
		T capacity;

		template<typename... args_t>
		constexpr cap(args_t&&... args) noexcept(std::is_nothrow_constructible_v<T, args_t...>)
		: capacity(std::forward<args_t>(args)...)
		{
		}
	};
	QUIVER_DEFINE_PROPERTY_CHECKER(capacity)
	template<typename graph_t>
	struct has_capacities : public std::integral_constant<bool, has_capacity_v<typename graph_t::out_edge_t>>
	{
	};
	template<typename graph_t>
	inline constexpr bool has_capacities_v = has_capacities<graph_t>::value;
	template<typename graph_t>
	using capacity_type = decltype(graph_t::out_edge_t::capacity);

	// combinator of different properties
	template<typename... T>
	struct cmb
	{
		static_assert(get_false<T...>(), "cmb<> may not be empty");
	};
	template<typename T, typename... U>
	struct cmb<T, U...> : public T, public cmb<U...>
	{
		template<typename head_t, typename... tail_t>
		constexpr cmb(head_t&& head, tail_t&&... tail) noexcept(std::is_nothrow_constructible_v<T, head_t> && std::is_nothrow_constructible_v<cmb<U...>, tail_t...>)
		: T(std::forward<head_t>(head)), cmb<U...>(std::forward<tail_t>(tail)...)
		{
		}
	};
	template<>
	struct cmb<>
	{
		constexpr cmb() noexcept
		{
		}
	};
}

#endif // !QUIVER_PROPERTIES_HPP_INCLUDED
