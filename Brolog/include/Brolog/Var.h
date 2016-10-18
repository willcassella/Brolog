// Var.h
#pragma once

namespace brolog
{
	template <typename T>
	struct Var
	{
		////////////////////////
		///   Constructors   ///
	public:

		Var()
			: _has_value(false)
		{
		}
		Var(const T& value)
			: _has_value(true), _value(value)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		bool has_value() const
		{
			return _has_value;
		}

		const T& get_value() const
		{
			return _value;
		}

		//////////////////
		///   Fields   ///
	private:

		bool _has_value;
		union
		{
			T _value;
		};
	};
}
