// main.cpp

#include "../include/Brolog/Brolog.h"
#include "../include/Brolog/Predicates/Math.h"
#include <iostream>

using FNotBreeze = brolog::FactType<struct NotBreeze, int, int>;
using FNotStench = brolog::FactType<struct NotStench, int, int>;
using FBump = brolog::FactType<struct Bump, int, int>;

using RNotPit = brolog::RuleType<struct NotPit, int, int>;
using RNotWumpus = brolog::RuleType<struct NotWumpus, int, int>;

using WumpusDB = brolog::DataBase<FNotBreeze, FNotStench, FBump, RNotPit, RNotWumpus>;

int main()
{
	using namespace brolog;

	auto database = WumpusDB{};

	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<FNotBreeze, 'X', 'Y'>>();

	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<FBump, 'X', 'Y'>>();

	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<FNotBreeze, 'X', 'A'>,
		Satisfy<ConstantSum<int, 1>, 'Y', 'A'>>();

	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<FNotBreeze, 'X', 'A'>,
		Satisfy<ConstantSum<int, -1>, 'Y', 'A'>>();

	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<FNotBreeze, 'A', 'Y'>,
		Satisfy<ConstantSum<int, 1>, 'X', 'A'>>();

	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<FNotBreeze, 'A', 'Y'>,
		Satisfy<ConstantSum<int, -1>, 'X', 'A'>>();

	database.insert_fact<FBump>(-1, 0);
	database.insert_fact<FBump>(-1, 1);
	database.insert_fact<FBump>(0, -1);
	database.insert_fact<FBump>(1, -1);
	database.insert_fact<FNotBreeze>(0, 0);

	auto query = database.satisfy<RNotPit>(Unknown<'X'>(), Unknown<'Y'>());
	query([](auto x, auto y)
	{
		std::cout << "X = " << x << ", Y = " << y << std::endl;
	});

	std::cin.get();
}
