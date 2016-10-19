// Rule.h
#pragma once

#include "VarPack.h"
#include "DataBase.h"
#include "Enumerator.h"

namespace brolog
{
	template <typename CookieT, typename ... ArgTs>
	struct RuleType
	{
		using ArgTypes = tmp::type_list<ArgTs...>;

		using ArgTuple = std::tuple<Var<ArgTs>*...>;

		template <typename DBaseT>
		using Instance = void(*)(const DBaseT& dataBase, ArgTuple& args, const ContinueFn& next);

		template <typename DBaseT, typename ContinueFnT>
		static void satisfy(const DBaseT& dataBase, ArgTuple& args, const ContinueFnT& next)
		{

		}
	};

	/* Declares a series of paramaters for this Rule. */
	template <char ... Ns>
	using Params = tmp::char_list<Ns...>;

	/* The predicate to satisfy, and the way to satisfy it (argument names).
	 * If any of the argument names have not previously been used in this rule, it is interpreted as a new unbound variable. */
	template <typename PredicateT, char ... ArgNs>
	struct Satisfy
	{
	};

	template <typename TypeT, typename Params, typename ... PredicateTs>
	struct Rule
	{
		template <typename DBaseT>
		static void satisfy(const DBaseT& dataBase, typename TypeT::ArgTuple& args, const ContinueFn& next)
		{
			auto varPack = create_var_pack<typename TypeT::ArgTypes, Params>();
			satisfy_predicate(tmp::type_list<PredicateTs...>{}, dataBase, varPack, next);
		}

	private:

		template <
		typename PredT,
		char ... ArgNs,
		typename ... SatTs,
		typename DBaseT,
		typename OuterVarPackT,
		typename ContinueFnT>
		static void satisfy_predicate(
			tmp::type_list<Satisfy<PredT, ArgNs...>, SatTs...>,
			const DBaseT& dataBase,
			OuterVarPackT& outerVarPack,
			const ContinueFnT& next)
		{
			// Create a new var pack for this scope (allows introducing new variables and type-checking existing ones)
			auto localVarPack = create_var_pack<typename PredT::ArgTypes, tmp::char_list<ArgNs...>>(outerVarPack);

			// Recursively satisfy predicates
			PredT::satisfy(dataBase, create_arg_tuple(typename PredT::ArgTypes{}, tmp::char_list<ArgNs...>{}, localVarPack),
				[&]() {
					satisfy_predicate(tmp::type_list<SatTs...>{}, dataBase, localVarPack, next);
			});
		}

		template <
		typename DBaseT,
		typename OuterVarPackT,
		typename ContinueFnT>
		static void satisfy_predicate(
			tmp::type_list<>,
			const DBaseT& /*dataBase*/,
			OuterVarPackT& /*outerVarPack*/,
			const ContinueFnT& next)
		{
			next();
		}
	};
}
