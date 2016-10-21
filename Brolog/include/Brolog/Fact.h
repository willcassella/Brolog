// Fact.h - Copyright (c) 2016 Will Cassella
#pragma once

#include <set>
#include "ArgPack.h"
#include "Enumerator.h"
#include "DataBase.h"

namespace brolog
{
	template <typename CookieT, typename ... ArgTs>
	struct FactType
	{
		/* The type of object for this term that is stored in the database. */
		using Instance = std::tuple<ArgTs...>;

		using ArgTypes = tmp::type_list<ArgTs...>;

		template <typename DBaseT, typename ContinueFnT>
		static bool satisfy(const DBaseT& dataBase, const std::tuple<Var<ArgTs>*...>& args, const ContinueFnT& next)
		{
			const auto& instances = static_cast<const DataBaseElement<DBaseT, FactType>&>(dataBase).instances;
			bool initiallyUnified = arg_pack_unified<0>(args);
			bool satisfied = false;

			// While we haven't run out of facts, and we haven't found a fact that matches the arguments (if it the arguments were initally unified)
			for (auto fact = instances.begin(); fact != instances.end() && !(initiallyUnified && satisfied); ++fact)
			{
				// Unify the argumentss
				satisfied |= unify_arg_pack(args, *fact, next);
			}

			return satisfied;
		}

		/* Creates a new instance of this fact and inserts it into the database. */
		template <typename DBaseT>
		static void make_instance(DBaseT& dataBase, ArgTs ... values)
		{
			static_cast<DataBaseElement<DBaseT, FactType>&>(dataBase).instances.insert(std::make_tuple(std::forward<ArgTs>(values)...));
		}
	};

	template <typename DBase, typename Cookie, typename ... Ts>
	struct DataBaseElement < DBase, FactType<Cookie, Ts...> >
	{
		std::set<typename FactType<Cookie, Ts...>::Instance> instances;
	};
}
