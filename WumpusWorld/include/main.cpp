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

void debug_print(int worldSize, const Coordinate& pos, const KnowledgeDB& database)
{
	for (int y = worldSize - 1; y >= 0; --y)
	{
		for (int x = 0; x < worldSize; ++x)
		{
			Coordinate coords{ x, y };
			bool visited = database.known_visited(coords);

			if (coords == pos)
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
				if (coords == pos)
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

			if (coords == pos)
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

void smart_ineference(World world)
{
	// Create a knowledge base
	KnowledgeDB database(world.get_size());
	Coordinate location = world.get_start();

	do
	{
		// Get the percepts for the current location
		database.visited(location, world.get_percepts(location));

		// Print info
		debug_print(world.get_size(), location, database);
		std::cin.get();

		// Decide on the next move
		if (!database.next_safe_unexplored(location))
		{
			printf("Taking a chance...\n");
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
	World test(10, 0.0f, 0.1f, 0.3f);
	smart_ineference(test);
}