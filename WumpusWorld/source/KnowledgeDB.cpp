// KnowledgeDB.cpp

#include <Brolog/Brolog.h>
#include <Brolog/Predicates/Math.h>
#include "../include/KnowledgeDB.h"

/////////////////
///   Facts   ///

/* The X and Y coordinates of a tile that has been visited. */
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

/* The X and Y coordinates of all tiles that were visited and are safe to revisit. */
using RSafeVisited = brolog::RuleType<struct CSafeVisited, int, int>;

/* The X and Y coordinates of all tiles that are reachable. */
using RReachable = brolog::RuleType<struct CReachable, int, int>;

/* The X and Y coordinates of all tiles that are reachable and unexplored. */
using RReachableUnexplored = brolog::RuleType<struct CReachableUnexplored, int, int>;

/* The X and Y coordinages of all tiles in which there definately is nothing that will kill you, are reachable, and have not been visited yet. */
using RSafeReachableUnexplored = brolog::RuleType<struct CSafeReachableUnexplored, int, int>;

/* The X and Y coordinates of all tiles that there can't be prove are not safe, and have not been visited yet. */
using RMaybeSafeReachableUnexplored = brolog::RuleType<struct CMaybeSafeReachableUnexplored, int, int>;

/* THe X and Y coordinates of tile you can shoot a wumpus tile from, given as
 * Wx, Wy, Sx, Sy */
using RShootWumpus = brolog::RuleType<struct CShoot, int, int, int, int>;

// Database type (all supported facts and rules)
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
	RNoBreeze,
	RNoStench,
	RNotPit,
	RPit,
	RNotWumpus,
	RWumpus,
	RSafe,
	RSafeVisited,
	RReachable,
	RReachableUnexplored,
	RSafeReachableUnexplored,
	RMaybeSafeReachableUnexplored,
	RShootWumpus>;

/* Adds all instances of the 'RNeighbor' rule to the database. */
void add_neighbor_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y,
		NEIGHBOR_X,
		NEIGHBOR_Y
	};

	// Handles case for neighbor to the left
	database.insert_rule<RNeighbor, Params<X, Y, NEIGHBOR_X, Y>,
		Satisfy<ConstantSum<int, -1>, NEIGHBOR_X, X>>(); // NEIGHBOR_X is X - 1

	// Handles case for neighbor to the right
	database.insert_rule<RNeighbor, Params<X, Y, NEIGHBOR_X, Y>,
		Satisfy<ConstantSum<int, 1>, NEIGHBOR_X, X>>(); // NEIGHBOR_X is X + 1

	// Handles case for neighbor above
	database.insert_rule<RNeighbor, Params<X, Y, X, NEIGHBOR_Y>,
		Satisfy<ConstantSum<int, 1>, NEIGHBOR_Y, Y>>(); // NEIGHBOR_Y is Y + 1

	// Handles case for neighbor below
	database.insert_rule<RNeighbor, Params<X, Y, X, NEIGHBOR_Y>,
		Satisfy<ConstantSum<int, -1>, NEIGHBOR_Y, Y>>(); // NEIGHBOR_Y is Y - 1
}

/* Adds all instances of the 'RNoBreeze' rule to the database. */
void add_no_breeze_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y,
	};

	// There is no breeze in a tile if we've visited it, and we haven't recorded a breeze or an obstacle
	database.insert_rule<RNoBreeze, Params<X, Y>,
		Satisfy<FVisited, X, Y>,
		NotSatisfy<FObstacle, X, Y>,
		NotSatisfy<FBreeze, X, Y>>();
}

/* Adds all instances of the 'RNoStench' rule to the database. */
void add_no_stench_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y
	};

	// There is no stench on a tile if we have visited it and we have not recorded a stench or an obstacle
	database.insert_rule<RNoStench, Params<X, Y>,
		Satisfy<FVisited, X, Y>,
		NotSatisfy<FObstacle, X, Y>,
		NotSatisfy<FStench, X, Y>>();
}

