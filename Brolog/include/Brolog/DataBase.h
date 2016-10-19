// DataBase.h
#pragma once

namespace brolog
{
	template <typename DBase, typename TermT>
	struct DataBaseElement;

	template <typename ... TermTs>
	struct DataBase : DataBaseElement<DataBase<TermTs...>, TermTs>...
	{
		template <typename TermT, typename ... Args>
		void insert(Args&& ... args)
		{
			TermT::make_instance(*this, std::forward<Args>(args)...);
		}

		template <typename TermT, typename SatisfyArgT, typename EnumeratorFn>
		std::size_t satisfy(SatisfyArgT& arg, const EnumeratorFn& enumerator) const
		{
			return TermT::satisfy(*this, arg, enumerator);
		}
	};
}
