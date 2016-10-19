// TMP.h
#pragma once

#include <type_traits>

namespace brolog
{
	namespace tmp
	{
		template <typename ... T>
		struct type_list {};

		template <char ... Cs>
		using char_list = std::integer_sequence<char, Cs...>;

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
	}
}