/* Adds all instances of the RNotPit' rule to the database. */
void add_not_pit_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y,
		NEIGHBOR_X,
		NEIGHBOR_Y
	};

	// There is not a pit on a tile if the tile contains a dead wumpus
	database.insert_rule<RNotPit, Params<X, Y>,
		Satisfy<FDeadWumpus, X, Y>>();

	// There is not a pit on a tile if we've visited it and did not die from a pit
	database.insert_rule<RNotPit, Params<X, Y>,
		Satisfy<FVisited, X, Y>,
		NotSatisfy<FPitDeath, X, Y>>();

	// There is not a pit on a tile if there is not a breeze on any of its neighbors
	database.insert_rule<RNotPit, Params<X, Y>,
		Satisfy<RNoBreeze, NEIGHBOR_X, NEIGHBOR_Y>,
		Satisfy<RNeighbor, X, Y, NEIGHBOR_X, NEIGHBOR_Y>>();
}

/* Adds all instances of the 'RPit' rule to the database. */
void add_pit_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		PIT_X,
		PIT_Y,
		BREEZE_X,
		BREEZE_Y,
		LEFT_OF_BREEZE_X,
		RIGHT_OF_BREEZE_X,
		ABOVE_BREEZE_Y,
		BELOW_BREEZE_Y
	};

	// There is a pit on the tile if we went there and died from a pit
	database.insert_rule<RPit, Params<PIT_X, PIT_Y>,
		Satisfy<FPitDeath, PIT_X, PIT_Y>>();

	database.insert_rule<RPit, Params<PIT_X, BREEZE_Y>,
		Satisfy<FBreeze, BREEZE_X, BREEZE_Y>, // There is a breeze on the same row as the tile
		NotSatisfy<FPitDeath, BREEZE_X, BREEZE_Y>, // That does not originate from a time we died in a pit
		Satisfy<ConstantSum<int, 1>, PIT_X, BREEZE_X>, // PIT_X is BREEZE_X + 1 (the breeze is to the left of the tile)
		Satisfy<ConstantSum<int, -1>, LEFT_OF_BREEZE_X, BREEZE_X>, // LEFT_OF_BREEZE_X is BREEZE_X - 1
		Satisfy<ConstantSum<int, -1>, BELOW_BREEZE_Y, BREEZE_Y>, // BELOW_BREEZE_Y is BREEZE_Y - 1
		Satisfy<ConstantSum<int, 1>, ABOVE_BREEZE_Y, BREEZE_Y>, // ABOVE_BREEZE_Y is BREEZE_Y + 1
		Satisfy<RNotPit, BREEZE_X, ABOVE_BREEZE_Y>, // There is not a pit above the breeze
		Satisfy<RNotPit, BREEZE_X, BELOW_BREEZE_Y>, // There is not a pit below the breeze
		Satisfy<RNotPit, LEFT_OF_BREEZE_X, BREEZE_Y>>(); // There is not a pit to the left of the breeze

	database.insert_rule<RPit, Params<PIT_X, BREEZE_Y>,
		Satisfy<FBreeze, BREEZE_X, BREEZE_Y>, // There is a breeze on the same row as the tile
		NotSatisfy<FPitDeath, BREEZE_X, BREEZE_Y>, // That does not originate from a time we died in a pit
		Satisfy<ConstantSum<int, -1>, PIT_X, BREEZE_X>, // PIT_X is BREEZE_X - 1 (the breeze is to the right of the tile)
		Satisfy<ConstantSum<int, 1>, RIGHT_OF_BREEZE_X, BREEZE_X>, // RIGHT_OF_BREEZE_X is BREEZE_X + 1
		Satisfy<ConstantSum<int, -1>, BELOW_BREEZE_Y, BREEZE_Y>, // BELOW_BREEZE_Y is PIT_Y - 1
		Satisfy<ConstantSum<int, 1>, ABOVE_BREEZE_Y, BREEZE_Y>, // ABOVE_BREEZE_Y is PIT_Y + 1
		Satisfy<RNotPit, BREEZE_X, ABOVE_BREEZE_Y>, // There is not a pit above the breeze
		Satisfy<RNotPit, BREEZE_X, BELOW_BREEZE_Y>, // There is not a pit below the breeze
		Satisfy<RNotPit, RIGHT_OF_BREEZE_X, BREEZE_Y>>(); // There is not a pit to the right of the breeze

	database.insert_rule<RPit, Params<BREEZE_X, PIT_Y>,
		Satisfy<FBreeze, BREEZE_X, BREEZE_Y>, // There is a breeze in the same column as the tile
		NotSatisfy<FPitDeath, BREEZE_X, BREEZE_Y>, // That does not originate from a time we died in a pit
		Satisfy<ConstantSum<int, 1>, PIT_Y, BREEZE_Y>, // PIT_Y is BREEZE_Y + 1 (the breeze is below the tile)
		Satisfy<ConstantSum<int, -1>, BELOW_BREEZE_Y, BREEZE_Y>, // BELOW_BREEZE_Y is BREEZE_Y - 1
		Satisfy<ConstantSum<int, -1>, LEFT_OF_BREEZE_X, BREEZE_X>, // LEFT_OF_BREEZE_X is BREEZE_X - 1
		Satisfy<ConstantSum<int, 1>, RIGHT_OF_BREEZE_X, BREEZE_X>, // RIGHT_OF_BREEZE_X is BREEZE_X + 1
		Satisfy<RNotPit, BREEZE_X, BELOW_BREEZE_Y>, // There is not a pit below the breeze
		Satisfy<RNotPit, LEFT_OF_BREEZE_X, BREEZE_Y>, // There is not a pit to the left of the breeze
		Satisfy<RNotPit, RIGHT_OF_BREEZE_X, BREEZE_Y>>(); // There is not a pit to the right of the breeze

	database.insert_rule<RPit, Params<BREEZE_X, PIT_Y>,
		Satisfy<FBreeze, BREEZE_X, BREEZE_Y>, // There is a breeze in the same column as the tile
		NotSatisfy<FPitDeath, BREEZE_X, BREEZE_Y>, // That does not originate from a time we died in a pit
		Satisfy<ConstantSum<int, -1>, PIT_Y, BREEZE_Y>, // PIT_Y is BREEZE_Y - 1 (the breeze is above the tile)
		Satisfy<ConstantSum<int, 1>, ABOVE_BREEZE_Y, BREEZE_Y>, // ABOVE_BREEZE_Y is BREEZE_Y + 1
		Satisfy<ConstantSum<int, -1>, LEFT_OF_BREEZE_X, BREEZE_X>, // LEFT_OF_BREEZE_X is BREEZE_X - 1
		Satisfy<ConstantSum<int, 1>, RIGHT_OF_BREEZE_X, BREEZE_X>, // RIGHT_OF_BREEZE_X is BREEZE_X + 1
		Satisfy<RNotPit, BREEZE_X, ABOVE_BREEZE_Y>, // There is not a pit above the breeze
		Satisfy<RNotPit, LEFT_OF_BREEZE_X, BREEZE_Y>, // There is not a pit to the left of the breeze
		Satisfy<RNotPit, RIGHT_OF_BREEZE_X, BREEZE_Y>>(); // There is not a pit to the right of the breeze
}

