// Rule.h
#pragma once

#include <cstdint>
#include "Var.h"
#include "TMP.h"
#include "DataBase.h"
#include "Enumerator.h"

namespace brolog
{
	template <typename Cookie, typename ... ArgTs>
	struct RuleType
	{
		using SatisfyArgT = std::tuple<Var<ArgTs>...>;

		using EnumeratorT = Enumerator<std::tuple<ArgTs...>>;

		template <typename DBase>
		using InstanceT = std::size_t(*)(const DBase&, const SatisfyArgT&, const EnumeratorT&);
	};

	template <std::size_t I>
	struct VarArg
	{
	};

	/* The term to satisfy, and the way to satisfy it (argument indices)
	 * If the argument index is out of bounds of the given arguments, it needs to create a new unbound variable. */
	template <typename TermT, typename ... Args>
	struct Satisfy
	{
		template <typename DBaseT, typename ... ArgTs, typename EnumeratorFn>
		static auto satisfy(const DBaseT& dataBase, std::tuple<Var<ArgTs>...>& args, const EnumeratorFn& enumerator)
		{
			return static_cast<DataBaseElement<TermT>&>(dataBase).satisfy();
		}

	private:

		template <typename ArgT, typename ... ArgTs>
		static bool construct_satisfy_args(tmp::list<ArgT, ArgTs...>, )
	};
}
