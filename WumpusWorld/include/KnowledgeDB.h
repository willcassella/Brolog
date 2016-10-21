// KnowledgeDB.h
#pragma once

#include <memory>
#include "World.h"

class KnowledgeDB
{
	struct Data;

	////////////////////////
	///   Constructors   ///
public:

	KnowledgeDB(int size);
	~KnowledgeDB();

	///////////////////
	///   Methods   ///
public:

	/* Reports that a tile was visited, and what was observed on that tile. */
	void visited(const Coordinate& coord, TileObsT observations);

	/* Reports that a wumpus has been killed on the given tile, and which stench tiles have been eliminated. */
	void dead_wumpus(const Coordinate& coord, const std::vector<Coordinate>& invalidatedStenches);

	/* Attemps to deduce a known wumpus location, and returns whether one was found. */
	bool next_wumpus(Coordinate& coords) const;

	/* Attempts to find a tile that is known to be both safe and unexplored, and returns whether one was found. */
	bool next_safe_unexplored(Coordinate& coords) const;

	/* If the above query fails, you can attempt to find a tile that is unexplored, and not known to be unsafe. Returns whether one was found.
	 * If this returns 'false' and you have shot all wumpuses and still not found the gold, the world is impossible to solve. */
	bool next_maybe_safe_unexplored(Coordinate& coords) const;

	/* Returns whether the given tile is known to be visited. Used for debuggin. */
	bool known_visited(const Coordinate& coords) const;

	/* Returns whether the given tile is known to contian a stench. Used for debugging. */
	bool known_stench(const Coordinate& coords) const;

	/* Returns whether the given tile is known to contain a breeze. Used for debuggin. */
	bool known_breeze(const Coordinate& coords) const;

	/* Returns whether the given tile is known to contain an obstacle. Used for debuggin. */
	bool known_obstacle(const Coordinate& coords) const;

	/* Returns whether the given tile is known to contain a pit. Used for debugging. */
	bool known_pit(const Coordinate& coords) const;

	/* Returns whether the given tile is known to contain a wumpus. Used for debugging. */
	bool known_wumpus(const Coordinate& coords) const;

	/* Returns whether the given tile is known to contain a dead wumpus. Used for debugging. */
	bool known_dead_wumpus(const Coordinate& coords) const;

	//////////////////
	///   Fields   ///
private:

	std::unique_ptr<Data> _data;
};
