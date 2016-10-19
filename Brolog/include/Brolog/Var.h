// Var.h
#pragma once

namespace brolog
{
	template <typename T>
	struct Var
	{
		///////////////////
		///   Methods   ///
	public:

		virtual bool unified() const = 0;

		virtual const T& value() const = 0;

		virtual void unify(const T& value) = 0;

		virtual void unbind() = 0;
	};
}
