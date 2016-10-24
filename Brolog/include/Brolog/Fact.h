// Fact.h - Copyright (c) 2016 Will Cassella
#pragma once

#include <set>
#include "ArgPack.h"
#include "DataBase.h"

namespace brolog
{
	/* Type defining a 'Fact Type'.
	 * 'CookieT' - Any unique type, not necesarily defined. Used to disambiguate this FacType from any others that
	 * take the same arguments. */
	template <typename CookieT, typename ... ArgTs>
	struct FactType
	{
		/* The type of object that is stored in the database for each instance of this FactType. */
		using Instance = std::tuple<ArgTs...>;

		/* The list of argument types required to satisfy this FactType. */
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

		/* Removes an instance of this fact from the database. */
		template <typename DBaseT>
		static void remove_instance(DBaseT& database, ArgTs ... values)
		{
			auto& instances = static_cast<DataBaseElement<DBaseT, FactType>&>(database).instances;
			auto instance = std::make_tuple(std::forward<ArgTs>(values)...);

			auto pos = instances.find(instance);
			if (pos != instances.end())
			{
				instances.erase(pos);
			}
		}
	};

	template <typename DBase, typename Cookie, typename ... Ts>
	struct DataBaseElement < DBase, FactType<Cookie, Ts...> >
	{
		std::set<typename FactType<Cookie, Ts...>::Instance> instances;
	};
}
