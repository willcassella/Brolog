// DataBase.h - Copyright (c) 2016 Will Cassella
#pragma once

#include "ArgPack.h"

namespace brolog
{
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

	template <typename DBase, typename TermT>
	struct DataBaseElement;

	template <typename TypeT, typename Params, typename ... PredicateTs>
	struct Rule;

	template <typename ... TermTs>
	struct DataBase : DataBaseElement<DataBase<TermTs...>, TermTs>...
	{
		template <typename FactT, typename ... Args>
		void insert_fact(Args&& ... args)
		{
			FactT::make_instance(*this, std::forward<Args>(args)...);
		}

		template <typename RuleT, typename Params, typename ... PredicateTs>
		void insert_rule()
		{
			using RuleInstance = Rule<RuleT, Params, PredicateTs...>;
			RuleT::template make_instance<RuleInstance>(*this);
		}

		template <typename TermT, typename ... ArgTs>
		auto satisfy(const ArgTs& ... args)
		{
			// Create the var chain for this invocation and fill it
			auto varChain = create_user_var_chain<VarChainRoot, char(0)>(typename TermT::ArgTypes{}, tmp::type_list<ArgTs...>{});
			fill_user_var_chain<char(0)>(varChain, args...);
			auto nameList = get_user_var_chain_name_list<char(0)>(tmp::char_list<>{}, tmp::type_list<ArgTs...>{});

			return [=](const auto& out) mutable {
				auto argPack = create_arg_pack(typename TermT::ArgTypes{}, nameList, varChain);

				auto end = [&]() -> bool {
					output_unknowns<0>(tmp::char_list<>{}, tmp::type_list<ArgTs...>{}, out, argPack);
					return true;
				};

				TermT::satisfy(*this, argPack, end);
			};
		}
	};
}
