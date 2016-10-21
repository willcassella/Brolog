// main.cpp

#include <iostream>
#include <Brolog/Brolog.h>
#include <Brolog/Predicates/Math.h>

/////////////////
///   Facts   ///

/* The X and Y coordinates of a tile in which a breeze was observed. */
using FBreeze = brolog::FactType<struct CBreeze, int, int>;

/* The X and Y coordinates of a tile in which a stench was observed. */
using FStench = brolog::FactType<struct CStench, int, int>;

/* The X and Y coordinates of a tile in which an obstacle was observed. */
using FObstacle = brolog::FactType<struct CObstacle, int, int>;

/* The X and Y coordinates of a tile in which nothing was observed. */
using FEmpty = brolog::FactType<struct CEmpty, int, int>;

/////////////////
///   Rules   ///

/* The X and Y coordinates of all tiles that the agent has been too. */
using RVisited = brolog::RuleType<struct CVisited, int, int>;

/* The X and Y coordinates of all VISITED tiles in which no breeze was observed. */
using RNoBreeze = brolog::RuleType<struct CNoBreeze, int, int>;

/* The X and Y coordinates of all VISITED tiles in which no stench was observed. */
using RNoStench = brolog::RuleType<struct CNoStench, int, int>;

/* The X and Y coordinates of all tiles in which there definately IS NOT a pit. */
using RNotPit = brolog::RuleType<struct CNotPit, int, int>;

/* The X and Y coordinates of all tiles in which there definately IS a pit. */
using RPit = brolog::RuleType<struct CPit, int, int>;

/* The X and Y coordinates of all tiles in which there definately IS NOT a wumpus. */
using RNotWumpus = brolog::RuleType<struct CNotWumpus, int, int>;

/* The X and Y coordinates of all tiles in which there definately IS a wumpus. */
using RWumpus = brolog::RuleType<struct CWumpus, int, int>;

/* The X and Y coordinates of all tiles in which there definately is nothing that will kill you. */
using RSafe = brolog::RuleType<struct CSafe, int, int>;

/* The X and Y coordinates of all tiles in which there definately is nothing that will kill you, and we haven't been to yet. */
using RSafeUnexplored = brolog::RuleType<struct CSafeUnexplored, int, int>;

// Database type
using WumpusDB = brolog::DataBase<
	FBreeze,
	FStench,
	FObstacle,
	FEmpty,
	RVisited,
	RNoBreeze,
	RNoStench,
	RNotPit,
	RPit,
	RNotWumpus,
	RWumpus,
	RSafe,
	RSafeUnexplored>;

void add_visited_rules(WumpusDB& database)
{
	using namespace brolog;

	// We have visited a tile if we have recorded a breeze on it
	database.insert_rule<RVisited, Params<'X', 'Y'>,
		Satisfy<FBreeze, 'X', 'Y'>>();

	// We have visited a tile if we have recorded a stench on it
	database.insert_rule<RVisited, Params<'X', 'Y'>,
		Satisfy<FStench, 'X', 'Y'>>();

	// We have visited a tile if we have recorded an obstacle on it
	database.insert_rule<RVisited, Params<'X', 'Y'>,
		Satisfy<FObstacle, 'X', 'Y'>>();

	// We have visited a tile if we have recorded it as being empty
	database.insert_rule<RVisited, Params<'X', 'Y'>,
		Satisfy<FEmpty, 'X', 'Y'>>();
}

void add_no_breeze_rules(WumpusDB& database)
{
	using namespace brolog;

	// There is no breeze in a tile if we've visited it, and we haven't recorded a breeze or an obstacle
	database.insert_rule<RNoBreeze, Params<'X', 'Y'>,
		Satisfy<RVisited, 'X', 'Y'>,
		NotSatisfy<FBreeze, 'X', 'Y'>,
		NotSatisfy<FObstacle, 'X', 'Y'>>();
}

void add_no_stench_rules(WumpusDB& database)
{
	using namespace brolog;

	// There is no stench on a tile if we have visited it and we have not recorded a stench or an obstacle
	database.insert_rule<RNoStench, Params<'X', 'Y'>,
		Satisfy<RVisited, 'X', 'Y'>,
		NotSatisfy<FStench, 'X', 'Y'>,
		NotSatisfy<FObstacle, 'X', 'Y'>>();
}

