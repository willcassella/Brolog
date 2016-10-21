// KnowledgeDB.cpp

#include <Brolog/Brolog.h>
#include <Brolog/Predicates/Math.h>
#include "../include/KnowledgeDB.h"

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

/* The X and Y coordinates of all tiles that were visited and are safe to revisit. */
using RSafeVisited = brolog::RuleType<struct CSafeVisited, int, int>;

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
	RSafeVisited,
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

void add_safe_visited_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	database.insert_rule<RSafeVisited, Params<'X', 'Y'>,
		Satisfy<FVisited, 'X', 'Y'>,
		NotSatisfy<FPitDeath, 'X', 'Y'>,
		NotSatisfy<FWumpusDeath, 'X', 'Y'>>();

	database.insert_rule<RSafeVisited, Params<'X', 'Y'>,
		Satisfy<FVisited, 'X', 'Y'>,
		NotSatisfy<FPitDeath, 'X', 'Y'>,
		Satisfy<FDeadWumpus, 'X', 'Y'>>();
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

	// There is not a pit on a tile if there is not a breeze on any of its neighbors
	database.insert_rule<RNotPit, Params<'X', 'Y'>,
		Satisfy<RNoBreeze, 'A', 'B'>,
		Satisfy<RNeighbor, 'X', 'Y', 'A', 'B'>>();
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

	// There is not a wumpus on the tile if there is not a stench on one of its neighbors
	database.insert_rule<RNotWumpus, Params<'X', 'Y'>,
		Satisfy<RNoStench, 'A', 'B'>,
		Satisfy<RNeighbor, 'X', 'Y', 'A', 'B'>>();
}

void add_wumpus_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// A tile contains a wumpus if we've died from a wumpus and not killed it
	database.insert_rule<RWumpus, Params<'X', 'Y'>,
		Satisfy<FWumpusDeath, 'X', 'Y'>,
		NotSatisfy<FDeadWumpus, 'X', 'Y'>>();

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

	// A tile is reachable if it neighbors a safe visited tile that is not an obstacle, and it itself is not an obstacle
	database.insert_rule<RReachable, Params<'X', 'Y'>,
		Satisfy<RSafeVisited, 'A', 'B'>,
		NotSatisfy<FObstacle, 'A', 'B'>,
		Satisfy<RNeighbor, 'X', 'Y', 'A', 'B'>,
		NotSatisfy<FObstacle, 'X', 'Y'>>();
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

	// A tile is reachable and maybe safe if it's reachable, not visited, and you haven't deduced that there is a pit or wumpus there
	database.insert_rule<RMaybeSafeReachableUnexplored, Params<'X', 'Y'>,
		Satisfy<RReachable, 'X', 'Y'>,
		NotSatisfy<FVisited, 'X', 'Y'>,
		NotSatisfy<RPit, 'X', 'Y'>,
		NotSatisfy<RWumpus, 'X', 'Y'>>();
}

//////////////////////////////
///   Knowledge Database   ///

struct KnowledgeDB::Data
{
	WumpusWorldDB database;
};

KnowledgeDB::KnowledgeDB(int size)
{
	// Add all rules to the database
	_data = std::make_unique<Data>();
	add_neighbor_rules(_data->database);
	add_safe_visited_rules(_data->database);
	add_no_breeze_rules(_data->database);
	add_no_stench_rules(_data->database);
	add_not_pit_rules(_data->database);
	add_pit_rules(_data->database);
	add_not_wumpus_rules(_data->database);
	add_wumpus_rules(_data->database);
	add_safe_rules(_data->database);
	add_reachable_rules(_data->database);
	add_safe_reachable_rules(_data->database);
	add_safe_reachable_unexplored_rules(_data->database);
	add_maybe_safe_reachable_unexplored_rules(_data->database);

	// Add walls to the database
	for (int i = 0; i < size; ++i)
	{
		_data->database.insert_fact<FObstacle>(i, -1);
		_data->database.insert_fact<FVisited>(i, -1);
		_data->database.insert_fact<FObstacle>(i, size);
		_data->database.insert_fact<FVisited>(i, size);

		_data->database.insert_fact<FObstacle>(-1, i);
		_data->database.insert_fact<FVisited>(-1, i);
		_data->database.insert_fact<FObstacle>(size, i);
		_data->database.insert_fact<FVisited>(size, i);
	}
}

KnowledgeDB::~KnowledgeDB()
{
}

void KnowledgeDB::visited(const Coordinate& coord, TileObsT observations)
{
	// Add the visited fact
	_data->database.insert_fact<FVisited>(coord.x, coord.y);

	// Add the stench breeze fact, if observed
	if ((observations & TileObs::BREEZE) != 0)
	{
		_data->database.insert_fact<FBreeze>(coord.x, coord.y);
	}

	// Add the stench fact, if observed
	if ((observations & TileObs::STENCH) != 0)
	{
		_data->database.insert_fact<FStench>(coord.x, coord.y);
	}

	// Add the obstacle fact, if observed
	if ((observations & TileObs::BUMP) != 0)
	{
		_data->database.insert_fact<FObstacle>(coord.x, coord.y);
	}

	// Add the pit death fact, if observed
	if ((observations & TileObs::PIT_DEATH) != 0)
	{
		_data->database.insert_fact<FPitDeath>(coord.x, coord.y);
	}

	// Add the wumpus death fact, if observed
	if ((observations & TileObs::WUMPUS_DEATH) != 0)
	{
		_data->database.insert_fact<FWumpusDeath>(coord.x, coord.y);
	}
}

void KnowledgeDB::dead_wumpus(const Coordinate& coord, const std::vector<Coordinate>& invalidatedStenches)
{
	// Insert the dead wumpus fact
	_data->database.insert_fact<FDeadWumpus>(coord.x, coord.y);

	// Remove all invalidated stench facts from the database
	for (auto stench : invalidatedStenches)
	{
		_data->database.remove_fact<FStench>(stench.x, stench.y);
	}
}

bool KnowledgeDB::next_wumpus(Coordinate& coords) const
{
	auto query = _data->database.satisfy<RWumpus>(brolog::Unknown<'X'>(), brolog::Unknown<'Y'>());

	bool found = false;
	query([&](int x, int y) {
		found = true;
		coords.x = x;
		coords.y = y;
	});

	return found;
}

bool KnowledgeDB::next_safe_unexplored(Coordinate& coords) const
{
	auto query = _data->database.satisfy<RSafeReachableUnexplored>(brolog::Unknown<'X'>(), brolog::Unknown<'Y'>());

	bool found = false;
	query([&](int x, int y) {
		found = true;
		coords.x = x;
		coords.y = y;
	});

	return found;
}

bool KnowledgeDB::next_maybe_safe_unexplored(Coordinate& coords) const
{
	// Attempt to satisfy the 'MaysafeReachableUnexplored' rule where both arguments are unknown.
	auto query = _data->database.satisfy<RMaybeSafeReachableUnexplored>(brolog::Unknown<'X'>(), brolog::Unknown<'Y'>());

	bool found = false;
	query([&](int x, int y) {
		found = true;
		coords.x = x;
		coords.y = y;
	});

	return found;
}
