// main.cpp

#include <future>
#include <iostream>
#include "../include/World.h"
#include "../include/KnowledgeDB.h"

void debug_print(int worldSize, const Coordinate& pos, const KnowledgeDB& database)
{
	for (int y = worldSize - 1; y >= 0; --y)
	{
		for (int x = 0; x < worldSize; ++x)
		{
			Coordinate coords{ x, y };
			bool visited = database.known_visited(coords);

			if (database.known_gold(coords))
			{
				printf("$");
			}
			else if (coords == pos)
			{
				printf("*");
			}
			else if (visited)
			{
				printf("<");
			}
			else
			{
				printf("(");
			}

			if (database.known_obstacle(coords))
			{
				printf("XXX> ");
				continue;
			}

			// If the tile contains a stench
			if (database.known_stench(coords))
			{
				printf("s");
			}
			else
			{
				printf(" ");
			}

			// If the tile contains a breeze
			if (database.known_breeze(coords))
			{
				printf("b");
			}
			else
			{
				printf(" ");
			}

			// If the tile contains a pit
			if (database.known_pit(coords))
			{
				printf("P");
			}
			else if (database.known_wumpus(coords))
			{
				printf("W");
			}
			else if (database.known_dead_wumpus(coords))
			{
				printf("D");
			}
			else
			{
				printf(" ");
			}

			if (database.known_gold(coords))
			{
				printf("$ ");
			}
			else if (coords == pos)
			{
				printf("* ");
			}
			else if (visited)
			{
				printf("> ");
			}
			else
			{
				printf(") ");
			}
		}

		printf("\n");
	}

	printf("\n\n\n");
}

BenchmarkResults smart_inference(World world)
{
	// Create a knowledge base
	KnowledgeDB database(world.get_size());

	// Instantiate player object
	Player player{ world.get_start(), world.num_wumpi() };

	// Report the starting conditions
	database.visited(player.location, world.explore(player.location));

	while (true)
	{
		// Get the next action reccomended from the database
		auto action = database.next_action(player);

		switch (action.type)
		{
		case Action::Type::MOVE:
			{
				// Get the percepts at that location
				auto percepts = world.explore(action.location);

				// Report them to the database
				database.visited(action.location, percepts);

				// Determine if we could actually move there
				if ((percepts & (TilePercepts::BUMP | TilePercepts::PIT_DEATH | TilePercepts::WUMPUS_DEATH)) == 0)
				{
					player.location = action.location;
				}
				break;
			}
		case Action::Type::SHOOT:
			{
				// Go to the shoot location (will be a previously visited tile)
				player.location = action.location;

				// Fire the arrow
				auto result = world.shoot_arrow(player.location, action.direction);
				player.num_arrows -= 1;

				// Report the results to the knowledge database if we hit anything
				if (result.hit)
				{
					database.dead_wumpus(result.invalidated_self_stench, result.invalidated_stenches);
				}

				break;
			}
		default:
			{
				// Handle's the 'GRAB' and 'STOP' cases, return
				return world.get_benchmark();
			}
		}

		// Debug print
		//debug_print(world.get_size(), player.location, database);
		//std::cin.get();
	}
}

int main()
{
	constexpr std::size_t NUM_RUNS = 100;
	constexpr int WORLD_SIZE = 15;

	// Vector to hold benchmark results
	std::vector<std::future<BenchmarkResults>> benchmarks;
	benchmarks.reserve(NUM_RUNS);

	// Start running benchmarks asynchronously
	for (int i = 0; i < NUM_RUNS; ++i)
	{
		// Generate a world
		World world(WORLD_SIZE, 0.05f, 0.05f, 0.1f);

		// Run the algorithm
		benchmarks.push_back(std::async(std::launch::async, smart_inference, world));
	}

	// Agregate benchmark totals
	std::size_t totalGoldFound = 0;
	std::size_t totalNumWumpusKilled = 0;
	std::size_t totalTilesExplored = 0;
	std::size_t totalWumpusDeaths = 0;
	std::size_t totalPitDeaths = 0;

	for (int i = 0; i < NUM_RUNS; ++i)
	{
		auto benchmark = benchmarks[i].get();
		std::cout << "Completed run " << i << std::endl;

		if (benchmark.found_gold)
		{
			totalGoldFound += 1;
		}

		totalNumWumpusKilled += benchmark.num_wumpi_killed;
		totalTilesExplored += benchmark.num_tiles_explored;
		totalWumpusDeaths += benchmark.times_killed_by_wumpus;
		totalPitDeaths += benchmark.times_killed_by_pit;
	}

	std::cout << "Number of runs: " << NUM_RUNS << std::endl;
	std::cout << "World size: " << WORLD_SIZE << std::endl;
	std::cout << "Total times gold found: " << totalGoldFound << std::endl;
	std::cout << "Total number of wumpi killed: " << totalNumWumpusKilled << std::endl;
	std::cout << "Total number of tiles explored: " << totalTilesExplored << std::endl;
	std::cout << "Times kiled by wumpus: " << totalWumpusDeaths << std::endl;
	std::cout << "Times killed by pit: " << totalPitDeaths << std::endl;
	std::cin.get();
}