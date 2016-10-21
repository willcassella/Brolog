// main.cpp

#include <iostream>
#include <Brolog/Brolog.h>
#include <Brolog/Predicates/Math.h>

/////////////////
///   Facts   ///

/* The X and Y coordinates of a that has been visited. */
using FVisited = brolog::FactType<struct CVisited, int, int>;

/* The X and Y coordinates of a tile in which a breeze was observed. */
using FBreeze = brolog::FactType<struct CBreeze, int, int>;

/* The X and Y coordinates of a tile in which a stench was observed. */
using FStench = brolog::FactType<struct CStench, int, int>;

/* The X and Y coordinates of a tile in which a glimmer was observed. */
using FGlimmer = brolog::FactType<struct CGlimmer, int, int>;

/* The X and Y coordinates of a tile in which an obstacle was observed. */
using FObstacle = brolog::FactType<struct CObstacle, int, int>;

/* The X and Y coordinates of a tile in which the agent died from falling into a pit. */
using FPitDeath = brolog::FactType<struct CPitDeath, int, int>;

/* The X and Y coordinates of a tile in which the agent died from (being eaten by?) a wumpus. */
using FWumpusDeath = brolog::FactType<struct CWumpusDeath, int, int>;

/* The X and Y coordinates of a tile that contains a dead wumpus. */
using FDeadWumpus = brolog::FactType<struct DeadWumpus, int, int>;

/////////////////
///   Rules   ///

/* Takes a pair of X and Y coordinates (X1, Y1, X2, Y2), and resolves if they are neighbors. */
using RNeighbor = brolog::RuleType<struct CNeighbor, int, int, int, int>;

/* The X and Y coordinates of all tiles that the agent has been too in which nothing was observed. */
using REmpty = brolog::RuleType<struct CEmpty, int, int>;

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

/* The X and Y coordinates of all tiles that are reachable. */
using RReachable = brolog::RuleType<struct CReachable, int, int>;

/* The X and Y coordinates of all tiles in which there definately is nothing that will kill you, and are reachable. */
using RSafeReachable = brolog::RuleType<struct CSafeReachable, int, int>;

/* The X and Y coordinages of all tiles in which there definately is nothing that will kill you, are reachable, and have not been visited yet. */
using RSafeReachableUnexplored = brolog::RuleType<struct CSafeReachableUnexplored, int, int>;

/* The X and Y coordinates of all tiles that there can't be prove are not safe, and have not been visited yet. */
using RMaybeSafeReachableUnexplored = brolog::RuleType<struct CMaybeSafeReachableUnexplored, int, int>;

// Database type
using WumpusWorldDB = brolog::DataBase<
	FVisited,
	FBreeze,
	FStench,
	FGlimmer,
	FObstacle,
	FPitDeath,
	FWumpusDeath,
	FDeadWumpus,

	RNeighbor,
	REmpty,
	RNoBreeze,
	RNoStench,
	RNotPit,
	RPit,
	RNotWumpus,
	RWumpus,
	RSafe,
	RReachable,
	RSafeReachable,
	RSafeReachableUnexplored,
	RMaybeSafeReachableUnexplored>;

void add_neighbor_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Handles case for neighbor to the left
	database.insert_rule<RNeighbor, Params<'X', 'Y', 'L', 'Y'>,
		Satisfy<ConstantSum<int, -1>, 'L', 'X'>>(); // L is X - 1

	database.insert_rule<RNeighbor, Params<'X', 'Y', 'R', 'Y'>,
		Satisfy<ConstantSum<int, 1>, 'R', 'X'>>(); // R is X + 1

	database.insert_rule<RNeighbor, Params<'X', 'Y', 'X', 'A'>,
		Satisfy<ConstantSum<int, 1>, 'A', 'Y'>>(); // A is Y + 1

	database.insert_rule<RNeighbor, Params<'X', 'Y', 'X', 'B'>,
		Satisfy<ConstantSum<int, -1>, 'B', 'Y'>>(); // B is Y - 1
}

