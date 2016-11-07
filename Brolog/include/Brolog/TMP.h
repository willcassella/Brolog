// TMP.h - Copyright (c) 2016 Will Cassella
#pragma once

#include <type_traits>

namespace brolog
{
	/* Template meta-programming namespace. */
	namespace tmp
	{
		template <typename ... T>
		struct type_list {};

		/* Given a type_list and a type, evaluates to std::true_type if the type exists in the type_list, std::false_type otherwise. */
		template <typename TypeList, typename T>
		struct element_of;

		template <typename F, typename ... Fs, typename T>
		struct element_of < type_list<F, Fs...>, T > : element_of<type_list<Fs...>, T>
		{
		};

		template <typename T, typename ... Fs>
		struct element_of < type_list<T, Fs...>, T > : std::true_type
		{
		};

		template <typename T>
		struct element_of < type_list<>, T > : std::false_type
		{
		};

		/* Similar to 'type_list', but for integer constants (here used for variable names). */
		template <int ... Is>
		using int_list = std::integer_sequence<int, Is...>;

		/* Given a int and a int list, evaluates to std::true_type if the given int exists in the int_list, std::false_type otherwise. */
		template <int T, typename CharList>
		struct element_of_int_list;

		template <int T, int I, int ... Is>
		struct element_of_int_list < T, int_list<I, Is...> > : element_of_int_list<T, int_list<Is...>>
		{
		};

		template <int T, int ... Is>
		struct element_of_int_list < T, int_list<T, Is...> > : std::true_type
		{
		};

		template <int T>
		struct element_of_int_list < T, int_list<> > : std::false_type
		{
		};

		/* Replacement for C++17 fold expressions, 'or'. */
		template <bool B, bool ... Bs>
		struct fold_or : std::bool_constant<B || fold_or<Bs...>::value>
		{
		};

		template <bool B>
		struct fold_or< B > : std::bool_constant<B>
		{
		};

		/* Evaluates to std::true_type if the given base type is the base of any of the given types, std::false_type otherwise. */
		template <typename BaseT, typename ... DerivedTs>
		struct is_base_of_any : fold_or<std::is_base_of<BaseT, DerivedTs>::value...>
		{
		};

		/* Casts the first element of the series of types that can be cast to the target type. */
		template <typename Target, typename T, typename ... Ts>
		auto cast_first_suitable(T& first, Ts& ... rest) -> std::enable_if_t<std::is_base_of<Target, T>::value, Target&>
		{
			return static_cast<Target&>(first);
		}

		template <typename Target, typename T, typename ... Ts>
		auto cast_first_suitable(T& /*first*/, Ts& ... rest) -> std::enable_if_t<!std::is_base_of<Target, T>::value, Target&>
		{
			return cast_first_suitable<Target>(rest...);
		}
	}
}
