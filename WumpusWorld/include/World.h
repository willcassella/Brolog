//World.h
#pragma once
#include <vector>
#include <algorithm>
#include <ctime>
#include <set>
#include <cmath>

namespace TileObservations
{
	constexpr unsigned NONE = 0;
	constexpr unsigned BREEZE = (1 << 0);
	constexpr unsigned STENCH = (1 << 1);
	constexpr unsigned BUMP = (1 << 2);
	constexpr unsigned GLIMMER = (1 << 3);
	constexpr unsigned PIT_DEATH = (1 << 4);
	constexpr unsigned WUMPUS_DEATH = (1 << 5);
}

struct Coordinate
{
	int x;
	int y;

	Coordinate above() {
		Coordinate temp;
		temp.x = x;
		temp.y = y + 1;
		return temp;
	}

	Coordinate below() {
		Coordinate temp;
		temp.x = x;
		temp.y = y - 1;
		return temp;
	}

	Coordinate left() {
		Coordinate temp;
		temp.x = x - 1;
		temp.y = y;
		return temp;
	}

	Coordinate right() {
		Coordinate temp;
		temp.x = x + 1;
		temp.y = y;
		return temp;
	}
};

bool operator==(const Coordinate& lhs, const Coordinate& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
bool operator<(const Coordinate& lhs, const Coordinate& rhs)
{
	return (lhs.x < rhs.x && lhs.y < rhs.y);
}
bool operator!=(const Coordinate& lhs, const Coordinate& rhs)
{
	return !(lhs == rhs);
}

class World {
public:
	World(int size, float pWumpus, float pPit, float pObs) {
		worldSize = size;
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

	unsigned getInfo(Coordinate youAreHere) {
		unsigned Obs = TileObservations::NONE;
		if (youAreHere == gold_tile)
		{
			Obs = TileObservations::GLIMMER;
		}

		unsigned percepts = search<TileObservations::STENCH>(youAreHere, wumpus_tiles) | search<TileObservations::BREEZE>(youAreHere, pit_tiles) | checkCurrent<TileObservations::BUMP>(youAreHere, obstacle_tiles)
			| checkCurrent<TileObservations::WUMPUS_DEATH>(youAreHere, wumpus_tiles) | checkCurrent<TileObservations::PIT_DEATH>(youAreHere, pit_tiles) | Obs;
		if (percepts && TileObservations::STENCH != 0)
		{
			reported_stenches.push_back(youAreHere);
		}
		return percepts;
	}

	template <unsigned Obs>
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

	template <unsigned Obs>
	static unsigned checkCurrent(Coordinate coord, const std::vector<Coordinate>& tiles)
	{
		auto result = std::find(tiles.begin(), tiles.end(), coord);
		if (result != tiles.end()) {
			return Obs;
		}

		return 0;
	}

	Coordinate get_start()
	{
		return player_start;
	}

	Coordinate get_gold()
	{
		return gold_tile;
	}

	bool shootArrow(Coordinate youAreHere, int direction)
	{
		//0 = left, 1 = up, 2 = right, 3 = down
		Coordinate temp = youAreHere;
		switch (direction)
		{
		case 0:
			for (int i = youAreHere.x; i >= 0; i--) {
				temp.x = i;
				if (isWumpus(temp)) {
					return true;
				}
				if (isObstacle(temp)) {
					return false;
				}
			}
			break;
		case 1:
			for (int i = youAreHere.y; i < worldSize; i++) {
				temp.x = i;
				if (isWumpus(temp)) {
					return true;
				}
				if (isObstacle(temp)) {
					return false;
				}
			}
			break;
		case 2:
			for (int i = youAreHere.x; i < worldSize; i--) {
				temp.x = i;
				if (isWumpus(temp)) {
					return true;
				}
				if (isObstacle(temp)) {
					return false;
				}
			}
			break;
		case 3:
			for (int i = youAreHere.y; i >= 0; i--) {
				temp.x = i;
				if (isWumpus(temp)) {
					return true;
				}
				if (isObstacle(temp)) {
					return false;
				}
			}
			break;
		default:
			break;
		}
		return false;
	}
	bool isWumpus(Coordinate youAreHere)
	{
		if ((getInfo(youAreHere) & TileObservations::WUMPUS_DEATH) != 0) {
			return true;
		}
		return false;
		
	}
	bool isObstacle(Coordinate youAreHere)
	{
		if ((getInfo(youAreHere) & TileObservations::BUMP) != 0) {
			return true;
		}
		return false;
	}

private:
	std::vector<Coordinate> wumpus_tiles;
	std::vector<Coordinate> pit_tiles;
	std::vector<Coordinate> obstacle_tiles;
	std::vector<Coordinate> reported_stenches;
	Coordinate gold_tile;
	Coordinate player_start;
	int worldSize;

	Coordinate randomCoord(int size)
	{
		Coordinate temp;
		temp.x = randomInt(size);
		temp.y = randomInt(size);
		return temp;
	}

	int randomInt(int max)
	{
		return rand() % max;
	}

};