void add_empty_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// A tile is empty if it has been visited and nothing was observed
	database.insert_rule<REmpty, Params<'X', 'Y'>,
		Satisfy<FVisited, 'X', 'Y'>,
		NotSatisfy<FBreeze, 'X', 'Y'>,
		NotSatisfy<FStench, 'X', 'Y'>,
		NotSatisfy<FGlimmer, 'X', 'Y'>,
		NotSatisfy<FObstacle, 'X', 'Y'>,
		NotSatisfy<FPitDeath, 'X', 'Y'>,
		NotSatisfy<FWumpusDeath, 'X', 'Y'>>();
}

void add_no_breeze_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// There is no breeze in a tile if we've visited it, and we haven't recorded a breeze or an obstacle
	database.insert_rule<RNoBreeze, Params<'X', 'Y'>,
		Satisfy<FVisited, 'X', 'Y'>,
		NotSatisfy<FObstacle, 'X', 'Y'>,
		NotSatisfy<FBreeze, 'X', 'Y'>>();
}

void add_no_stench_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// There is no stench on a tile if we have visited it and we have not recorded a stench or an obstacle
	database.insert_rule<RNoStench, Params<'X', 'Y'>,
		Satisfy<FVisited, 'X', 'Y'>,
		NotSatisfy<FObstacle, 'X', 'Y'>,
		NotSatisfy<FStench, 'X', 'Y'>>();
}

void add_not_pit_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// There is not a pit on a tile if the tile contains a dead wumpus
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<FDeadWumpus, 'X', 'Y'>>();

	// There is not a pit on a tile if we've visited it and did not die from a pit
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<FVisited, 'X', 'Y'>,
		NotSatisfy<FPitDeath, 'X', 'Y'>>();

	// There is not a pit on the tile if there is not a breeze on the left
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<RNoBreeze, 'L', 'Y'>,
		Satisfy<ConstantSum<int, 1>, 'X', 'L'>>(); // X is L + 1

	// There is not a pit on the tile if there is not a breeze to the right
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<RNoBreeze, 'R', 'Y'>,
		Satisfy<ConstantSum<int, -1>, 'X', 'R'>>(); // X is R - 1

	// There is not a pit on the tile if there is not a breeze below
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<RNoBreeze, 'X', 'B'>,
		Satisfy<ConstantSum<int, 1>, 'Y', 'B'>>(); // Y is B + 1

	// There is not a pit on the tile if there is not a breeze above
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<RNoBreeze, 'X', 'A'>,
		Satisfy<ConstantSum<int, -1>, 'Y', 'A'>>(); // Y is A - 1
}

