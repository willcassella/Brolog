// Rule.h
#pragma once

#include <cstdint>
#include <tuple>
#include "VarChain.h"
#include "DataBase.h"
#include "Enumerator.h"

namespace brolog
{
	template <typename Cookie, typename ... Ts>
	struct RuleType
	{
		using SatisfyArgT = std::tuple<Var<typename ParamTs::Type>...>;

		using EnumeratorT = Enumerator<std::tuple<typename ParamTs::Type...>>;

		template <typename DBase>
		using InstanceT = std::size_t(*)(const DBase&, const SatisfyArgT&, const EnumeratorT&);
	};

	/* The predicate to satisfy, and the way to satisfy it (argument names).
	 * If any of the argument names have not previously been used in this rule, it is interpreted as a new unbound variable. */
	template <typename PredT, char ... Args>
	struct Satisfy
	{
		template <typename NextSat, typename DBaseT, typename VarChainT, typename EnumeratorFn>
		static std::size_t satisfy(const DBaseT& dataBase, const VarChainT& vars, const EnumeratorFn& enumerator)
		{
			auto localArgPack = create_arg_pack(typename PredT::arg_list{}, tmp::char_list<Args...>{}, vars);
			PredT::satisfy(dataBase, localArgPack, [&](const auto&)
			{
				NextSat::satisfy(dataBase, localArgPack.var_chain, enumerator);
			});
		}
	};

	template <char ... Ps>
	using Params = tmp::char_list<Ps...>;

	template <typename Type, typename Ps, typename ... Predicates>
	struct Rule {};
}
