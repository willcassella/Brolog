// main.cpp

#include "../include/World.h"
#include "../include/KnowledgeDB.h"

int main()
{
	World test(10, 0.1f, 0.03f, 0.04f);

	Coordinate start = test.get_start();
	Coordinate gold = test.get_gold();
	test.getInfo(start);
	test.getInfo(gold);
}