void add_pit_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// There is a pit on the tile if we went there and died from a pit
	database.insert_rule<RPit, Params<'X', 'Y'>,
		Satisfy<FPitDeath, 'X', 'Y'>>();

	database.insert_rule<RPit, Params<'X', 'Y'>,
		Satisfy<FBreeze, 'E', 'Y'>, // There is a breeze (to the left of the tile)
		Satisfy<ConstantSum<int, 1>, 'X', 'E'>, // X is E + 1
		Satisfy<ConstantSum<int, -1>, 'L', 'E'>, // L is E - 1
		Satisfy<ConstantSum<int, -1>, 'B', 'Y'>, // B is Y - 1
		Satisfy<ConstantSum<int, 1>, 'A', 'Y'>, // A is Y + 1
		Satisfy<RNotPit, 'E', 'A'>, // There is not a pit above the breeze
		Satisfy<RNotPit, 'E', 'B'>, // There is not a pit below the breeze
		Satisfy<RNotPit, 'L', 'Y'>>(); // There is not a pit to the left of the breeze

	database.insert_rule<RPit, Params<'X', 'Y'>,
		Satisfy<FBreeze, 'E', 'Y'>, // There is a breeze (to the right of the tile)
		Satisfy<ConstantSum<int, -1>, 'X', 'E'>, // X is E - 1
		Satisfy<ConstantSum<int, 1>, 'E', 'R'>, // R is E + 1
		Satisfy<ConstantSum<int, -1>, 'B', 'Y'>, // B is Y - 1
		Satisfy<ConstantSum<int, 1>, 'A', 'Y'>, // A is Y + 1
		Satisfy<RNotPit, 'E', 'A'>, // There is not a pit above the breeze
		Satisfy<RNotPit, 'E', 'B'>, // There is not a pit below the breeze
		Satisfy<RNotPit, 'R', 'Y'>>(); // There is not a pit to the right of the breeze

	database.insert_rule<RPit, Params<'X', 'Y'>,
		Satisfy<FBreeze, 'X', 'E'>, // There is a breeze (below the tile)
		Satisfy<ConstantSum<int, 1>, 'Y', 'E'>, // Y is E + 1
		Satisfy<ConstantSum<int, -1>, 'B', 'E'>, // B is E - 1
		Satisfy<ConstantSum<int, -1>, 'L', 'X'>, // L is X - 1
		Satisfy<ConstantSum<int, 1>, 'R', 'X'>, // R is X + 1
		Satisfy<RNotPit, 'X', 'B'>, // There is not a pit below the breeze
		Satisfy<RNotPit, 'L', 'E'>, // There is not a pit to the left of the breeze
		Satisfy<RNotPit, 'R', 'E'>>(); // There is not a pit to the right of the breeze

	database.insert_rule<RPit, Params<'X', 'Y'>,
		Satisfy<FBreeze, 'X', 'E'>, // There is a breeze (above the tile)
		Satisfy<ConstantSum<int, -1>, 'Y', 'E'>, // Y is E - 1
		Satisfy<ConstantSum<int, 1>, 'A', 'E'>, // A is E + 1
		Satisfy<ConstantSum<int, -1>, 'L', 'X'>, // L is X - 1
		Satisfy<ConstantSum<int, 1>, 'R', 'X'>, // R is X + 1
		Satisfy<RNotPit, 'X', 'A'>, // There is not a pit above the breeze
		Satisfy<RNotPit, 'L', 'E'>, // There is not a pit to the left of the breeze
		Satisfy<RNotPit, 'R', 'E'>>(); // There is not a pit to the right of the breeze
}

void add_not_wumpus_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// There is not a wumpus on the tile if the tile contains a dead wumpus
	database.insert_rule<RNotWumpus, Params<'X', 'Y'>,
		Satisfy<FDeadWumpus, 'X', 'Y'>>();

	// There is not a wumpus on the tile if we have been there and did not die from a wumpus
	database.insert_rule<RNotWumpus, Params<'X', 'Y'>,
		Satisfy<FVisited, 'X', 'Y'>,
		NotSatisfy<FWumpusDeath, 'X', 'Y'>>();

	// There is not a wumpus on the tile if there is a pit on the tile
	database.insert_rule<RNotWumpus, Params<'X', 'Y'>,
		Satisfy<RPit, 'X', 'Y'>>();

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

void add_wumpus_rules(WumpusWorldDB& database)
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

void add_safe_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// A tile is safe if it does not contain a wumpus or a pit
	database.insert_rule<RSafe, Params<'X', 'Y'>,
		Satisfy<RNotPit, 'X', 'Y'>,
		Satisfy<RNotWumpus, 'X', 'Y'>>();
}

void add_reachable_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	database.insert_rule<RReachable, Params<'X', 'Y'>,
		Satisfy<FVisited, 'L', 'Y'>, // There is a visited tile to the left of the tile
		Satisfy<ConstantSum<int, 1>, 'X', 'L'>, // X is L + 1
		NotSatisfy<FObstacle, 'L', 'Y'>, // The visited tile is does not contain an obstacle
		Satisfy<RSafe, 'L', 'Y'>>(); // The visited tile is safe

	database.insert_rule<RReachable, Params<'X', 'Y'>,
		Satisfy<FVisited, 'R', 'Y'>, // There is a visited tile to the right of the tile
		Satisfy<ConstantSum<int, -1>, 'X', 'R'>, // X is R - 1
		NotSatisfy<FObstacle, 'R', 'Y'>, // The visited tile does not contain an obstacle
		Satisfy<RSafe, 'R', 'Y'>>(); // The visited tile is safe

	database.insert_rule<RReachable, Params<'X', 'Y'>,
		Satisfy<FVisited, 'X', 'B'>, // There is a visited tile below the tile
		Satisfy<ConstantSum<int, 1>, 'Y', 'B'>, // Y is B + 1
		NotSatisfy<FObstacle, 'X', 'B'>, // The visited tile does not contain an obstacle
		Satisfy<RSafe, 'X', 'B'>>(); // The visited tile is safe

	database.insert_rule<RReachable, Params<'X', 'Y'>,
		Satisfy<FVisited, 'X', 'A'>, // There is a visited tile above the tile
		Satisfy<ConstantSum<int, -1>, 'Y', 'A'>, // Y is A - 1
		NotSatisfy<FObstacle, 'X', 'A'>, // The visited tile does not contain an obstacle
		Satisfy<RSafe, 'X', 'A'>>(); // The visited tile is safe
}

