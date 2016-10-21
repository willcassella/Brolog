//World.h
#pragma once
#include <vector>
#include <ctime>
#include <set>
#include <cmath>

enum TileObservations
{
	NONE = 0,
	BREEZE = (1 << 0),
	STENCH = (1 << 1),
	BUMP = (1 << 2),
	GLIMMER = (1 << 3),
	PIT_DEATH = (1 << 4),
	WUMPUS_DEATH = (1 << 5)
};

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
bool operator!=(const Coordinate& lhs, const Coordinate& rhs)
{
	return !(lhs == rhs);
}

class World {
public:
	World(int size, float pWumpus, float pPit, float pObs) {
		srand(time(NULL));
		std::set<Coordinate> usedCoords;
		int numTiles = size * size;
		int numWumps = floor(pWumpus * numTiles);
		int numPits = floor(pPit * numTiles);
		int numObs = floor(pObs * numTiles);

		Coordinate temp;
		for (int i = 0; i < numObs; i++) {
			temp = randomCoord(size);
			while (usedCoords.find(temp) != usedCoords.end()) {
				temp = randomCoord(size);
			}
			usedCoords.insert(temp);
			obstacle_tiles.push_back(temp);
		}

		for (int i = 0; i < numWumps; i++) {
			temp = randomCoord(size);
			while (usedCoords.find(temp) != usedCoords.end()) {
				temp = randomCoord(size);
			}
			usedCoords.insert(temp);
			wumpus_tiles.push_back(temp);
		}

		for (int i = 0; i < numPits; i++) {
			temp = randomCoord(size);
			while (usedCoords.find(temp) != usedCoords.end()) {
				temp = randomCoord(size);
			}
			usedCoords.insert(temp);
			pit_tiles.push_back(temp);
		}
	}

	TileObservations getInfo(Coordinate youAreHere) {
		TileObservations Obs = NONE;
		if (youAreHere == gold_tile)
		{
			Obs = GLIMMER;
		}

		TileObservations percepts = search<STENCH>(youAreHere, wumpus_tiles) | search<BREEZE>(youAreHere, pit_tiles) | checkCurrent<BUMP>(youAreHere, obstacle_tiles)
			| checkCurrent<WUMPUS_DEATH>(youAreHere, wumpus_tiles) | checkCurrent<PIT_DEATH>(youAreHere, pit_tiles) | Obs;
	}

	template <TileObservations Obs>
	static TileObservations search(Coordinate coord, const std::vector<Coordinate>& tiles)
	{
		auto result = std::find(tiles.begin(), tiles.end(), coord.above());
		if (result != tiles.end())
		{
			return Obs;
		}

		result = std::find(tiles.begin(), tiles.end(), coord.below())
			if (result != tiles.end())
			{
				return Obs;
			}

		result = std::find(tiles.begin(), tiles.end(), coord.left())
			if (result != tiles.end())
			{
				return Obs;
			}

		result = std::find(tiles.begin(), tiles.end(), coord.right())
			if (result != tiles.end())
			{
				return Obs;
			}

		return NONE;
	}

	template <TileObservations Obs>
	static TileObservations checkCurrent(Coordinate coord, const std::vector<Coordinate>& tiles)
	{
		auto result = std::find(tiles.begin(), tiles.end(), coord);
		if (result != tiles.end()) {
			return Obs;
		}

		return 0;
	}

private:
	std::vector<Coordinate> wumpus_tiles;
	std::vector<Coordinate> pit_tiles;
	std::vector<Coordinate> obstacle_tiles;
	std::vector<Coordinate> reported_stenches;
	Coordinate gold_tile;

	Coordinate randomCoord(int size)
	{
		Coordinate temp;
		temp.x = randomInt(size);
		temp.y = randomInt(size);
		return temp;
	}

	int randomInt(int max)
	{
		return rand() % 100;
	}

};