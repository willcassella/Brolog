// main.cpp

#include <iostream>
#include "../include/Brolog/Brolog.h"

using FNeighbor = brolog::FactType<struct Neighbor, int, int>;
//
//using RConnected = brolog::RuleType<struct Connected, int, int>;
//
//using Connected_1 = brolog::Rule<RConnected, brolog::Params<'X', 'Y'>,
//	brolog::Satisfy<FNeighbor, 'X', 'X'>>;

int main()
{
	brolog::DataBase<FNeighbor> dataBase;
	dataBase.insert<FNeighbor>(1, 2);
	dataBase.insert<FNeighbor>(2, 2);
	dataBase.insert<FNeighbor>(1, 4);
	dataBase.insert<FNeighbor>(2, 3);
	dataBase.insert<FNeighbor>(3, 4);
	dataBase.insert<FNeighbor>(10, 10);

	auto varPack = brolog::create_var_chain<brolog::VarChainRoot, brolog::StoredVarChainElement>(
		brolog::tmp::type_list<int, float>{}, brolog::tmp::char_list<'X', 'Y'>{});

	std::cin.get();
}
