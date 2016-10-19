// main.cpp

#include <iostream>
#include "../include/Brolog/Brolog.h"
#include "../include/Brolog/VarChain.h"

using FNeighbor = brolog::FactType<struct Neighbor, int, int>;

struct EmptyVars {};

int main()
{
	brolog::DataBase<FNeighbor> dataBase;
	dataBase.insert<FNeighbor>(1, 2);
	dataBase.insert<FNeighbor>(2, 2);
	dataBase.insert<FNeighbor>(1, 4);
	dataBase.insert<FNeighbor>(2, 3);
	dataBase.insert<FNeighbor>(3, 4);

	EmptyVars emptyVars{};
	auto argPack = brolog::create_arg_pack(FNeighbor::ArgTypes{}, brolog::tmp::char_list<'X', 'X'>{}, emptyVars);

	FNeighbor::satisfy(dataBase, argPack, []()
	{
		int test = 3;
	});

	std::cin.get();
}
