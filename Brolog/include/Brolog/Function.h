// Function.h
#pragma once

#include <utility>
#include <type_traits>

namespace brolog
{
	template <typename Fn>
	struct Function;

	template <typename Ret, typename ... Args>
	struct Function < Ret(Args...) >
	{
		union UserData
		{
			const void* lambda;
			Ret(*func)(Args...);
		};
		using InvokerFn = Ret(UserData, Args...);

		////////////////////////
		///   Constructors   ///
	public:

		template <typename Fn>
		Function(Fn&& func)
		{
			_user_data.lambda = &func;
			_invoker = [](UserData userData, Args ... args) -> Ret {
				return (*static_cast<const std::remove_reference_t<Fn>*>(userData.lambda))(std::forward<Args>(args)...);
			};
		}

		Function(Function& copy)
			: _user_data(copy._user_data),
			_invoker(copy._invoker)
		{
		}

		Function(const Function& copy)
			: _user_data(copy._user_data),
			_invoker(copy._invoker)
		{
		}

		Function(Function&& move)
			: _user_data(move._user_data),
			_invoker(move._invoker)
		{
		}

		Function(const Function&& move)
			: _user_data(move._user_data),
			_invoker(move._invoker)
		{
		}

		/////////////////////
		///   Operators   ///
	public:

		Ret operator()(Args ... args) const
		{
			return _invoker(_user_data, std::forward<Args>(args)...);
		}

		//////////////////
		///   Fields   ///
	private:

		UserData _user_data;
		InvokerFn* _invoker;
	};
}