void add_safe_reachable_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// A tile is safe and reachable if it is safe and reachable
	database.insert_rule<RSafeReachable, Params<'X', 'Y'>,
		Satisfy<RReachable, 'X', 'Y'>,
		Satisfy<RSafe, 'X', 'Y'>>();
}

void add_safe_reachable_unexplored_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// A tile is safe and reachable and unexplored if it is safe, reachable, and not visited
	database.insert_rule<RSafeReachableUnexplored, Params<'X', 'Y'>,
		Satisfy<RSafeReachable, 'X', 'Y'>,
		NotSatisfy<FVisited, 'X', 'Y'>>();
}

void add_maybe_safe_reachable_unexplored_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// A tile is reachable and maybe safe if it's reachable, unexplored, and you haven't deduced that there is a pit or wumpus there
	database.insert_rule<RMaybeSafeReachableUnexplored, Params<'X', 'Y'>,
		Satisfy<RReachable, 'X', 'Y'>,
		NotSatisfy<FVisited, 'X', 'Y'>,
		NotSatisfy<RPit, 'X', 'Y'>,
		NotSatisfy<RWumpus, 'X', 'Y'>>();
}

int main()
{
	// Instantiate the database
	using brolog::Unknown;
	auto database = WumpusWorldDB{};
	add_neighbor_rules(database);
	add_empty_rules(database);
	add_no_breeze_rules(database);
	add_no_stench_rules(database);
	add_not_pit_rules(database);
	add_pit_rules(database);
	add_not_wumpus_rules(database);
	add_wumpus_rules(database);
	add_safe_rules(database);
	add_reachable_rules(database);
	add_safe_reachable_rules(database);
	add_safe_reachable_unexplored_rules(database);
	add_maybe_safe_reachable_unexplored_rules(database);

	// Insert facts
	constexpr int MAP_WIDTH = 5;
	constexpr int MAP_HEIGHT = 4;

	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		database.insert_fact<FObstacle>(x, -1);
		database.insert_fact<FVisited>(x, -1);
		database.insert_fact<FObstacle>(x, MAP_HEIGHT);
		database.insert_fact<FVisited>(x, MAP_HEIGHT);
	}

	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		database.insert_fact<FObstacle>(-1, y);
		database.insert_fact<FVisited>(-1, y);
		database.insert_fact<FObstacle>(MAP_WIDTH, y);
		database.insert_fact<FVisited>(MAP_WIDTH, y);
	}

	database.insert_fact<FVisited>(0, 0);
	database.insert_fact<FVisited>(1, 0);
	database.insert_fact<FVisited>(2, 0);
	database.insert_fact<FBreeze>(2, 0);
	database.insert_fact<FVisited>(0, 1);
	database.insert_fact<FVisited>(1, 1);
	database.insert_fact<FBreeze>(1, 1);
	database.insert_fact<FDeadWumpus>(3, 0);
	database.insert_fact<FVisited>(3, 0);
	database.insert_fact<FBreeze>(3, 0);
	database.insert_fact<FVisited>(0, 2);
	database.insert_fact<FBreeze>(0, 2);

	auto query = database.satisfy<RNeighbor>(Unknown<'X'>(), Unknown<'Y'>(), 0, 0);

	std::cout << "Running..." << std::endl;

	query([](auto x, auto y)
	{
		std::cout << "X = " << x << ", Y = " << y << std::endl;
	});

	std::cout << "Done";
	std::cin.get();
}