/* Adds all instances of the 'RNotWumpus' rule to the database. */
void add_not_wumpus_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y,
		NEIGHBOR_X,
		NEIGHBOR_Y
	};

	// There is not a wumpus on the tile if the tile contains a dead wumpus
	database.insert_rule<RNotWumpus, Params<X, Y>,
		Satisfy<FDeadWumpus, X, Y>>();

	// There is not a wumpus on the tile if we have been there and did not die from a wumpus
	database.insert_rule<RNotWumpus, Params<X, Y>,
		Satisfy<FVisited, X, Y>,
		NotSatisfy<FWumpusDeath, X, Y>>();

	// There is not a wumpus on the tile if there is a pit on the tile
	database.insert_rule<RNotWumpus, Params<X, Y>,
		Satisfy<RPit, X, Y>>();

	// There is not a wumpus on the tile if there is not a stench on one of its neighbors
	database.insert_rule<RNotWumpus, Params<X, Y>,
		Satisfy<RNoStench, NEIGHBOR_X, NEIGHBOR_Y>,
		Satisfy<RNeighbor, X, Y, NEIGHBOR_X, NEIGHBOR_Y>>();
}

/* Adds all instances of the 'RWumpus' rule to the database. */
void add_wumpus_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		WUMPUS_X,
		WUMPUS_Y,
		STENCH_X,
		STENCH_Y,
		LEFT_OF_STENCH_X,
		RIGHT_OF_STENCH_X,
		ABOVE_STENCH_Y,
		BELOW_STENCH_Y,
	};

	// A tile contains a wumpus if we've died from a wumpus and have not killed it
	database.insert_rule<RWumpus, Params<WUMPUS_X, WUMPUS_Y>,
		Satisfy<FWumpusDeath, WUMPUS_X, WUMPUS_Y>,
		NotSatisfy<FDeadWumpus, WUMPUS_X, WUMPUS_Y>>();

	database.insert_rule<RWumpus, Params<WUMPUS_X, STENCH_Y>,
		Satisfy<FStench, STENCH_X, STENCH_Y>, // There is a stench in the same row as the tile
		Satisfy<RNotWumpus, STENCH_X, STENCH_Y>, // That does NOT contain a wumpus
		Satisfy<ConstantSum<int, 1>, WUMPUS_X, STENCH_X>, // WUMPUS_X is STENCH_X + 1 (the stench is to the left of the tile)
		Satisfy<ConstantSum<int, -1>, LEFT_OF_STENCH_X, STENCH_X>, // LEFT_OF_STENCH_X is STENCH_X - 1
		Satisfy<ConstantSum<int, -1>, BELOW_STENCH_Y, STENCH_Y>, // BELOW_STENCH_Y is STENCH_Y - 1
		Satisfy<ConstantSum<int, 1>, ABOVE_STENCH_Y, STENCH_Y>, // ABOVE_STENCH_Y is STENCH_Y + 1
		Satisfy<RNotWumpus, STENCH_X, ABOVE_STENCH_Y>, // There is not a wumpus above the stench
		Satisfy<RNotWumpus, STENCH_X, BELOW_STENCH_Y>, // There is not a wumpus below the stench
		Satisfy<RNotWumpus, LEFT_OF_STENCH_X, STENCH_Y>>(); // There is not a wumpus to the left of the stench

	database.insert_rule<RWumpus, Params<WUMPUS_X, STENCH_Y>,
		Satisfy<FStench, STENCH_X, STENCH_Y>, // There is a stench in the same row as the tile
		Satisfy<RNotWumpus, STENCH_X, STENCH_Y>, // That does NOT contain a wumpus
		Satisfy<ConstantSum<int, -1>, WUMPUS_X, STENCH_X>, // WUMPUS_X is STENCH_X - 1 (the stench is to the right of the tile)
		Satisfy<ConstantSum<int, 1>, RIGHT_OF_STENCH_X, STENCH_X>, // RIGHT_OF_STENCH_X is STENCH_X + 1
		Satisfy<ConstantSum<int, -1>, BELOW_STENCH_Y, STENCH_Y>, // BELOW_STENCH_Y is STENCH_Y - 1
		Satisfy<ConstantSum<int, 1>, ABOVE_STENCH_Y, STENCH_Y>, // ABOVE_STENCH_Y is STENCH_Y + 1
		Satisfy<RNotWumpus, STENCH_X, ABOVE_STENCH_Y>, // There is not a wumpus above the stench
		Satisfy<RNotWumpus, STENCH_X, BELOW_STENCH_Y>, // There is not a wumpus below the stench
		Satisfy<RNotWumpus, RIGHT_OF_STENCH_X, STENCH_Y>>(); // There is not a wumpus to the right of the stench

	database.insert_rule<RWumpus, Params<STENCH_X, WUMPUS_Y>,
		Satisfy<FStench, STENCH_X, STENCH_Y>, // There is a stench in the same column as the tile
		Satisfy<RNotWumpus, STENCH_X, STENCH_Y>, // That does NOT contain a wumpus
		Satisfy<ConstantSum<int, 1>, WUMPUS_Y, STENCH_Y>, // WUMPUS_Y is STENCH_Y + 1 (the stench is below the tile)
		Satisfy<ConstantSum<int, -1>, BELOW_STENCH_Y, STENCH_Y>, // BELOW_STENCH_Y is STENCH_Y - 1
		Satisfy<ConstantSum<int, -1>, LEFT_OF_STENCH_X, STENCH_X>, // LEFT_OF_STENCH_X is STENCH_X - 1
		Satisfy<ConstantSum<int, 1>, RIGHT_OF_STENCH_X, STENCH_X>, // RIGHT_OF_STENCH_X is STENCH_X + 1
		Satisfy<RNotWumpus, STENCH_X, BELOW_STENCH_Y>, // There is not a wumpus below the stench
		Satisfy<RNotWumpus, LEFT_OF_STENCH_X, STENCH_Y>, // There is not a wumpus to the left of the stench
		Satisfy<RNotWumpus, RIGHT_OF_STENCH_X, STENCH_Y>>(); // There is not a wumpus to the right of the stench

	database.insert_rule<RWumpus, Params<STENCH_X, WUMPUS_Y>,
		Satisfy<FStench, STENCH_X, STENCH_Y>, // There is a stench in the same column as the tile
		Satisfy<RNotWumpus, STENCH_X, STENCH_Y>, // That does NOT contain a wumpus
		Satisfy<ConstantSum<int, -1>, WUMPUS_Y, STENCH_Y>, // WUMPUS_Y is STENCH_Y - 1 (the stench is above the tile)
		Satisfy<ConstantSum<int, 1>, ABOVE_STENCH_Y, STENCH_Y>, // ABOVE_STENCH_Y is STENCH_Y + 1
		Satisfy<ConstantSum<int, -1>, LEFT_OF_STENCH_X, STENCH_X>, // LEFT_OF_STENCH_X is STENCH_X - 1
		Satisfy<ConstantSum<int, 1>, RIGHT_OF_STENCH_X, STENCH_X>, // RIGHT_OF_STENCH_X is STENCH_X + 1
		Satisfy<RNotWumpus, STENCH_X, ABOVE_STENCH_Y>, // There is not a wumpus above the stench
		Satisfy<RNotWumpus, LEFT_OF_STENCH_X, STENCH_Y>, // There is not a wumpus to the left of the stench
		Satisfy<RNotWumpus, RIGHT_OF_STENCH_X, STENCH_Y>>(); // There is not a wumpus to the right of the stench
}

