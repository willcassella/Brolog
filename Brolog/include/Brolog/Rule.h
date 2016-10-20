// Rule.h - Copyright (c) 2016 Will Cassella
#pragma once

#include "ArgPack.h"
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
			// Enumerate all instances of this rule in the database
			const auto& instances = static_cast<const DataBaseElement<DBaseT, RuleType>&>(dataBase).instances;
			auto control = EControl::CONTINUE;

			for (auto rule = instances.begin(); rule != instances.end() && control != EControl::BREAK; ++rule)
			{
				(*rule)(dataBase, args, next);
			}
		}

		/* Creates a new instance of this fact and inserts it into the database. */
		template <typename RuleInstance, typename DBaseT>
		static void make_instance(DBaseT& dataBase)
		{
			static_cast<DataBaseElement<DBaseT, RuleType>&>(dataBase).instances.insert(&RuleInstance::template satisfy<DBaseT>);
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

	/* The predicate to not satify, and teh way to satisfy it (argument names).
	 * Negation requires that all arguments have already been unified, and therfore does not allow introducing new arguments. */
	template <typename PredicateT, char ... ArgNs>
	struct NotSatisfy
	{
	};

	template <typename TypeT, typename Params, typename ... PredicateTs>
	struct Rule
	{
		using Type = TypeT;

		template <typename DBaseT>
		static void satisfy(const DBaseT& dataBase, typename TypeT::ArgTuple& args, const ContinueFn& next)
		{
			// Create an initial var chain
			auto varChain = create_var_chain<VarChainRoot, ReferencedVarChainElement>(typename TypeT::ArgTypes{}, Params{});

			// Fill it
			if (!fill_initial_arg_chain<0>(typename TypeT::ArgTypes{}, Params{}, args, varChain))
			{
				// Args don't work, backtrack immediately
				return;
			}

			// Satisfy the first predicate
			satisfy_predicate(tmp::type_list<PredicateTs...>{}, dataBase, next, varChain);
		}

	private:

		template <
		typename PredT,
		char ... ArgNs,
		typename ... SatTs,
		typename DBaseT,
		typename ContinueFnT,
		typename ... OuterVarChainTs>
		static void satisfy_predicate(
			tmp::type_list<Satisfy<PredT, ArgNs...>, SatTs...>,
			const DBaseT& dataBase,
			const ContinueFnT& next,
			OuterVarChainTs& ... outerVarChains)
		{
			// Create a new var chain for this scope (allows introducing new variables and type-checking existing ones)
			auto localVarChain = create_var_chain<VarChainRoot, StoredVarChainElement>(
				typename PredT::ArgTypes{}, tmp::char_list<ArgNs...>{}, outerVarChains...);

			// Create an arg pack for this predicate
			auto argPack = create_arg_pack(typename PredT::ArgTypes{}, tmp::char_list<ArgNs...>{}, outerVarChains..., localVarChain);

			// Recursively satisfy predicates
			PredT::satisfy(dataBase, argPack,
				[&]() {
					satisfy_predicate(tmp::type_list<SatTs...>{}, dataBase, next, outerVarChains..., localVarChain);
			});
		}

		template <
		typename PredT,
		char ... ArgNs,
		typename ... SatTs,
		typename DBaseT,
		typename ContinueFnT,
		typename ... OuterVarChainTs>
		static void satisfy_predicate(
			tmp::type_list<NotSatisfy<PredT, ArgNs...>, SatTs...>,
			const DBaseT& database,
			const ContinueFnT& next,
			OuterVarChainTs& ... outerVarChains)
		{
			// Do NOT create a local var chain for this scope (since introducing new variables in negative predicates is not allowed)
			auto argPack = create_arg_pack(typename PredT::ArgTypes{}, tmp::char_list<ArgNs...>{}, outerVarChains...);

			bool satisfied = false;
			PredT::satisfy(database, argPack,
				[&]() {
				satisfied = true;
			});

			if (!satisfied)
			{
				satisfy_predicate(tmp::type_list<SatTs...>{}, database, next, outerVarChains...);
			}
		}

		template <
		typename DBaseT,
		typename ContinueFnT,
		typename ... OuterVarChainTs>
		static void satisfy_predicate(
			tmp::type_list<>,
			const DBaseT& /*dataBase*/,
			const ContinueFnT& next,
			OuterVarChainTs& ... /*outerVarCHains*/)
		{
			next();
		}
	};

	template <typename DBase, typename CookieT, typename ... ArgTs>
	struct DataBaseElement< DBase, RuleType<CookieT, ArgTs...> >
	{
		//////////////////
		///   Fields   ///
	public:

		std::set<typename RuleType<CookieT, ArgTs...>::template Instance<DBase>> instances;
	};
}
