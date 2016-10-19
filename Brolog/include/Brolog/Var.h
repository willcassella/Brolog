// Var.h
#pragma once

#include <tuple>
#include "TMP.h"

namespace brolog
{
	template <typename T>
	struct Var
	{
		////////////////////////
		///   Constructors   ///
	public:

		Var()
			: _unified(false)
		{
		}
		Var(const T& value)
			: _unified(true), _value(value)
		{
		}
		Var(const Var& copy)
			: _unified(copy._unified)
		{
			if (unified())
			{
				_value = copy._value;
			}
		}

		///////////////////
		///   Methods   ///
	public:

		bool unified() const
		{
			return _unified;
		}

		void unify(const T& value)
		{
			_value = value;
			_unified = true;
		}

		const T& value() const
		{
			return _value;
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

	template <typename ... Ts>
	struct ArgPack
	{
		using TupleType = std::tuple<Var<Ts>*...>;

		///////////////////
		///   Methods   ///
	public:

		virtual TupleType get_args() = 0;

		virtual void reset() = 0;
	};
}
