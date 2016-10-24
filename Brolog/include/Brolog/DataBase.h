// DataBase.h - Copyright (c) 2016 Will Cassella
#pragma once

#include "ArgPack.h"

namespace brolog
{
	template <typename DBase, typename ElementT>
	struct DataBaseElement;

	template <typename TypeT, typename Params, typename ... PredicateTs>
	struct Rule;

	template <std::size_t I, char ... Ns, typename T, typename ... Ts, typename OutFnT, typename TupleT, typename ... ArgTs>
	void output_unknowns(tmp::char_list<Ns...> names, tmp::type_list<T, Ts...>, const OutFnT& out, const TupleT& tuple, const ArgTs& ... args)
	{
		output_unknowns<I + 1>(names, tmp::type_list<Ts...>{}, out, tuple, args...);
	}

	template <std::size_t I, char N, char ... Ns, typename ... Ts, typename OutFnT, typename TupleT, typename ... ArgTs>
	auto output_unknowns(tmp::char_list<Ns...>, tmp::type_list<Unknown<N>, Ts...>, const OutFnT& out, const TupleT& tuple, const ArgTs& ... args) ->
		std::enable_if_t<!tmp::element_of_char_list<N, tmp::char_list<Ns...>>::value>
	{
		output_unknowns<I + 1>(tmp::char_list<N, Ns...>{}, tmp::type_list<Ts...>{}, out, tuple, args..., std::get<I>(tuple)->value());
	}

	template <std::size_t I, char N, char ... Ns, typename ... Ts, typename OutFnT, typename TupleT, typename ... ArgTs>
	auto output_unknowns(tmp::char_list<Ns...> names, tmp::type_list<Unknown<N>, Ts...>, const OutFnT& out, const TupleT& tuple, const ArgTs& ... args) ->
		std::enable_if_t<tmp::element_of_char_list<N, tmp::char_list<Ns...>>::value>
	{
		output_unknowns<I + 1>(names, tmp::type_list<Ts...>{}, out, tuple, args...);
	}

	template <std::size_t I, char ... Ns, typename OutFnT, typename TupleT, typename ... ArgTs>
	void output_unknowns(tmp::char_list<Ns...>, tmp::type_list<>, const OutFnT& out, const TupleT& /*tuple*/, const ArgTs& ... args)
	{
		out(args...);
	}

	/* A database of rules and facts of the given types. May be used to satisfy queries against those rules and facts. */
	template <typename ... ElementTs>
	struct DataBase : DataBaseElement<DataBase<ElementTs...>, ElementTs>...
	{
		/* Inserts an instance of the given type of fact into the database.
		 * If an equivalent instance of the given type of fact already exists in the database, this has no effect.
		 * The given fact type must be a type supported by this database.
		 */
		template <typename FactT, typename ... Args>
		void insert_fact(Args&& ... args)
		{
			FactT::make_instance(*this, std::forward<Args>(args)...);
		}

		/* Removes any equivalent instances of the given type of fact into the database.
		 * If no equivalent instances of the given type of fact exist in the database, this has no effect.
		 * The given fact type must be a type supported by this database.
		 */
		template <typename FactT, typename ... Args>
		void remove_fact(Args&& ... args)
		{
			FactT::remove_instance(*this, std::forward<Args>(args)...);
		}

		/* Inserts an instance of the given type of rule into the database.
		 * The 'Params' argument should be of the form "Params<...>", to declare parameters for this scope.
		 * The set of Predicates should be of the form "Satisfy<...>" or "NotSatisfy<...>".
		 * The given rule type must be a type supported by this database. The same applies for any rule types or fact types
		 * referenced in the predicates for this rule.
		 */
		template <typename RuleT, typename Params, typename ... PredicateTs>
		void insert_rule()
		{
			using RuleInstance = Rule<RuleT, Params, PredicateTs...>;
			RuleT::template make_instance<RuleInstance>(*this);
		}

		/* Constructs a query object that can resolve the given predicate with the given arguments.
		 * You may use the 'Unknown<VAR>' type to indicate an unknown variable.
		 * The returned object may be called with the function call operator to run the query. It's only argument
		 * should be a function to call each time the predicate is satisfied, with a number of parameters matching the number of unique unknowns
		 * given to this function. The query object returns the number of times the given function was called.
		 * The returned query object remains valid even if rules or facts are added or removed from the database.
		 */
		template <typename TermT, typename ... ArgTs>
		auto create_query(const ArgTs& ... args) const
		{
			// Create the var chain for this invocation and fill it
			auto varChain = create_user_var_chain<VarChainRoot, char(0)>(typename TermT::ArgTypes{}, tmp::type_list<ArgTs...>{});
			fill_user_var_chain<char(0)>(varChain, args...);
			auto nameList = get_user_var_chain_name_list<char(0)>(tmp::char_list<>{}, tmp::type_list<ArgTs...>{});

			// The query object that is returned to the caller
			// First argument is a function to call at each unification
			return [=](const auto& out) mutable -> std::size_t
			{
				// Create an arg pack to kick off the predicate
				auto argPack = create_arg_pack(typename TermT::ArgTypes{}, nameList, varChain);

				// The number of times the predicate was satisfied
				std::size_t numInvocations = 0;

				// Construct an end 'next' function to be called when the predicate is satisfied.
				auto end = [&]() -> bool
				{
					// Call the given output function
					output_unknowns<0>(tmp::char_list<>{}, tmp::type_list<ArgTs...>{}, out, argPack);
					numInvocations += 1;

					// Return 'true' so that the outer predicate knows it was resolved
					return true;
				};

				// Run the predicate
				TermT::satisfy(*this, argPack, end);

				// Return the number of times the output function was called
				return numInvocations;
			};
		}
	};
}