/* Adds all instances of the 'RSafe' rule to the database. */
void add_safe_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y
	};

	// A tile is safe if it does not contain a wumpus or a pit
	database.insert_rule<RSafe, Params<X, Y>,
		Satisfy<RNotPit, X, Y>,
		Satisfy<RNotWumpus, X, Y>>();
}

/* Adds all instances of the 'RSafeVisited' rule to the database. */
void add_safe_visited_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y
	};

	// A tile is safe and visited if it has been visited, and we were not killed by a pit or a wumpus
	database.insert_rule<RSafeVisited, Params<X, Y>,
		Satisfy<FVisited, X, Y>,
		NotSatisfy<FPitDeath, X, Y>,
		NotSatisfy<FWumpusDeath, X, Y>>();

	// A tile is safe and visited if it has been visited, we were not killed by a pit, and there is a dead wumpus on it
	database.insert_rule<RSafeVisited, Params<X, Y>,
		Satisfy<FVisited, X, Y>,
		NotSatisfy<FPitDeath, X, Y>,
		Satisfy<FDeadWumpus, X, Y>>();
}

/* Adds all instances of the 'RReachable' rule to the database. */
void add_reachable_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y,
		NEIGHBOR_X,
		NEIGHBOR_Y
	};

	// A tile is reachable if it neighbors a safe visited tile that is not an obstacle, and it itself is not proven to be an obstacle
	database.insert_rule<RReachable, Params<X, Y>,
		Satisfy<RSafeVisited, NEIGHBOR_X, NEIGHBOR_Y>,
		NotSatisfy<FObstacle, NEIGHBOR_X, NEIGHBOR_Y>,
		Satisfy<RNeighbor, X, Y, NEIGHBOR_X, NEIGHBOR_Y>,
		NotSatisfy<FObstacle, X, Y>>();
}

