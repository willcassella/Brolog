// KnowledgeDB.h
#pragma once

#include <memory>
#include "World.h"

struct Action
{
	enum class Type
	{
		/* The player should move to a new location. The location coordinates are given in the 'location' field. */
		MOVE,

		/* The player should shoot in a direction, from a location. The location is given in the 'location' field.
		 * The location the wumpus is expected to be in is given in the 'location' field.
		 */
		SHOOT,

		/* The player should grab the gold from the tile they are currently in. */
		GRAB,

		/* No more reachable tiles are left to explore and we haven't found the gold, go home. */
		STOP
	};

	//////////////////
	///   Fields   ///
public:

	/* The type of action that should be performed. */
	Type type;

	/* The direction the player should shoot in, uesd by the 'SHOOT' action. */
	Direction_t direction;

	/* If the action type is 'MOVE', this is the coordinates the player should move to.
	 * If the action type is 'SHOOT', this is the expected location of the wumpus. */
	Coordinate location;
};

class KnowledgeDB
{
	struct Data;

	////////////////////////
	///   Constructors   ///
public:

	KnowledgeDB(int worldSize);
	~KnowledgeDB();

	///////////////////
	///   Methods   ///
public:

	/* Returns what the next action of the player should be, given their current location. */
	Action next_action(Coordinate location) const;

	/* Reports that a tile was visited, and what was observed on that tile. */
	void visited(Coordinate coord, TilePercepts_t percepts);

	/* Reports that a wumpus has been killed, and which stench tiles have been eliminated. */
	void dead_wumpus(bool invalidedSelfStench, Direction_t invalidNeighborStenches);

	/* Returns whether the given tile is known to be visited. Used for debugging. */
	bool known_visited(Coordinate coords) const;

	/* Returns whether the given tile is known to contian a stench. Used for debugging. */
	bool known_stench(Coordinate coords) const;

	/* Returns whether the given tile is known to contain a breeze. Used for debugging. */
	bool known_breeze(Coordinate coords) const;

	/* Returns whether the given tile is known to contain an obstacle. Used for debugging. */
	bool known_obstacle(Coordinate coords) const;

	/* Returns whether the given tile is known to contain a pit. Used for debugging. */
	bool known_pit(Coordinate coords) const;

	/* Returns whether the given tile is known to contain a wumpus. Used for debugging. */
	bool known_wumpus(Coordinate coords) const;

	/* Returns whether the given tile is known to contain a dead wumpus. Used for debugging. */
	bool known_dead_wumpus(Coordinate coords) const;

	/* Returns whether the given tile is known to contain gold. Used for debugging. */
	bool known_gold(Coordinate coord) const;

private:

	/* Attemps to deduce a known wumpus location, and returns whether one was found. */
	bool next_wumpus(Coordinate& coords) const;

	/* Attempts to find a tile that is known to be both safe and unexplored, and returns whether one was found. */
	bool next_safe_unexplored(Coordinate& coords) const;

	/* If the above query fails, you can attempt to find a tile that is unexplored, and not known to be unsafe. Returns whether one was found.
	* If this returns 'false' and you have shot all wumpuses and still not found the gold, the world is impossible to solve. */
	bool next_maybe_safe_unexplored(Coordinate& coords) const;

	//////////////////
	///   Fields   ///
private:

	std::unique_ptr<Data> _data;
};
