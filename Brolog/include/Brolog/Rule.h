// Rule.h - Copyright (c) 2016 Will Cassella
#pragma once

#include "ArgPack.h"
#include "DataBase.h"
#include "Function.h"

namespace brolog
{
	template <typename CookieT, typename ... ArgTs>
	struct RuleType
	{
		using ArgTypes = tmp::type_list<ArgTs...>;

		using ArgTuple = std::tuple<Var<ArgTs>*...>;

		template <typename DBaseT>
		using Instance = bool(*)(const DBaseT& dataBase, ArgTuple& args, const ContinueFn& next);

		template <typename DBaseT, typename ContinueFnT>
		static bool satisfy(const DBaseT& dataBase, ArgTuple& args, const ContinueFnT& next)
		{
			// Enumerate all instances of this rule in the database
			const auto& instances = static_cast<const DataBaseElement<DBaseT, RuleType>&>(dataBase).instances;

			// If all the arguments to this rule were initally unified, we only have to find the first clause that works
			bool initiallyUnified = arg_pack_unified<0>(args);

			// Stores whether this rule was ever satisfied
			bool satisfied = false;

			for (auto rule = instances.begin(); rule != instances.end() && !(initiallyUnified && satisfied); ++rule)
			{
				satisfied |= (*rule)(dataBase, args, next);
			}

			return satisfied;
		}

		/* Creates a new instance of this fact and inserts it into the database. */
		template <typename RuleInstance, typename DBaseT>
		static void make_instance(DBaseT& dataBase)
		{
			static_cast<DataBaseElement<DBaseT, RuleType>&>(dataBase).instances.push_back(&RuleInstance::template satisfy<DBaseT>);
		}
	};

	/* Declares a series of paramaters for this Rule. */
	template <int ... Ns>
	using Params = tmp::int_list<Ns...>;

	/* The predicate to satisfy, and the way to satisfy it (argument names).
	 * If any of the argument names have not previously been used in this rule, it is interpreted as a new unbound variable. */
	template <typename PredicateT, int ... ArgNs>
	struct Satisfy
	{
	};

	/* The predicate to not satify, and teh way to satisfy it (argument names).
	 * Negation requires that all arguments have already been unified, and therfore does not allow introducing new arguments. */
	template <typename PredicateT, int ... ArgNs>
	struct NotSatisfy
	{
	};

	template <typename TypeT, typename Params, typename ... PredicateTs>
	struct Rule
	{
		using Type = TypeT;

		template <typename DBaseT>
		static bool satisfy(const DBaseT& dataBase, typename TypeT::ArgTuple& args, const ContinueFn& next)
		{
			// Create an initial var chain
			auto varChain = create_var_chain<VarChainRoot, ReferencedVarChainElement>(typename TypeT::ArgTypes{}, Params{});

			// Fill it
			if (!fill_initial_arg_chain<0>(typename TypeT::ArgTypes{}, Params{}, args, varChain))
			{
				// Args don't work, backtrack immediately
				return false;
			}

			// Satisfy the first predicate
			return satisfy_predicate(tmp::type_list<PredicateTs...>{}, dataBase, next, varChain);
		}

	private:

		template <
		typename PredT,
		int ... ArgNs,
		typename ... SatTs,
		typename DBaseT,
		typename ContinueFnT,
		typename ... OuterVarChainTs>
		static bool satisfy_predicate(
			tmp::type_list<Satisfy<PredT, ArgNs...>, SatTs...>,
			const DBaseT& dataBase,
			const ContinueFnT& next,
			OuterVarChainTs& ... outerVarChains)
		{
			// Create a new var chain for this scope (allows introducing new variables and type-checking existing ones)
			auto localVarChain = create_var_chain<VarChainRoot, StoredVarChainElement>(
				typename PredT::ArgTypes{}, tmp::int_list<ArgNs...>{}, outerVarChains...);

			// Create an arg pack for this predicate
			auto argPack = create_arg_pack(typename PredT::ArgTypes{}, tmp::int_list<ArgNs...>{}, outerVarChains..., localVarChain);

			// Recursively satisfy predicates
			return PredT::satisfy(dataBase, argPack,
				[&]() {
					return satisfy_predicate(tmp::type_list<SatTs...>{}, dataBase, next, outerVarChains..., localVarChain);
			});
		}

		template <
		typename PredT,
		int ... ArgNs,
		typename ... SatTs,
		typename DBaseT,
		typename ContinueFnT,
		typename ... OuterVarChainTs>
		static bool satisfy_predicate(
			tmp::type_list<NotSatisfy<PredT, ArgNs...>, SatTs...>,
			const DBaseT& database,
			const ContinueFnT& next,
			OuterVarChainTs& ... outerVarChains)
		{
			// Do NOT create a local var chain for this scope (since introducing new variables in negative predicates is not allowed)
			auto argPack = create_arg_pack(typename PredT::ArgTypes{}, tmp::int_list<ArgNs...>{}, outerVarChains...);

			bool satisfied = false;
			PredT::satisfy(database, argPack,
				[&]() {
				satisfied = true;
				return true;
			});

			if (satisfied)
			{
				return false;
			}

			return satisfy_predicate(tmp::type_list<SatTs...>{}, database, next, outerVarChains...);
		}

		template <
		typename DBaseT,
		typename ContinueFnT,
		typename ... OuterVarChainTs>
		static bool satisfy_predicate(
			tmp::type_list<>,
			const DBaseT& /*dataBase*/,
			const ContinueFnT& next,
			OuterVarChainTs& ... /*outerVarCHains*/)
		{
			return next();
		}
	};

	template <typename DBase, typename CookieT, typename ... ArgTs>
	struct DataBaseElement< DBase, RuleType<CookieT, ArgTs...> >
	{
		//////////////////
		///   Fields   ///
	public:

		/* Rules are stored as a vector instead of a set, since there is less likelyhood of duplication
		 * and it allows for control over iteration order. */
		std::vector<typename RuleType<CookieT, ArgTs...>::template Instance<DBase>> instances;
	};
}