/* Adds all instances of the 'RReachableUnexplored' rules to the database. */
void add_reachable_unexplored_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y
	};

	// A tile is reachable and unexplored if it is reachable and not visited
	database.insert_rule<RReachableUnexplored, Params<X, Y>,
		Satisfy<RReachable, X, Y>,
		NotSatisfy<FVisited, X, Y>>();
}

/* Adds all instances of the 'RSafeReachableUnexplored' rules to the database. */
void add_safe_reachable_unexplored_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y
	};

	// A tile is safe and reachable and unexplored if it is reachable, unexplored, and proven to be safe
	database.insert_rule<RSafeReachableUnexplored, Params<X, Y>,
		Satisfy<RReachableUnexplored, X, Y>,
		Satisfy<RSafe, X, Y>>();
}

/* Adds all instances of the 'RMaybeSafeReachableUnexplored' rules to the database. */
void add_maybe_safe_reachable_unexplored_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y
	};

	// A tile is maybe safe, reachable, and unexplored if it is reachable and unexplored, and not proven to be a pit or a wumpus
	database.insert_rule<RMaybeSafeReachableUnexplored, Params<X, Y>,
		Satisfy<RReachableUnexplored, X, Y>,
		NotSatisfy<RPit, X, Y>,
		NotSatisfy<RWumpus, X, Y>>();
}

