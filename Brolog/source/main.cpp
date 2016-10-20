// main.cpp - Copyright (c) 2016 Will Cassella

#include <iostream>
#include "../include/Brolog/Brolog.h"

using FNeighbor = brolog::FactType<struct Neighbor, char, char>;

using RConnected = brolog::RuleType<struct Connected, char, char>;

void print()
{
	std::cout << "Instance found" << std::endl;
}

int main()
{
	brolog::DataBase<FNeighbor, RConnected> dataBase;
	dataBase.insert_fact<FNeighbor>('a', 'b');
	dataBase.insert_fact<FNeighbor>('b', 'c');
	dataBase.insert_fact<FNeighbor>('c', 'd');

	dataBase.insert_rule<brolog::Rule<RConnected, brolog::Params<'X', 'Y'>,
		brolog::Satisfy<FNeighbor, 'X', 'Y'>>>();

	dataBase.insert_rule<brolog::Rule<RConnected, brolog::Params<'X', 'Y'>,
		brolog::Satisfy<FNeighbor, 'Y', 'X'>>>();

	dataBase.insert_rule<brolog::Rule<RConnected, brolog::Params<'X', 'Y'>,
		brolog::Satisfy<FNeighbor, 'X', 'Z'>,
		brolog::Satisfy<RConnected, 'Z', 'Y'>>>();

	dataBase.satisfy<RConnected>(brolog::Unknown<'X'>(), 'd')(&print);

	std::cin.get();
}
