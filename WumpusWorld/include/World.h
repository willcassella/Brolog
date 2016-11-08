//World.h
#pragma once

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <vector>

struct Coordinate
{
	////////////////////////
	///   Constructors   ///
public:

	Coordinate()
		: x(0), y(0)
	{
	}
	Coordinate(int x, int y)
		: x(x), y(y)
	{
	}

	/* Randomly generates a coordinate between (0, 0) and (max, max).
	 * It will not pick a coordinate within the given list of disallowed coordinates. */
	static Coordinate random(int max, const std::vector<Coordinate>& disallowed)
	{
		assert(disallowed.size() < static_cast<std::size_t>(max * max));

		Coordinate result;

		do
		{
			result = { rand() % max, rand() % max };
		} while (std::find(disallowed.begin(), disallowed.end(), result) != disallowed.end());

		return result;
	}

	//////////////////s/
	///   Methods   ///
public:

	/* Creates a coordinate one unit north of this tile. */
	Coordinate north() const
	{
		return Coordinate{ x, y + 1 };
	}

	/* Creates a coordinate one unit south of this tile. */
	Coordinate south() const
	{
		return Coordinate{ x, y - 1 };
	}

	/* Creates a coordinate one unit east of this tile. */
	Coordinate east() const
	{
		return Coordinate{ x - 1, y };
	}

	/* Creates a coordinate one unit west of this tile. */
	Coordinate west() const
	{
		return Coordinate{ x + 1, y };
	}

	/////////////////////
	///   Operators   ///
public:

	bool operator==(const Coordinate& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Coordinate& rhs) const
	{
		return !(*this == rhs);
	}

	//////////////////
	///   Fields   ///
public:

	int x;
	int y;
};

using TilePercepts_t = int;
namespace TilePercepts
{
	enum
	{
		NONE = 0,
		BUMP = (1 << 0),
		BREEZE = (1 << 1),
		STENCH = (1 << 2),
		GLIMMER = (1 << 3),
		PIT_DEATH = (1 << 4),
		WUMPUS_DEATH = (1 << 5)
	};
}

using Direction_t = int;
namespace Direction
{
	enum
	{
		NORTH = (1 << 0),
		SOUTH = (1 << 1),
		EAST = (1 << 2),
		WEST = (1 << 3)
	};
}

struct ShootArrowResult
{
	//////////////////
	///   Fields   ///
public:

	/* Whether the arrow killed a wumpus. */
	bool hit = false;

	/* If the arrow killed a wumpus, if we can remove the stench on that wumpus' tile. */
	bool invalidated_self_stench = false;

	/* If the arrow killed a wumpus, the tiles surrounding the wumpus's tile we can remove. */
	Direction_t invalidated_stenches = 0;
};

struct BenchmarkResults
{
	//////////////////
	///   Fields   ///
public:

	bool found_gold = false;
	std::size_t num_tiles_explored = 0;
	std::size_t num_wumpi_killed = 0;
	std::size_t times_killed_by_pit = 0;
	std::size_t times_killed_by_wumpus = 0;
};

class World
{
	////////////////////////
	///   Constructors   ///
public:

	/* Initializes a world with the size (all worlds are square), probability of spawning a wumpus, probability of spawning
	 * a pit, and probability of spawning an obstacle. */
	World(int size, float pWumpus, float pPit, float pObs)
		: world_size(size)
	{
		// Calculate the total number of all types of tiles (given their probablilities)
		const int numTiles = size * size;
		const int numWumps = (int)floor(pWumpus * numTiles);
		const int numPits = (int)floor(pPit * numTiles);
		const int numObs = (int)floor(pObs * numTiles);

		// Make sure we won't run out of tiles
		assert(numWumps + numPits + numObs + 1 < numTiles);

		// Stores all currently used coordinates
		std::vector<Coordinate> usedCoords;
		usedCoords.reserve(numWumps + numPits + numObs);

		// Instantiate all obstacles
		for (int i = 0; i < numObs; i++)
		{
			auto coord = Coordinate::random(size, usedCoords);
			usedCoords.push_back(coord);
			obstacle_tiles.push_back(coord);
		}

		// Instantiate all wumpi
		for (int i = 0; i < numWumps; i++)
		{
			auto coord = Coordinate::random(size, usedCoords);
			usedCoords.push_back(coord);
			wumpus_tiles.push_back(coord);
		}

		// Instantiate all pits
		for (int i = 0; i < numPits; i++)
		{
			auto coord = Coordinate::random(size, usedCoords);
			usedCoords.push_back(coord);
			pit_tiles.push_back(coord);
		}

		// Instantiate the gold
		gold_tile = Coordinate::random(size, usedCoords);
		usedCoords.push_back(gold_tile);

		// Set player start
		player_start = Coordinate::random(size, usedCoords);
	}

	///////////////////
	///   Methods   ///
public:

	/* Returns the size of the world. */
	int get_size() const
	{
		return world_size;
	}

	/* Returns the starting coordinates of the player. */
	Coordinate get_start() const
	{
		return player_start;
	}

	/* Returns the number of wumpus' in the world. */
	std::size_t num_wumpi() const
	{
		return wumpus_tiles.size();
	}

