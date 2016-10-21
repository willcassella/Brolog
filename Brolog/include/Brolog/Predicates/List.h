// List.h
#pragma once

#include "../Brolog.h"

namespace brolog
{
	template <typename T>
	struct EmptyList
	{
		// Unifies the expression A = []
		using ArgTypes = tmp::type_list<std::vector<T>>;

		template <typename DBaseT, typename ContinueFnT>
		static bool satisfy(const DBaseT& database, const std::tuple<Var<std::vector<T>>*>& args, const ContinueFnT& next)
		{
			auto* a = std::get<0>(args);

			// If a has already been unified
			if (a->unified())
			{
				// Make sure it's empty
				if (!a->value().empty())
				{
					return false;
				}

				return next();
			}

			a->unify(std::vector<T>{});
			bool satisfied = next();
			a->unbind();
			return satisfied;
		}
	};

	template <typename T>
	struct ListMember
	{
		using ArgTypes = tmp::type_list<T, std::vector<T>>;

		template <typename DBaseT, typename ContinueFnT>
		static bool satisfy(const DBaseT& /*database*/, const std::tuple<Var<T>*, Var<std::vector<T>>*>& args, const ContinueFnT& next)
		{
			auto* a = std::get<0>(args);
			auto* b = std::get<1>(args);

			// We can only unify if B has been unified
			if (!b->unified())
			{
				return false;
			}

			// If A has already been unified, we only have to check if it exists in B
			if (!a->unified())
			{
				if (b->value().find(a->value()) == b->value().end())
				{
					return false;
				}

				return next();
			}

			// A has not been unified, so just bind it to every member of B
			bool satisfied = false;
			for (const auto& element : b->value())
			{
				a->unify(element);
				satisfied |= next();
				a->unbind();
			}

			return satisfied;
		}
	};

	template <typename T>
	struct ListFront
	{
		// Unifies the expression [A...] = [B, C...]
		using ArgTypes = tmp::type_list<std::vector<T> /*A*/, T /*B*/, std::vector<T> /*C*/>;

		template <typename DBaseT, typename ContinueFnT>
		static bool satisfy(const DBaseT& database, const std::tuple<Var<T>*, Var<std::vector<T>>*, Var<std::vector<T>>*>& args, const ContinueFnT& next)
		{
			auto* a = std::get<0>(args);
			auto* b = std::get<1>(args);
			auto* c = std::get<2>(args);

			// If A has been unified
			if (a->unified())
			{
				// We can only unify B and C is A is not empty
				if (a->value().empty())
				{
					return false;
				}

				// If c has been unified
				if (c->unified())
				{
					// Make sure A and C are of compatible length
					if (a->value().size() != c->value().size() + 1)
					{
						return false;
					}

					// Check make sure C matches A - 1
					for (std::size_t i = 0; i < c->value().size(); ++i)
					{
						if (a->value()[i + 1] != c->value()[i])
						{
							return false;
						}
					}

					// If b has been unified
					if (b->unified())
					{
						// Make sure it's equal to the first element of A
						if (b->value() != a->value().front())
						{
							return false;
						}

						return next();
					}
					else
					{
						// Unify b with the first element of a
						b->unify(a->value().front());
						bool satisfied = next();
						b->unbind();
						return satisfied;
					}
				}
				else
				{
					// Add everything except the first element of a to c
					std::vector<T> value;
					value.reserve(a->value().size() - 1);

					for (std::size_t i = 1; i < a->value().size(); ++i)
					{
						value.push_back(a->value()[i]);
					}

					// Unify c with the value we created
					c->unify(value);

					// If b has been unified
					bool satisfied = false;
					if (b->unified())
					{
						b->unify(a->value().front());
						satisfied = next();
						b->unbind();
					}
					else
					{
						satisfied = next();
					}

					c->unbind();
					return satisfied;
				}
			}
			else
			{
				// We can only unify A if b and C have been unified
				if (!b->unified() || !c->unified())
				{
					return false;
				}

				// Create a value for a
				std::vector<T> value;
				value.reseve(c->value().size() + 1);

				value.push_back(b->value());
				for (const auto& element : c->value())
				{
					value.push_back(element);
				}

				// Unify a with the value we created
				a->unify(value);
				auto satisfied = next();
				a->unbind();
				return satisfied;
			}
		}
	};
}
