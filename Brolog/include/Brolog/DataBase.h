// DataBase.h
#pragma once

#include "ArgPack.h"

namespace brolog
{
	template <typename DBase, typename TermT>
	struct DataBaseElement;

	template <typename ... TermTs>
	struct DataBase : DataBaseElement<DataBase<TermTs...>, TermTs>...
	{
		template <typename FactT, typename ... Args>
		void insert_fact(Args&& ... args)
		{
			FactT::make_instance(*this, std::forward<Args>(args)...);
		}

		template <typename RuleT>
		void insert_rule()
		{
			typename RuleT::Type::template make_instance<RuleT>(*this);
		}

		template <typename TermT, typename ... ArgTs>
		auto satisfy(const ArgTs& ... args)
		{
			// Create the var chain for this invocation and fill it
			auto varChain = create_user_var_chain<VarChainRoot, char(0)>(typename TermT::ArgTypes{}, tmp::type_list<ArgTs...>{});
			fill_user_var_chain<char(0)>(varChain, args...);
			auto nameList = get_user_var_chain_name_list<char(0)>(tmp::char_list<>{}, tmp::type_list<ArgTs...>{});

			return [=](const auto& end) mutable {
				auto argPack = create_arg_pack(typename TermT::ArgTypes{}, nameList, varChain);
				TermT::satisfy(*this, argPack, end);
			};
		}
	};
}
