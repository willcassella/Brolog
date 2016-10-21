//World.h
#pragma once
#include <vector>
#include <algorithm>
#include <ctime>
#include <set>
#include <cmath>

using TileObsT = unsigned;

namespace TileObs
{
	constexpr TileObsT NONE = 0;
	constexpr TileObsT BREEZE = (1 << 0);
	constexpr TileObsT STENCH = (1 << 1);
	constexpr TileObsT BUMP = (1 << 2);
	constexpr TileObsT GLIMMER = (1 << 3);
	constexpr TileObsT PIT_DEATH = (1 << 4);
	constexpr TileObsT WUMPUS_DEATH = (1 << 5);
}

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

	//////////////////
	///   Fields   ///
public:

	int x;
	int y;

	///////////////////
	///   Methods   ///
public:

	Coordinate above() const
	{
		return Coordinate{ x, y + 1 };
	}

	Coordinate below() const
	{
		return Coordinate{ x, y - 1 };
	}

	Coordinate left() const
	{
		return Coordinate{ x - 1, y };
	}

	Coordinate right() const
	{
		return Coordinate{ x + 1, y };
	}
};

inline bool operator==(const Coordinate& lhs, const Coordinate& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
inline bool operator!=(const Coordinate& lhs, const Coordinate& rhs)
{
	return !(lhs == rhs);
}

class World
{
	////////////////////////
	///   Constructors   ///
public:

	World(int size, float pWumpus, float pPit, float pObs)
	{
		srand((int)time(NULL));
		std::vector<Coordinate> usedCoords;
		int numTiles = size * size;
		int numWumps = (int)floor(pWumpus * numTiles);
		int numPits = (int)floor(pPit * numTiles);
		int numObs = (int)floor(pObs * numTiles);

		Coordinate temp = randomCoord(size);
		for (int i = 0; i < numObs; i++) {
			while (std::find(usedCoords.begin(), usedCoords.end(), temp) != usedCoords.end()) {
				temp = randomCoord(size);
			}
			usedCoords.push_back(temp);
			obstacle_tiles.push_back(temp);
		}

		for (int i = 0; i < numWumps; i++) {
			while (std::find(usedCoords.begin(), usedCoords.end(), temp) != usedCoords.end()) {
				temp = randomCoord(size);
			}
			usedCoords.push_back(temp);
			wumpus_tiles.push_back(temp);
		}

		for (int i = 0; i < numPits; i++) {
			while (std::find(usedCoords.begin(), usedCoords.end(), temp) != usedCoords.end()) {
				temp = randomCoord(size);
			}
			usedCoords.push_back(temp);
			pit_tiles.push_back(temp);
		}

		while (std::find(usedCoords.begin(), usedCoords.end(), temp) != usedCoords.end()) {
			temp = randomCoord(size);
		}
		gold_tile = temp;
		usedCoords.push_back(temp);
		while (std::find(usedCoords.begin(), usedCoords.end(), temp) != usedCoords.end()) {
			temp = randomCoord(size);
		}
		player_start = temp;
	}

	TileObsT getInfo(Coordinate youAreHere)
	{
		TileObsT percepts = TileObs::NONE;
		if (youAreHere == gold_tile)
		{
			percepts = TileObs::GLIMMER;
		}

		percepts |=
			search<TileObs::STENCH>(youAreHere, wumpus_tiles)
			| search<TileObs::BREEZE>(youAreHere, pit_tiles)
			| checkCurrent<TileObs::BUMP>(youAreHere, obstacle_tiles)
			| checkCurrent<TileObs::WUMPUS_DEATH>(youAreHere, wumpus_tiles)
			| checkCurrent<TileObs::PIT_DEATH>(youAreHere, pit_tiles);

		if ((percepts & TileObs::STENCH) != 0)
		{
			reported_stenches.push_back(youAreHere);
		}

		return percepts;
	}

	template <TileObsT Obs>
	static unsigned search(Coordinate coord, const std::vector<Coordinate>& tiles)
	{
		auto result = std::find(tiles.begin(), tiles.end(), coord.above());
		if (result != tiles.end())
		{
			return Obs;
		}

		result = std::find(tiles.begin(), tiles.end(), coord.below());
			if (result != tiles.end())
			{
				return Obs;
			}

		result = std::find(tiles.begin(), tiles.end(), coord.left());
			if (result != tiles.end())
			{
				return Obs;
			}

		result = std::find(tiles.begin(), tiles.end(), coord.right());
			if (result != tiles.end())
			{
				return Obs;
			}

		return 0;
	}

	template <TileObsT Obs>
	static unsigned checkCurrent(Coordinate coord, const std::vector<Coordinate>& tiles)
	{
		auto result = std::find(tiles.begin(), tiles.end(), coord);
		if (result != tiles.end()) {
			return Obs;
		}

		return TileObs::NONE;
	}

	Coordinate get_start() const
	{
		return player_start;
	}

	Coordinate get_gold() const
	{
		return gold_tile;
	}

private:

	std::vector<Coordinate> wumpus_tiles;
	std::vector<Coordinate> pit_tiles;
	std::vector<Coordinate> obstacle_tiles;
	std::vector<Coordinate> reported_stenches;
	Coordinate gold_tile;
	Coordinate player_start;

	static int randomInt(int max)
	{
		return rand() % max;
	}

	static Coordinate randomCoord(int size)
	{
		return Coordinate{ randomInt(size), randomInt(size) };
	}
};