void add_not_pit_rules(WumpusDB& database)
{
	using namespace brolog;

	// There is not a pit on a tile if there is a wumpus on the tile
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<RWumpus, 'X', 'Y'>>();

	// There is not a pit on the tile if there is not a breeze on the tile
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<RNoBreeze, 'X', 'Y'>>();

	// There is not a pit on the tile if there is an obstacle on the tile
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<FObstacle, 'X', 'Y'>>();

	// There is not a pit on the tile if there is not a breeze on the left
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<RNoBreeze, 'L', 'Y'>,
		Satisfy<ConstantSum<int, 1>, 'X', 'L'>>();

	// There is not a pit on the tile if there is not a breeze to the right
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<RNoBreeze, 'R', 'Y'>,
		Satisfy<ConstantSum<int, -1>, 'X', 'R'>>();

	// There is not a pit on the tile if there is not a breeze below
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<RNoBreeze, 'X', 'B'>,
		Satisfy<ConstantSum<int, 1>, 'Y', 'B'>>();

	// There is not a pit on the tile if there is not a breeze above
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<RNoBreeze, 'X', 'A'>,
		Satisfy<ConstantSum<int, -1>, 'Y', 'A'>>();
}

void add_not_wumpus_rules(WumpusDB& database)
{
	using namespace brolog;

	// There is not a wumpus on the tile if there is a pit on the tile
	database.insert_rule<RNotWumpus, Params<'X', 'Y'>,
		Satisfy<RPit, 'X', 'Y'>>();

	// There is not a wumpus on the tile if we've already been to it and did not record a stench
	database.insert_rule<RNotWumpus, Params<'X', 'Y'>,
		Satisfy<RNoStench, 'X', 'Y'>>();

	// There is not a wumpus on the tile if we've alaredy been to it and recorded an obstacle
	database.insert_rule<RNotWumpus, Params<'X', 'Y'>,
		Satisfy<FObstacle, 'X', 'Y'>>();

	// There is not a wumpus on the tile if there is not a stench to the left
	database.insert_rule<RNotWumpus, Params<'X', 'Y'>,
		Satisfy<RNoStench, 'L', 'Y'>,
		Satisfy<ConstantSum<int, 1>, 'X', 'L'>>();

	// There is not a wumpus on the tile if there is not a stench to the right
	database.insert_rule<RNotWumpus, Params<'X', 'Y'>,
		Satisfy<RNoStench, 'R', 'Y'>,
		Satisfy<ConstantSum<int, -1>, 'X', 'R'>>();

	// There is not a wumpus on the tile if there is not a stench below
	database.insert_rule<RNotWumpus, Params<'X', 'Y'>,
		Satisfy<RNoStench, 'X', 'B'>,
		Satisfy<ConstantSum<int, 1>, 'Y', 'B'>>();

	// There is not a wumpus on the tile if there is not a stench above
	database.insert_rule<RNotWumpus, Params<'X', 'Y'>,
		Satisfy<RNoStench, 'X', 'A'>,
		Satisfy<ConstantSum<int, -1>, 'Y', 'A'>>();
}

