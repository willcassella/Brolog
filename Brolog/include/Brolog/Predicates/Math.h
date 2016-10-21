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
		static bool satisfy(const DBaseT& /*dbase*/, const std::tuple<Var<T>*, Var<T>*, Var<T>*>& args, const ContinueFnT& next)
		{
			auto* a = std::get<0>(args);
			auto* b = std::get<1>(args);
			auto* c = std::get<2>(args);

			// If a is not unified
			if (!a->unified())
			{
				// Try to unify a with b and c
				if (!b->unified() || !c->unified())
				{
					return false;
				}

				a->unify(b->value() + c->value());
				bool satisfied = next();
				a->unbind();
				return satisfied;
			}

			// If b has not been unified
			if (!b->unified())
			{
				// Try to unify b with a and c
				if (!a->unified() || !c->unified())
				{
					return false;
				}

				b->unify(a->value() - c->value());
				bool satisfied = next();
				b->unbind();
				return satisfied;
			}

			// If c has not been unified
			if (!c->unified())
			{
				// Try to unify c with a and b
				if (!a->unified() || !b->unified())
				{
					return false;
				}

				c->unify(a->value() - c->value());
				bool satisfied = next();
				c->unbind();
				return satisfied;
			}

			// They've all been unified, make sure the sum is correct
			if (a->value() == b->value() + c->value())
			{
				return next();
			}

			return false;
		}
	};

	template <typename T, T V>
	struct ConstantSum
	{
		using ArgTypes = tmp::type_list<T, T>;

		template <typename DBaseT, typename ContinueFnT>
		static bool satisfy(const DBaseT& dbase, const std::tuple<Var<T>*, Var<T>*>& args, const ContinueFnT& next)
		{
			// Create a third argument and unify it with the constant
			StoredVarChainElement<T, char(0)> c;
			c.unify(V);

			return Sum<T>::satisfy(dbase, std::make_tuple(std::get<0>(args), std::get<1>(args), &c), next);
		}
	};

	template <typename T>
	using Equal = ConstantSum<T, 0>;
}
