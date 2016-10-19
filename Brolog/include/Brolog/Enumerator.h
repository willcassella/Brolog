// Enumerator.h
#pragma once

#include <type_traits>

namespace brolog
{
	enum class EControl
	{
		/* The calling function should not continue to call the enumerator. */
		BREAK = 0,
		CONTINUE = 1
	};

	template <typename T>
	struct Enumerator
	{
		using UserData = const void*;
		using InvokerFn = EControl(UserData, const T&);

		////////////////////////
		///   Constructors   ///
	public:

		template <typename Fn>
		Enumerator(Fn&& func)
		{
			_user_data = &func;
			_invoker = [](UserData userData, const T& value) -> EControl {
				(*static_cast<std::remove_reference_t<Fn>*>(userData))(value);
				return EControl::CONTINUE;
			};
		}

		/////////////////////
		///   Operators   ///
	public:

		EControl operator()(const T& value) const
		{
			return _invoker(_user_data, value);
		}

		//////////////////
		///   Fields   ///
	private:

		UserData _user_data;
		InvokerFn* _invoker;
	};

	struct Continuator
	{
		using UserData = const void*;
		using InvokerFn = EControl(UserData);

		////////////////////////
		///   Constructors   ///
	public:

		template <typename Fn>
		Continuator(Fn&& fn)
		{
			_user_data = &fn;
			_invoker = [](UserData userData) -> EControl {
				(*static_cast<const std::remove_reference_t<Fn>*>(userData))();
				return EControl::CONTINUE;
			};
		}

		///////////////////
		///   Methods   ///
	public:

		EControl next() const
		{
			return _invoker(_user_data);
		}

		//////////////////
		///   Fields   ///
	private:

		UserData _user_data;
		InvokerFn* _invoker;
	};
}
