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
	}
}
