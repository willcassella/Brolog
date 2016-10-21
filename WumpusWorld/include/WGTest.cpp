//WGTest.cpp
#include "../include/World.h"
int main()
{
	World test(10, 0.1, 0.03, 0.04);
	test;
	Coordinate start = test.get_start();
	Coordinate gold = test.get_gold();
	test.getInfo(start);
	test.getInfo(gold);
}