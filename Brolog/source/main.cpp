// main.cpp

#include <iostream>
#include "../include/Brolog/Brolog.h"

using FNeighbor = brolog::FactType<struct Neighbor, int, int>;

int main()
{
	brolog::DataBase<FNeighbor> dataBase;
	dataBase.insert<FNeighbor>(1, 2);
	dataBase.insert<FNeighbor>(1, 4);
	dataBase.insert<FNeighbor>(2, 3);
	dataBase.insert<FNeighbor>(3, 4);

	FNeighbor::SatisfyArgT incomplete = std::make_tuple(brolog::Var<int>(), brolog::Var<int>(4));
	dataBase.satisfy<FNeighbor>(incomplete, [](const FNeighbor::InstanceT& fact)
	{
		int test = 3;
	});

	std::cout << "Test" << std::endl;
	std::cin.get();
}