void add_shoot_wumpus_rules(WumpusWorldDB& database)
{
	using namespace brolog;

	// Variable names
	enum
	{
		X,
		Y,
		SAFE_NEIGHBOR_X,
		SAFE_NEIGHBOR_Y
	};

	// You can shoot a wumpus from a safe neighboring tile that is not an obstacle (keep it simple)
	database.insert_rule<RShootWumpus, Params<X, Y, SAFE_NEIGHBOR_X, SAFE_NEIGHBOR_Y>,
		Satisfy<RWumpus, X, Y>,
		Satisfy<RNeighbor, X, Y, SAFE_NEIGHBOR_X, SAFE_NEIGHBOR_Y>,
		Satisfy<RSafeVisited, SAFE_NEIGHBOR_X, SAFE_NEIGHBOR_Y>,
		NotSatisfy<FObstacle, SAFE_NEIGHBOR_X, SAFE_NEIGHBOR_Y>>();
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
	add_no_breeze_rules(_data->database);
	add_no_stench_rules(_data->database);
	add_not_pit_rules(_data->database);
	add_pit_rules(_data->database);
	add_not_wumpus_rules(_data->database);
	add_wumpus_rules(_data->database);
	add_safe_rules(_data->database);
	add_safe_visited_rules(_data->database);
	add_reachable_rules(_data->database);
	add_reachable_unexplored_rules(_data->database);
	add_safe_reachable_unexplored_rules(_data->database);
	add_maybe_safe_reachable_unexplored_rules(_data->database);
	add_shoot_wumpus_rules(_data->database);

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

Action KnowledgeDB::next_action(const Player& player) const
{
	Action result;

	// Determine if we've found the gold
	if (known_gold(player.location))
	{
		result.type = Action::Type::GRAB;
		return result;
	}

	// Determine if we can shoot a wumpus (the agent could be a pacifist, but wumpus queries are slow)
	Coordinate wumpusCoords;
	if (player.num_arrows > 0 && next_wumpus(wumpusCoords))
	{
		result.type = Action::Type::SHOOT;

		// Figure out where to shoot the wumpus from
		using namespace brolog;
		auto query = _data->database.create_query<RShootWumpus>(wumpusCoords.x, wumpusCoords.y, Unknown<'X'>(), Unknown<'Y'>());
		query([&](auto x, auto y) {
			result.location.x = x;
			result.location.y = y;
		});

		// Figure out which direction to shoot the wumpus in
		if (wumpusCoords.y > result.location.y)
		{
			result.direction = Direction::NORTH;
		}
		else if (wumpusCoords.y < result.location.y)
		{
			result.direction = Direction::SOUTH;
		}
		else if (wumpusCoords.x < result.location.x)
		{
			result.direction = Direction::EAST;
		}
		else if (wumpusCoords.x > result.location.x)
		{
			result.direction = Direction::WEST;
		}
		else
		{
			// Couldn't figure out direction to shoot in, something went wrong
			assert(false);
		}

		return result;
	}

	// Determine if there's anywhere we can move
	if (next_safe_unexplored(result.location))
	{
		result.type = Action::Type::MOVE;
		return result;
	}

	// No safe places we can visit, no wumpus' we can shoot, take a risk
	if (next_maybe_safe_unexplored(result.location))
	{
		result.type = Action::Type::MOVE;
		return result;
	}

	// Nowhere to go, no gold found, just give up
	result.type = Action::Type::STOP;
	return result;
}

void KnowledgeDB::visited(Coordinate coord, TilePercepts_t percepts)
{
	// Add the visited fact
	_data->database.insert_fact<FVisited>(coord.x, coord.y);

	// Add the stench breeze fact, if observed
	if (percepts & TilePercepts::BREEZE)
	{
		_data->database.insert_fact<FBreeze>(coord.x, coord.y);
	}

	// Add the stench fact, if observed
	if (percepts & TilePercepts::STENCH)
	{
		_data->database.insert_fact<FStench>(coord.x, coord.y);
	}

	// Add the glimmer fact, if observed
	if (percepts & TilePercepts::GLIMMER)
	{
		_data->database.insert_fact<FGlimmer>(coord.x, coord.y);
	}

	// Add the obstacle fact, if observed
	if (percepts & TilePercepts::BUMP)
	{
		_data->database.insert_fact<FObstacle>(coord.x, coord.y);
	}

	// Add the pit death fact, if observed
	if (percepts & TilePercepts::PIT_DEATH)
	{
		_data->database.insert_fact<FPitDeath>(coord.x, coord.y);
	}

	// Add the wumpus death fact, if observed
	if (percepts & TilePercepts::WUMPUS_DEATH)
	{
		_data->database.insert_fact<FWumpusDeath>(coord.x, coord.y);
	}
}

void KnowledgeDB::dead_wumpus(bool invalidatedSelfStench, Direction_t invalidatedNeighborStenches)
{
	// Get the coordinates at which we thought there was a wumpus
	Coordinate coord;
	next_wumpus(coord);

	// Insert the dead wumpus fact
	_data->database.insert_fact<FDeadWumpus>(coord.x, coord.y);

	// Remove all invalidated stench facts from the database
	if (invalidatedSelfStench)
	{
		_data->database.remove_fact<FStench>(coord.x, coord.y);
	}

	// If there is no longer a stench to the north
	if (invalidatedNeighborStenches & Direction::NORTH)
	{
		auto north = coord.north();
		_data->database.remove_fact<FStench>(north.x, north.y);
	}

	// If there is no longer a stench to the south
	if (invalidatedNeighborStenches & Direction::SOUTH)
	{
		auto south = coord.south();
		_data->database.remove_fact<FStench>(south.x, south.y);
	}

	// If there is no longer a stench to the east
	if (invalidatedNeighborStenches & Direction::EAST)
	{
		auto east = coord.east();
		_data->database.remove_fact<FStench>(east.x, east.y);
	}

	// If there is no longer a stench to the west
	if (invalidatedNeighborStenches & Direction::WEST)
	{
		auto west = coord.west();
		_data->database.remove_fact<FStench>(west.x, west.y);
	}
}

bool KnowledgeDB::next_wumpus(Coordinate& coords) const
{
	auto query = _data->database.create_query<RWumpus>(brolog::Unknown<'X'>(), brolog::Unknown<'Y'>());

	return query([&](int x, int y) {
		coords.x = x;
		coords.y = y;
	}) != 0;
}

bool KnowledgeDB::next_safe_unexplored(Coordinate& coords) const
{
	auto query = _data->database.create_query<RSafeReachableUnexplored>(brolog::Unknown<'X'>(), brolog::Unknown<'Y'>());

	return query([&](int x, int y) {
		coords.x = x;
		coords.y = y;
	}) != 0;
}

bool KnowledgeDB::next_maybe_safe_unexplored(Coordinate& coords) const
{
	// Attempt to satisfy the 'MaysafeReachableUnexplored' rule where both arguments are unknown.
	auto query = _data->database.create_query<RMaybeSafeReachableUnexplored>(brolog::Unknown<'X'>(), brolog::Unknown<'Y'>());

	return query([&](int x, int y) {
		coords.x = x;
		coords.y = y;
	}) != 0;
}

bool KnowledgeDB::known_visited(Coordinate coords) const
{
	return _data->database.create_query<FVisited>(coords.x, coords.y)([]() {}) != 0;
}

bool KnowledgeDB::known_stench(Coordinate coords) const
{
	return _data->database.create_query<FStench>(coords.x, coords.y)([]() {}) != 0;
}

bool KnowledgeDB::known_breeze(Coordinate coords) const
{
	return _data->database.create_query<FBreeze>(coords.x, coords.y)([]() {}) != 0;
}

bool KnowledgeDB::known_obstacle(Coordinate coords) const
{
	return _data->database.create_query<FObstacle>(coords.x, coords.y)([]() {}) != 0;
}

bool KnowledgeDB::known_pit(Coordinate coords) const
{
	return _data->database.create_query<RPit>(coords.x, coords.y)([]() {}) != 0;
}

bool KnowledgeDB::known_wumpus(Coordinate coords) const
{
	return _data->database.create_query<RWumpus>(coords.x, coords.y)([]() {}) != 0;
}

bool KnowledgeDB::known_dead_wumpus(Coordinate coords) const
{
	return _data->database.create_query<FDeadWumpus>(coords.x, coords.y)([]() {}) != 0;
}

bool KnowledgeDB::known_gold(Coordinate coord) const
{
	return _data->database.create_query<FGlimmer>(coord.x, coord.y)([]() {}) != 0;
}
