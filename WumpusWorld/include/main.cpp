// main.cpp

#include <iostream>
#include "../include/World.h"
#include "../include/KnowledgeDB.h"

struct Player
{
	//////////////////
	///   Fields   ///
public:

	Coordinate location;
	std::size_t num_arrows;
};

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
				if (database.known_gold(coords))
				{
					printf("XXXX$ ");
				}
				else if (coords == pos)
				{
					printf("XXXX* ");
				}
				else
				{
					printf("XXXX> ");
				}
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
			else
			{
				printf(" ");
			}

			// If the database contains a wumpus
			if (database.known_wumpus(coords))
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

BenchmarkResults smart_ineference(World world)
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
		auto action = database.next_action(player.location);

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
		debug_print(world.get_size(), player.location, database);
		std::cin.get();
	}
}

int main()
{
	World test(5, 0.05f, 0.05f, 0.1f);
	smart_ineference(test);
}