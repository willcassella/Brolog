// Fact.h
#pragma once

#include <tuple>
#include <type_traits>
#include "Var.h"
#include "Enumerator.h"
#include "DataBase.h"

namespace brolog
{
	template <typename Cookie, typename ... Ts>
	struct FactType
	{
		/* The type of object for this term that is stored in the database. */
		using InstanceT = std::tuple<Ts...>;

		using ArgTypes = tmp::type_list<Ts...>;

		using ArgTupleT = typename ArgPack<Ts...>::TupleType;

		template <typename DBase>
		static void satisfy(const DBase& dataBase, ArgPack<Ts...>& argPack, Continuator continuator)
		{
			const auto& instances = static_cast<const DataBaseElement<DBase, FactType>&>(dataBase).instances;
			auto factIterator = instances.begin();
			auto control = EControl::CONTINUE;
			auto args = argPack.get_args();

			// While we haven't run out of facts, and the enumerator wants us to continue
			for (; factIterator != instances.end() && control != EControl::BREAK; ++factIterator)
			{
				// If we can satisfy the current fact instance with the given argument
				if (satisfy_instance(std::integral_constant<std::size_t, 0>{}, *factIterator, args))
				{
					control = continuator.next();
				}

				argPack.reset();
			}
		}

		/* Creates a new instance of this fact and inserts it into the database. */
		template <typename DBase>
		static void make_instance(DBase& dataBase, Ts ... values)
		{
			static_cast<DataBaseElement<DBase, FactType>&>(dataBase).instances.insert(std::make_tuple(values...));
		}

	private:

		/* Attempts to satisfy the given instance of this fact type one member at a time. */
		template <std::size_t I>
		static bool satisfy_instance(std::integral_constant<std::size_t, I>, const InstanceT& fact, ArgTupleT& args)
		{
			// If the argument for this member hasn't been unified
			if (!std::get<I>(args)->unified())
			{
				// Unify it
				std::get<I>(args)->unify(std::get<I>(fact));
			}
			else if (std::get<I>(args)->value() != std::get<I>(fact))
			{
				return false;
			}

			// Attempt to satisfy the next member
			return satisfy_instance(std::integral_constant<std::size_t, I + 1>{}, fact, args);
		}

		static bool satisfy_instance(std::integral_constant<std::size_t, sizeof...(Ts)>, const InstanceT& /*fact*/, ArgTupleT& /*arg*/)
		{
			// No more members of the fact to satisfy, so we're done
			return true;
		}
	};

	template <typename DBase, typename Cookie, typename ... Ts>
	struct DataBaseElement < DBase, FactType<Cookie, Ts...> >
	{
		std::set<typename FactType<Cookie, Ts...>::InstanceT> instances;
	};
}
