// ArgPack.h - Copyright (c) 2016 Will Cassella
#pragma once

#include <tuple>
#include "VarChain.h"

namespace brolog
{
	/* Creates an 'arg pack' for the given variable types and names, given a set of var chains.
	 * This is used to create the tuple of variables for invoking a predicate. */
	template <typename ... Ts, int ... Ns, typename ... VarChainTs>
	std::tuple<Var<Ts>*...> create_arg_pack(tmp::type_list<Ts...> /*types*/, tmp::int_list<Ns...> /*names*/, VarChainTs& ... varChains)
	{
		// Return a pointer to the first VarChainElement in the set of var chains that has the requested variable type and name
		return std::make_tuple(&tmp::cast_first_suitable<VarChainElement<Ts, Ns>>(varChains...)...);
	}

	/* Recursively unifies an 'arg pack', with a fact one element at a time. */
	template <std::size_t I, typename T, typename ... Ts, typename ContinueFnT>
	bool unify_arg_pack_element(
		std::true_type,
		const std::tuple<Var<T>*, Var<Ts>*...>& args,
		const std::tuple<T, Ts...>& fact,
		const ContinueFnT& next)
	{
		// If the variable already has a value
		if (std::get<I>(args)->unified())
		{
			// Continue only if the value is equivalent to the value in the fact
			if (std::get<I>(args)->value() == std::get<I>(fact))
			{
				return unify_arg_pack_element<I + 1>(std::bool_constant<I < sizeof...(Ts)>{}, args, fact, next);
			}
		}
		else
		{
			// Unify the var value with the fact value
			std::get<I>(args)->unify(std::get<I>(fact));

			// Continue
			bool success = unify_arg_pack_element<I + 1>(std::bool_constant<I < sizeof...(Ts)>{}, args, fact, next);

			// Unbind the var
			std::get<I>(args)->unbind();

			return success;
		}

		return false;
	}

	/* Recursive end-case for 'unify_arg_pack_element', all arguments have been unified, calls the continuation function. */
	template <std::size_t I, typename ... Ts, typename ContinueFnT>
	bool unify_arg_pack_element(
		std::false_type,
		const std::tuple<Var<Ts>*...>& /*args*/,
		const std::tuple<Ts...>& /*fact*/,
		const ContinueFnT& next)
	{
		return next();
	}

	/* Unifies an 'arg pack' with a fact, calling the 'next' function when complete, or returning on failure.
	 * Returns whether unification was successful. This may return false if this unification failed, or if failure occurred further on. */
	template <typename ... Ts, typename ContinueFnT>
	bool unify_arg_pack(const std::tuple<Var<Ts>*...>& args, const std::tuple<Ts...>& fact, const ContinueFnT& next)
	{
		return unify_arg_pack_element<0>(std::bool_constant<0 < sizeof...(Ts)>{}, args, fact, next);
	}

	/* Returns whether the given 'arg pack' has been completely unified, recursive. */
	template <std::size_t I, typename TupleT>
	auto arg_pack_unified(const TupleT& argPack) -> std::enable_if_t<I < std::tuple_size<TupleT>::value, bool>
	{
		return std::get<I>(argPack)->unified() && arg_pack_unified<I + 1>(argPack);
	}

	/* Recursive end-case for 'arg_pack_unified', no more members to look at. */
	template <std::size_t I, typename TupleT>
	auto arg_pack_unified(const TupleT& argPack) -> std::enable_if_t<I >= std::tuple_size<TupleT>::value, bool>
	{
		return true;
	}
}
