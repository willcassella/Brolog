// VarChain.h
#pragma once

#include <cassert>
#include <tuple>
#include <vector>
#include "TMP.h"
#include "Var.h"

namespace brolog
{
	template <char N>
	struct VarName {};

	template <typename T, char N>
	struct VarChainElement : Var<T>, VarName<N>
	{
	};

	template <typename T, char Name>
	struct StoredVarChainElement : VarChainElement<T, Name>
	{
		////////////////////////
		///   Constructors   ///
	public:

		StoredVarChainElement()
			: _unified(false)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		bool unified() const final override
		{
			return _unified;
		}

		const T& value() const final override
		{
			assert(_unified);
			return _value;
		}

		void unify(const T& value) final override
		{
			assert(!_unified);
			_value = value;
			_unified = true;
		}

		void unbind() final override
		{
			assert(_unified);
			_unified = false;
		}

		//////////////////
		///   Fields   ///
	private:

		bool _unified;
		union
		{
			T _value;
		};
	};

	template <typename T, char Name>
	struct ReferencedVarChainElement : VarChainElement<T, Name>
	{
		////////////////////////
		///   Constructors   ///
	public:

		~ReferencedVarChainElement()
		{
			// Unbind all previously unbound vars, since the stack is unwinding
			for (auto var : _unbound)
			{
				if (var->unified())
				{
					var->unbind();
				}
			}
		}

		///////////////////
		///   Methods   ///
	public:

		bool unified() const final override
		{
			return !_vars.empty() && _vars.front()->unified();
		}

		const T& value() const final override
		{
			assert(this->unified());
			return _vars.front()->value();
		}

		void unify(const T& value) final override
		{
			assert(!this->unified());
			assert(!_vars.empty());

			for (auto var : _vars)
			{
				if (!var->unified())
				{
					var->unify(value);
				}
			}
		}

		void unbind() final override
		{
			// You can only unbind this if all vars were original unbound
			assert(_vars.size() == _unbound.size());
			for (auto var : _vars)
			{
				if (var->bound())
				{
					var->unbind();
				}
			}
		}

		bool add_var(Var<T>* var)
		{
			// If we've already been unified (by previously adding a unified variable)
			if (this->unified())
			{
				// If the variable has not been unified
				if (!var->unified())
				{
					// Unify it, and put it in
					var->unify(this->value());
					_unbound.push_back(var);
				}
				else
				{
					// Make sure the variable's value works with this var's value
					if (var->value() != this->value())
					{
						// Unification failed
						return false;
					}
				}
			}
			else
			{
				// If the variable we're adding has not been unified
				if (!var->unified())
				{
					_unbound.push_back(var);
				}
				else
				{
					// Try to unify all variables with the new variable's value
					for (auto existingVar : _vars)
					{
						// If we have a duplicate variable, unifying a previous one might unify a future one
						if (!existingVar->unified())
						{
							existingVar->unify(var->value());
						}
					}
				}
			}

			// Add it to the list of variables
			_vars.push_back(var);
			return true;
		}

		//////////////////
		///   Fields   ///
	private:

		/* Array of all vars referenced in this element. */
		std::vector<Var<T>*> _vars;

		/* Array of vars that were originally unbound. */
		std::vector<Var<T>*> _unbound;
	};

	namespace impl
	{
		template <
			bool ExistingVar,
			typename ChainT,
			typename T, char N,
			template <typename TypeT, char Name> class Element>
		struct AppendVarChain
		{
			static_assert(std::is_base_of<VarChainElement<T, N>, ChainT>::value,
				"Two occurrences of the same var name with different types is not allowed.");

			using result = ChainT;
		};

		template <
			typename ChainT,
			typename T, char N,
			template <typename TypeT, char Name> class Element>
		struct AppendVarChain< false, ChainT, T, N, Element>
		{
			struct result : Element<T, N>, ChainT {};
		};
	}

	struct VarChainRoot {};

	template <typename VarChainT, template <typename T, char N> class Element, typename T, typename ... Ts, char N, char ... Ns, typename ... OuterChainTs>
	auto create_var_chain(tmp::type_list<T, Ts...>, tmp::char_list<N, Ns...>, OuterChainTs& ... outers)
	{
		return create_var_chain<
			typename impl::AppendVarChain<
				tmp::is_base_of_any<VarName<N>, VarChainT, OuterChainTs...>::value,
				VarChainT, T, N, Element>::result, Element>
			(tmp::type_list<Ts...>{},
			tmp::char_list<Ns...>{},
			outers...);
	}

	template <typename VarChainT, template <typename T, char N> class Element, typename ... OuterChainTs>
	auto create_var_chain(tmp::type_list<>, tmp::char_list<>, OuterChainTs& ... /*outers*/)
	{
		return VarChainT();
	}

	template <std::size_t I, typename T, typename ... Ts, char N, char ... Ns, typename ArgPackT, typename VarChainT>
	bool fill_initial_arg_chain(tmp::type_list<T, Ts...>, tmp::char_list<N, Ns...>, const ArgPackT& argPack, VarChainT& varChain)
	{
		if (!static_cast<ReferencedVarChainElement<T, N>&>(varChain).add_var(std::get<I>(argPack)))
		{
			return false;
		}

		return fill_initial_arg_chain<I + 1>(tmp::type_list<Ts...>{}, tmp::char_list<Ns...>{}, argPack, varChain);
	}

	template <std::size_t I, typename ArgPackT, typename VarChainT>
	bool fill_initial_arg_chain(tmp::type_list<>, tmp::char_list<>, const ArgPackT& /*argPack*/, VarChainT& /*varChain*/)
	{
		return true;
	}
}
