// VarChain.h
#pragma once

#include "Var.h"

namespace brolog
{
	template <char Name>
	struct VarName {};

	template <typename T, char Name>
	struct VarChainElement : VarName<Name>
	{
		//////////////////
		///   Fields   ///
	public:

		Var<T> value;
	};

	namespace impl
	{
		template <bool ExistingVar, typename VarChain, typename T, char N>
		struct AddVarChainElement : VarChain
		{
			static_assert(std::is_base_of<VarChainElement<T, N>, VarChain>::value,
				"Two instances of same var name with different types is not allowed.");

			////////////////////////
			///   Constructors   ///
		public:

			AddVarChainElement() = default;

			template <typename InitVarChain>
			AddVarChainElement(const InitVarChain& varChain)
				: VarChain(varChain)
			{
			}
		};

		template <typename VarChain, typename T, char N>
		struct AddVarChainElement< false, VarChain, T, N > : VarChain, VarChainElement<T, N>
		{
			////////////////////////
			///   Constructors   ///
		public:

			AddVarChainElement<false, VarChain, T, N>() = default;

			AddVarChainElement<false, VarChain, T, N>(const AddVarChainElement<false, VarChain, T, N>& copy) = default;

			template <typename InitVarChain>
			AddVarChainElement(const InitVarChain& varChain)
				: VarChain(varChain)
			{
			}
		};

		template <typename VarChain, typename Types, typename Names>
		struct VarChainBuilder;

		template <typename VarChain, typename T, typename ... Ts, char N, char ... Ns>
		struct VarChainBuilder< VarChain, tmp::type_list<T, Ts...>, tmp::char_list<N, Ns...> >
		{
			using result = typename VarChainBuilder<
				AddVarChainElement<std::is_base_of<VarName<N>, VarChain>::value, VarChain, T, N>,
				tmp::type_list<Ts...>,
				tmp::char_list<Ns...>>::result;
		};

		template <typename VarChain>
		struct VarChainBuilder< VarChain, tmp::type_list<>, tmp::char_list<> >
		{
			using result = VarChain;
		};
	}

	template <typename ... Ts, char ... Ns, typename VarChain>
	auto create_arg_pack(tmp::type_list<Ts...>, tmp::char_list<Ns...>, const VarChain& varChain)
	{
		struct Result final : ArgPack<Ts...>
		{
			using VarChainT = typename impl::VarChainBuilder<VarChain, tmp::type_list<Ts...>, tmp::char_list<Ns...>>::result;

			////////////////////////
			///   Constructors   ///
		public:

			Result(const VarChain& outerVarChain)
				: outer_var_chain(&outerVarChain),
				var_chain(outerVarChain)
			{
			}

			///////////////////
			///   Methods   ///
		public:

			typename ArgPack<Ts...>::TupleType get_args() override
			{
				return std::make_tuple(&(static_cast<VarChainElement<Ts, Ns>&>(var_chain).value)...);
			}

			void reset() override
			{
				var_chain = *outer_var_chain;
			}

			//////////////////
			///   Fields   ///
		public:

			const VarChain* outer_var_chain;
			VarChainT var_chain;
		};

		return Result(varChain);
	}
}
