// TMP.h - Copyright (c) 2016 Will Cassella
#pragma once

#include <type_traits>

namespace brolog
{
	namespace tmp
	{
		template <typename ... T>
		struct type_list {};

		template <typename T>
		struct type {};

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

		template <char ... Cs>
		using char_list = std::integer_sequence<char, Cs...>;

		template <char T, typename CharList>
		struct element_of_char_list;

		template <char T, char C, char ... Cs>
		struct element_of_char_list < T, char_list<C, Cs...> > : element_of_char_list<T, char_list<Cs...>>
		{
		};

		template <char T, char ... Cs>
		struct element_of_char_list < T, char_list<T, Cs...> > : std::true_type
		{
		};

		template <char T>
		struct element_of_char_list < T, char_list<> > : std::false_type
		{
		};

		template <bool B, bool ... Bs>
		struct fold_or : std::bool_constant<B || fold_or<Bs...>::value>
		{
		};

		template <bool B>
		struct fold_or< B > : std::bool_constant<B>
		{
		};

		template <typename BaseT, typename ... DerivedTs>
		struct is_base_of_any : fold_or<std::is_base_of<BaseT, DerivedTs>::value...>
		{
		};

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
