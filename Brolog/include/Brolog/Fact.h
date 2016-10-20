// Fact.h
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
		static void satisfy(const DBaseT& dataBase, const std::tuple<Var<ArgTs>*...>& args, const ContinueFnT& next)
		{
			const auto& instances = static_cast<const DataBaseElement<DBaseT, FactType>&>(dataBase).instances;
			auto control = EControl::CONTINUE;

			// While we haven't run out of facts, and the enumerator wants us to continue
			for (auto fact = instances.begin(); fact != instances.end() && control != EControl::BREAK; ++fact)
			{
				// Unify the argumentss
				unify_arg_pack(args, *fact, next);
			}
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
