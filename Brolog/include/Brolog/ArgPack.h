// ArgPack.h - Copyright (c) 2016 Will Cassella
#pragma once

#include <tuple>
#include "VarChain.h"

namespace brolog
{
	template <typename ... Ts, char ... Ns, typename ... VarChainTs>
	std::tuple<Var<Ts>*...> create_arg_pack(tmp::type_list<Ts...>, tmp::char_list<Ns...>, VarChainTs& ... varChains)
	{
		return std::make_tuple(&tmp::cast_first_suitable<VarChainElement<Ts, Ns>>(varChains...)...);
	}

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

	template <std::size_t I, typename ... Ts, typename ContinueFnT>
	bool unify_arg_pack_element(
		std::false_type,
		const std::tuple<Var<Ts>*...>& /*args*/,
		const std::tuple<Ts...>& /*fact*/,
		const ContinueFnT& next)
	{
		return next();
	}

	template <typename ... Ts, typename ContinueFnT>
	bool unify_arg_pack(const std::tuple<Var<Ts>*...>& args, const std::tuple<Ts...>& fact, const ContinueFnT& next)
	{
		return unify_arg_pack_element<0>(std::bool_constant<0 < sizeof...(Ts)>{}, args, fact, next);
	}

	template <std::size_t I, typename TupleT>
	auto arg_pack_unified(const TupleT& argPack) -> std::enable_if_t<I < std::tuple_size<TupleT>::value, bool>
	{
		return std::get<I>(argPack)->unified() && arg_pack_unified<I + 1>(argPack);
	}

	template <std::size_t I, typename TupleT>
	auto arg_pack_unified(const TupleT& argPack) -> std::enable_if_t<I >= std::tuple_size<TupleT>::value, bool>
	{
		return true;
	}
}