void add_wumpus_rules(WumpusDB& database)
{
	using namespace brolog;
	database.insert_rule<RWumpus, Params<'X', 'Y'>,
		Satisfy<FStench, 'S', 'Y'>, // There is a stench to the left of the tile
		Satisfy<ConstantSum<int, 1>, 'X', 'S'>, // X is S + 1
		Satisfy<ConstantSum<int, -1>, 'L', 'S'>, // L is S - 1
		Satisfy<ConstantSum<int, -1>, 'B', 'Y'>, // B is Y - 1
		Satisfy<ConstantSum<int, 1>, 'A', 'Y'>, // A is Y + 1
		Satisfy<RNotWumpus, 'S', 'A'>, // There is not a wumpus above the stench
		Satisfy<RNotWumpus, 'S', 'B'>, // There is not a wumpus below the stench
		Satisfy<RNotWumpus, 'L', 'Y'>>(); // There is not a wumpus to the left of the stench

	database.insert_rule<RWumpus, Params<'X', 'Y'>,
		Satisfy<FStench, 'S', 'Y'>, // There is a stench to the right of the tile
		Satisfy<ConstantSum<int, -1>, 'X', 'S'>, // X is S - 1
		Satisfy<ConstantSum<int, 1>, 'M', 'R'>, // R is S + 1
		Satisfy<ConstantSum<int, -1>, 'B', 'Y'>, // B is Y - 1
		Satisfy<ConstantSum<int, 1>, 'A', 'Y'>, // A is Y + 1
		Satisfy<RNotWumpus, 'S', 'A'>, // There is not a wumpus above the stench
		Satisfy<RNotWumpus, 'S', 'B'>, // There is not a wumpus below the stench
		Satisfy<RNotWumpus, 'R', 'Y'>>(); // There is not a wumpus to the right of the stench

	database.insert_rule<RWumpus, Params<'X', 'Y'>,
		Satisfy<FStench, 'X', 'S'>, // There is a stench below the tile
		Satisfy<ConstantSum<int, 1>, 'Y', 'S'>, // Y is S + 1
		Satisfy<ConstantSum<int, -1>, 'B', 'S'>, // B is S - 1
		Satisfy<ConstantSum<int, -1>, 'L', 'X'>, // L is X - 1
		Satisfy<ConstantSum<int, 1>, 'R', 'X'>, // R is X + 1
		Satisfy<RNotWumpus, 'X', 'B'>, // There is not a wumpus below the stench
		Satisfy<RNotWumpus, 'L', 'S'>, // There is not a wumpus to the left of the stench
		Satisfy<RNotWumpus, 'R', 'S'>>(); // There is not a wumpus to the right of the stench

	database.insert_rule<RWumpus, Params<'X', 'Y'>,
		Satisfy<FStench, 'X', 'S'>, // There is a stench above the tile
		Satisfy<ConstantSum<int, -1>, 'Y', 'S'>, // Y is S - 1
		Satisfy<ConstantSum<int, 1>, 'A', 'S'>, // A is S + 1
		Satisfy<ConstantSum<int, -1>, 'L', 'X'>, // L is X - 1
		Satisfy<ConstantSum<int, 1>, 'R', 'X'>, // R is X + 1
		Satisfy<RNotWumpus, 'X', 'A'>, // There is not a wumpus above the stench
		Satisfy<RNotWumpus, 'L', 'S'>, // There is not a wumpus to the left of the stench
		Satisfy<RNotWumpus, 'R', 'S'>>(); // There is not a wumpus to the right of the stench
}

void add_safe_tile_rules(WumpusDB& database)
{
	using namespace brolog;

	// A tile is safe if it does not contain a wumpus or a pit
	database.insert_rule<RSafe, Params<'X', 'Y'>,
		Satisfy<RNotPit, 'X', 'Y'>,
		Satisfy<RNotWumpus, 'X', 'Y'>>();
}

void add_safe_unvisited_tile_rules(WumpusDB& database)
{
	using namespace brolog;

	// A tile is safe and univisited if it is safe and unvisisted
	database.insert_rule<RSafe, Params<'X', 'Y'>,
		Satisfy<RSafe, 'X', 'Y'>,
		NotSatisfy<RVisited, 'X', 'Y'>>();
}

int main()
{
	using brolog::Unknown;
	auto database = WumpusDB{};
	add_visited_rules(database);
	add_no_breeze_rules(database);
	add_no_stench_rules(database);
	add_not_pit_rules(database);
	add_not_wumpus_rules(database);
	add_wumpus_rules(database);

	database.insert_fact<FObstacle>(-1, 0);
	database.insert_fact<FObstacle>(-1, 1);
	database.insert_fact<FObstacle>(0, -1);
	database.insert_fact<FObstacle>(1, -1);
	database.insert_fact<FEmpty>(0, 0);

	auto query = database.satisfy<RNotPit>(Unknown<'X'>(), Unknown<'Y'>());

	query([](auto x, auto y)
	{
		std::cout << "X = " << x << ", Y = " << y << std::endl;
	});

	std::cin.get();
}
