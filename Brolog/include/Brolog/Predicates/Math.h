// Math.h
#pragma once

#include "../Rule.h"

namespace brolog
{
	template <typename T>
	struct Sum
	{
		using ArgTypes = tmp::type_list<T, T, T>;

		template <typename DBaseT, typename ContinueFnT>
		static void satisfy(const DBaseT& /*dbase*/, const std::tuple<Var<T>*, Var<T>*, Var<T>*>& args, const ContinueFnT& next)
		{
			auto* a = std::get<0>(args);
			auto* b = std::get<1>(args);
			auto* c = std::get<2>(args);

			if (!b->unified())
			{
				return;
			}

			if (!c->unified())
			{
				return;
			}

			// If the first argument already has a value
			if (a->unified())
			{
				if (a->value() != b->value() + c->value())
				{
					return;
				}
				else
				{
					next();
				}
			}
			else
			{
				a->unify(b->value() + c->value());
				next();
				a->unbind();
			}
		}
	};

	template <typename T, T V>
	struct ConstantSum
	{
		using ArgTypes = tmp::type_list<T, T>;

		template <typename DBaseT, typename ContinueFnT>
		static void satisfy(const DBaseT& dbase, const std::tuple<Var<T>*, Var<T>*>& args, const ContinueFnT& next)
		{
			// Create a third argument and unify it with the constant
			StoredVarChainElement<T, char(0)> c;
			c.unify(V);

			Sum<T>::satisfy(dbase, std::make_tuple(std::get<0>(args), std::get<1>(args), &c), next);
		}
	};
}
