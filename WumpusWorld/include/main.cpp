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
	int num_arrows;
};

void debug_print(int worldSize, const KnowledgeDB& database)
{
	for (int x = 0; x < worldSize; ++x)
	{
		for (int y = 0; y < worldSize; ++y)
		{
			Coordinate coords{ x, y };
			bool visited = database.known_visited(coords);

			if (visited)
			{
				printf("<");
			}
			else
			{
				printf("(");
			}

			if (database.known_obstacle(coords))
			{
				printf("XXXX> ");
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

			if (visited)
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

void smart_ineference(World world)
{
	// Create a knowledge base
	KnowledgeDB database(world.get_size());
	Coordinate location = world.get_start();

	do
	{
		// Get info for location
		database.visited(location, world.getInfo(location));

		// Print info
		debug_print(world.get_size(), database);
		std::cin.get();

		// Decide on the next move
		if (!database.next_safe_unexplored(location))
		{
			if (!database.next_maybe_safe_unexplored(location))
			{
				printf("World impossible");
				std::cin.get();
				exit(0);
			}
		}
	} while (true);
}

int main()
{
	World test(10, 0.1f, 0.03f, 0.04f);
	smart_ineference(test);
}