	/* Returns the percepts the player experiences exploring this tile, and sets 'coord' to their new location. */
	TilePercepts_t explore(Coordinate coord)
	{
		_benchmark.num_tiles_explored += 1;

		// If they're on an obstacle
		if (has_obstacle(coord))
		{
			// They only feel a bump, coord does not change
			return TilePercepts::BUMP;
		}

		TilePercepts_t percepts = TilePercepts::NONE;

		// If there's gold on the tile
		if (coord == gold_tile)
		{
			// They perceive a glimmer
			percepts |= TilePercepts::GLIMMER;
			_benchmark.found_gold = true;
		}

		// If there's a breeze on the tile
		if (has_breeze(coord))
		{
			percepts |= TilePercepts::BREEZE;
		}

		// If there's a wumpus on the tile or any of the neighboring tiles
		if (has_stench(coord))
		{
			percepts |= TilePercepts::STENCH;
		}

		// Report if the player died
		if (has_wumpus(coord))
		{
			percepts |= TilePercepts::WUMPUS_DEATH;
			_benchmark.times_killed_by_wumpus += 1;
		}
		else if (has_pit(coord))
		{
			percepts |= TilePercepts::PIT_DEATH;
			_benchmark.times_killed_by_pit += 1;
		}

		return percepts;
	}

	/* Returns whether  */
	ShootArrowResult shoot_arrow(Coordinate coord, Direction_t direction)
	{
		ShootArrowResult result;
		int* component = nullptr;
		int incr = 0;

		// Determine how to increment direction
		switch (direction)
		{
		case Direction::NORTH:
			component = &coord.y;
			incr = 1;
			break;

		case Direction::SOUTH:
			component = &coord.y;
			incr = -1;
			break;

		case Direction::EAST:
			component = &coord.x;
			incr = -1;
			break;

		case Direction::WEST:
			component = &coord.x;
			incr = 1;
			break;

		default:
			// Invalid arguments
			return result;
		}

		// Determine if the arrow hit a wumpus
		for (; coord.x >= 0 && coord.x < world_size && coord.y >= 0 && coord.y < world_size; *component += incr)
		{
			if (has_wumpus(coord))
			{
				result.hit = true;
				break;
			}

			if (has_obstacle(coord))
			{
				break;
			}
		}

		// If we didn't hit a wumpus, just return
		if (!result.hit)
		{
			return result;
		}
		else
		{
			_benchmark.num_wumpi_killed += 1;
		}

		// Figure out which stenches have been invalidated
		auto iter = std::find(wumpus_tiles.begin(), wumpus_tiles.end(), coord);
		wumpus_tiles.erase(iter);

		// Determine which coordinates now longer have a stench
		if (!has_stench(coord))
		{
			result.invalidated_self_stench = true;
		}

		if (!has_stench(coord.north()))
		{
			result.invalidated_stenches |= Direction::NORTH;
		}

		if (!has_stench(coord.south()))
		{
			result.invalidated_stenches |= Direction::SOUTH;
		}

		if (!has_stench(coord.east()))
		{
			result.invalidated_stenches |= Direction::EAST;
		}

		if (!has_stench(coord.west()))
		{
			result.invalidated_stenches |= Direction::WEST;
		}

		return result;
	}

	/* Returns the benchmark object for the player so far. */
	BenchmarkResults get_benchmark() const
	{
		return _benchmark;
	}

private:

	/* Returns whether the given coordinate contains a stench. */
	bool has_stench(Coordinate coord) const
	{
		return has_wumpus(coord) || check_neighbors(coord, wumpus_tiles);
	}

	/* Returns whether the given coordinate contains a breeze. */
	bool has_breeze(Coordinate coord) const
	{
		return has_pit(coord) || check_neighbors(coord, pit_tiles);
	}

	/* Returns whether the given tile coordinate contains a wumpus. */
	bool has_wumpus(Coordinate coord) const
	{
		return std::find(wumpus_tiles.begin(), wumpus_tiles.end(), coord) != wumpus_tiles.end();
	}

	/* Returns whether the given tile coordinate contains a pit. */
	bool has_pit(Coordinate coord) const
	{
		return std::find(pit_tiles.begin(), pit_tiles.end(), coord) != pit_tiles.end();
	}

	/* Returns whether the given tile coordinate contains an obstacle. */
	bool has_obstacle(Coordinate coord) const
	{
		return std::find(obstacle_tiles.begin(), obstacle_tiles.end(), coord) != obstacle_tiles.end();
	}

	/* Checks if the given coordinate neighbors any of the the given coordinates, and returns the percept if it does. */
	static bool check_neighbors(Coordinate coord, const std::vector<Coordinate>& tiles)
	{
		// See if the tile above is in the list
		auto result = std::find(tiles.begin(), tiles.end(), coord.north());
		if (result != tiles.end())
		{
			return true;
		}

		// See if the tile below is in the list
		result = std::find(tiles.begin(), tiles.end(), coord.south());
		if (result != tiles.end())
		{
			return true;
		}

		// See if the tile to the left is in the list
		result = std::find(tiles.begin(), tiles.end(), coord.east());
		if (result != tiles.end())
		{
			return true;
		}

		// See if the tile to the right is in the list
		result = std::find(tiles.begin(), tiles.end(), coord.west());
		if (result != tiles.end())
		{
			return true;
		}

		return false;
	}

	//////////////////
	///   Fields   ///
private:

	std::vector<Coordinate> wumpus_tiles;
	std::vector<Coordinate> pit_tiles;
	std::vector<Coordinate> obstacle_tiles;
	Coordinate gold_tile;
	Coordinate player_start;
	int world_size;

	BenchmarkResults _benchmark;
};
