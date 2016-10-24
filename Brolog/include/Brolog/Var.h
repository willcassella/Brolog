// Var.h - Copyright (c) 2016 Will Cassella
#pragma once

namespace brolog
{
	template <typename T>
	struct Var
	{
		///////////////////
		///   Methods   ///
	public:

		/* Returns whether this variable has already been unified. */
		virtual bool unified() const = 0;

		/* Returns the unified value of this variable.
		 * NOTE: Check 'unified' before calling this function. */
		virtual const T& value() const = 0;

		/* Unifies this variable with the given value. */
		virtual void unify(const T& value) = 0;

		/* Deunifies this variable.
		 * You should only call this function if you were the caller to 'unify'. */
		virtual void unbind() = 0;
	};
}
