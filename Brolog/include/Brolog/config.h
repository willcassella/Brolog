// config.h
#pragma once

#ifdef _MSC_VER
#	define EXPORT __declspec(dllexport)
#	define IMPORT __declspec(dllimport)
#else
#	define EXPORT
#	define IMPORT
#endif

#ifdef BROLOG_BUILD
#	define BROLOG_API EXPORT
#else
#	define BROLOG_API IMPORT
#endif

namespace brolog
{
	template <typename ... T>
	struct list {};
}
