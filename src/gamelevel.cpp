#include "Gamelevel.hpp"
#include <future>

using namespace Birb;

bool GameLevel::CheckColor(Birb::Color color, Birb::Color target_color) const
{
	if (color.r == target_color.r &&
		color.g == target_color.g &&
		color.b == target_color.b)
		return true;

	return false;
}

GameLevel::GameLevel()
{}

GameLevel::GameLevel(std::string level_name, int tile_size, ResourceLoader& resources)
:tile_size(tile_size), resources(&resources)
{
	/* Initialize some booleans */
	successful_level_load = false;
	level_has_guards = false;
	level_finished = false;

	std::string level_path = "./res/levels/" + level_name + "/";
	std::string level_file_path = level_path + "level_pixels.bmp";

	SDL_Surface* level_surface = SDL_LoadBMP(level_file_path.c_str());
	if (!level_surface)
	{
		Debug::Log("Couldn't load the level file [" + level_file_path + "]!", Debug::Type::error);
		return;
	}

	int w=0, h=0, p=0;
	Uint8* pixels = Resources::CopyTexturePixels(level_surface, &w, &h, &p);

	/* Free the surface */
	SDL_FreeSurface(level_surface);

	/* Allocate the level pixels */
	Vector2int level_dimensions = { 32, 18 };
	this->level_dimensions = level_dimensions;
	level_pixels = new Vector2int[level_dimensions.x * level_dimensions.y];

	/* Reset all of the level pixels */
	for (int i = 0; i < level_dimensions.x * level_dimensions.y; ++i)
		level_pixels[i] = { -1, -1 };

	if (pixels)
	{
		Debug::Log("Loading level: " + level_name);
		SDL_Color tmpColor;
		for (int i = 0; i < level_dimensions.x; ++i)
		{
			for (int j = 0; j < level_dimensions.y; ++j)
			{
				tmpColor = utils::TexturePixelToColor(pixels, Vector2int(i, j), level_dimensions.x);

				/* Check if the pixel is a level pixel */
				/* Levle pixel color = R: 67, G: 67, B: 67 */
				if (CheckColor(tmpColor, Color(67, 67, 67)))
				{
					level_pixels[utils::FlatIndex({ i, j }, level_dimensions)] = { i, j };
					continue;
				}

				/* Find the player spawn point */
				if (CheckColor(tmpColor, Color(0, 255, 0)))
				{
					player_spawn_point = { i , j };
					Debug::Log("Player spawn position: " + player_spawn_point.toString());
					continue;
				}

				/* Find the diamond spawn point */
				if (CheckColor(tmpColor, Color(139, 165, 255)))
				{
					diamond_spawn_point = { i, j };
					Debug::Log("Diamond spawn position: " + player_spawn_point.toString());
				}

				/* Find guard spawn positions */
				if (CheckColor(tmpColor, Color(255, 70, 100)))
				{
					level_has_guards = true;
					guard_spawn_points.push_back({ i, j });
					Debug::Log("Guard spawn position: " + guard_spawn_points[guard_spawn_points.size() - 1].toString());
				}
			}
		}

		successful_level_load = true;
	}
	else
	{
		return;
	}

	free (pixels);

	/* Print out pixels */
	for (int j = 0; j < level_dimensions.y; ++j)
	{
		for (int i = 0; i < level_dimensions.x; ++i)
		{
			if (level_pixels[utils::FlatIndex({ i, j }, { level_dimensions.x, level_dimensions.y })] == Vector2int({ -1, -1 }))
			{
				std::cout << "  ";
			}
			else
			{
				std::cout << " x";
			}
		}
		std::cout << "\n";
	}

	/* Generate the level scene */
	GenerateScene();
}

void GameLevel::Activate()
{
	/* Start the 10-second tick timer */
	ten_second_timer.Start();

	/* Activate guards */
	StartGuards();
}

void GameLevel::Deactivate()
{
	/* Stop the 10-second tick timer */
	ten_second_timer.Stop();

	/* Deactivate the guards */
	StopGuards();
}

std::vector<Vector2int> GameLevel::GetLevelPixels() const
{
	std::vector<Vector2int> pixel_list;
	pixel_list.reserve(level_dimensions.x * level_dimensions.y);

	for (int i = 0; i < level_dimensions.x; ++i)
	{
		for (int j = 0; j < level_dimensions.y; ++j)
		{
			int index = utils::FlatIndex({ i, j }, level_dimensions);
			if (level_pixels[index] != Vector2int({ -1, -1 }))
			{
				pixel_list.push_back(level_pixels[index]);
			}
		}
	}

	Debug::Log("Found " + std::to_string(pixel_list.size()) + " wall tiles in the level");
	return pixel_list;
}

Scene GameLevel::LevelWallScene() const
{
	return level_scene;
}

Scene GameLevel::LevelGuardScene() const
{
	/* Return an empty scene if there are not guards in the scene */
	if (level_has_guards)
		return guard_scene;
	else
		return Scene();
}

Scene GameLevel::LevelGuardLampScene() const
{
	/* Return an empty scene if there are not guards in the scene */
	if (level_has_guards)
		return guard_lamp_scene;
	else
		return Scene();
}

void GameLevel::StartGuards()
{
	GuardTick();
	//guard_tick_timer.Start();
}

void GameLevel::StopGuards()
{
	guard_tick_timer.Stop();
}

std::vector<std::future<void>> guard_futures;
static void MoveGuard(Entity* guard, const int& tile_size, const Scene& level_scene, Rect* level_tile_rects, Random rand, int* last_guard_side, size_t guard_index, Polygon* guard_lamps)
{
	Rect test_pos = guard->rect;

	/* Check all sides to find a direction that the guard can move to */
	Rect sides[4] = {
		Rect(test_pos.x + tile_size, test_pos.y, test_pos.w, test_pos.h, test_pos.color), // right
		Rect(test_pos.x - tile_size, test_pos.y, test_pos.w, test_pos.h, test_pos.color), // left
		Rect(test_pos.x, test_pos.y + tile_size, test_pos.w, test_pos.h, test_pos.color), // down
		Rect(test_pos.x, test_pos.y - tile_size, test_pos.w, test_pos.h, test_pos.color)  // up
	};


	std::vector<int> valid_sides;
	bool valid_side;
	for (int i = 0; i < 4; ++i)
	{
		/* Skip this side if it was used last time */
		if ((last_guard_side[guard_index] == 0 && i == 1) ||
			(last_guard_side[guard_index] == 1 && i == 0) ||
			(last_guard_side[guard_index] == 2 && i == 3) ||
			(last_guard_side[guard_index] == 3 && i == 2))
		{
			continue;
		}

		valid_side = true;

		for (int j = 0; j < level_scene.ObjectCount(); ++j)
		{
			if (Physics::RectCollision(sides[i], level_tile_rects[j]))
			{
				valid_side = false;
				break;
			}
		}

		if (valid_side)
			valid_sides.push_back(i);
	}

	/* Now that we (maybe) have some valid sides, decide which
	 * way to move to */

	/* No valid sides were found, just use the previous one as a last resort */
	int last_guard_side_opposite_index = 0;
	switch (last_guard_side[guard_index])
	{
		case (0):
			last_guard_side_opposite_index = 1;
			break;

		case (1):
			last_guard_side_opposite_index = 0;
			break;

		case (2):
			last_guard_side_opposite_index = 3;
			break;

		case (3):
			last_guard_side_opposite_index = 2;
			break;

		default:
			break;
	}

	if (valid_sides.size() == 0)
		valid_sides.push_back(last_guard_side_opposite_index);

	int random_num = rand.RandomInt(0, valid_sides.size() - 1);

	/* Set the new position */
	int side_index = valid_sides[random_num];
	guard->rect = sides[side_index];
	last_guard_side[guard_index] = side_index;

	/* Calculate new lamps for the guard */
	float lamp_size = 45.0f;
	Vector2f lamp_points[3];
	if (side_index == 0) // right
	{
		lamp_points[0] = { guard->rect.x + guard->rect.w, guard->rect.y };
		lamp_points[1] = { guard->rect.x + lamp_size + guard->rect.w, guard->rect.y + lamp_size / 2.0f };
		lamp_points[2] = { guard->rect.x + lamp_size + guard->rect.w, guard->rect.y - lamp_size / 2.0f };
	}
	else if (side_index == 1) // left
	{
		lamp_points[0] = { guard->rect.x, guard->rect.y };
		lamp_points[1] = { guard->rect.x - lamp_size, guard->rect.y + lamp_size / 2.0f };
		lamp_points[2] = { guard->rect.x - lamp_size, guard->rect.y - lamp_size / 2.0f };
	}
	else if (side_index == 2) // down
	{
		lamp_points[0] = { guard->rect.x, guard->rect.y + guard->rect.h };
		lamp_points[1] = { guard->rect.x + lamp_size / 2.0f, guard->rect.y + lamp_size + guard->rect.h };
		lamp_points[2] = { guard->rect.x - lamp_size / 2.0f, guard->rect.y + lamp_size + guard->rect.h };
	}
	else if (side_index == 3) // up
	{
		lamp_points[0] = { guard->rect.x, guard->rect.y };
		lamp_points[1] = { guard->rect.x + lamp_size / 2.0f, guard->rect.y - lamp_size };
		lamp_points[2] = { guard->rect.x - lamp_size / 2.0f, guard->rect.y - lamp_size };
	}

	guard_lamps[guard_index] = Polygon(lamp_points, 3, 0xb7c062);
	guard_lamps[guard_index].renderingPriority = 1;
}

void GameLevel::PlayerGuardCollisionCheck()
{
	for (size_t i = 0; i < guard_spawn_points.size(); ++i)
	{
		if (Physics::PolygonCollision(guard_lamps[i], player.rect.toPolygon()))
		{
			Debug::Log("Player got caught!");
			std::cout << "Player spawn point: " << player_spawn_point << std::endl;
			GeneratePlayer();
		}
	}
}

void GameLevel::PlayerDiamondCollisionCheck()
{
	if (Physics::EntityCollision(player, diamond))
		level_finished = true;
}

void GameLevel::GuardTick()
{
	/* Don't do anything if the level doesn't have any guards */
	if (!level_has_guards)
		return;

	//if (guard_tick_timer.ElapsedSeconds() > 1)
	//{
	for (size_t i = 0; i < guard_spawn_points.size(); ++i)
	{
		guard_futures.push_back(std::async(std::launch::async, MoveGuard, &guards[i], tile_size, level_scene, level_tile_rects, rand, last_guard_side, i, guard_lamps));
	}

	/* Wait for the threads to finish */
	for (size_t i = 0; i < guard_futures.size(); ++i)
	{
		guard_futures[i].wait();
	}

	/* Clear out all futures */
	guard_futures.clear();

	/* Check if the player got hit by any of the guard flashlights */
	PlayerGuardCollisionCheck();

	/* Restart the timer */
	guard_tick_timer.Start();
	//}
}

void GameLevel::PlayerMove(PlayerMoveDirection direction)
{
	Rect test_pos = player.rect;
	switch (direction)
	{
		case (left):
			test_pos.x -= tile_size;
			break;

		case (right):
			test_pos.x += tile_size;
			break;

		case (up):
			test_pos.y -= tile_size;
			break;

		case (down):
			test_pos.y += tile_size;
			break;
	}

	/* Test for collision */
	for (int i = 0; i < level_scene.ObjectCount(); ++i)
	{
		if (Physics::RectCollision(test_pos, level_tile_rects[i]))
			return;
	}

	player.rect = test_pos;

	/* Guard tick */
	GuardTick();

	/* Check for guard collision */
	PlayerGuardCollisionCheck();

	/* Check if the player collided with the diamond */
	PlayerDiamondCollisionCheck();
}

void GameLevel::TenSecondTick()
{
	if (ten_second_timer.ElapsedSeconds() > 10.0)
	{
		/* Call a guard tick */
		GuardTick();

		/* Move a random tile that isn't one of the outer bound tiles */
		bool found_working_tile = false;
		do
		{
			int random_tile_index = rand.RandomInt(0, level_scene.ObjectCount());

			/* Make sure that the tile isn't a border tile */
			if (level_tile_rects[random_tile_index].y == 0)
				continue;

			if (level_tile_rects[random_tile_index].x == 0)
				continue;

			if (level_tile_rects[random_tile_index].y == tile_size * (level_dimensions.y - 1))
				continue;

			if (level_tile_rects[random_tile_index].x == tile_size * (level_dimensions.x - 1))
				continue;

			int i = rand.RandomInt(0, 2);
			int j = rand.RandomInt(0, 2);
			Rect new_position = level_tile_rects[random_tile_index];

			if (i == 0)
				new_position.x += tile_size;
			else
				new_position.x -= tile_size;

			if (j == 0)
				new_position.y += tile_size;
			else
				new_position.y -= tile_size;

			/* Don't move tiles over the diamond */
			if (Physics::RectCollision(diamond.rect, new_position))
				continue;

			/* Don't move tiles over the player */
			if (Physics::RectCollision(player.rect, new_position))
				continue;

			/* Don't move tiles over the guards */
			for (size_t i = 0; i < guard_spawn_points.size(); ++i)
			{
				if (Physics::RectCollision(guards[i].rect, new_position))
					continue;
			}

			found_working_tile = true;
			level_tile_rects[random_tile_index] = new_position;

		} while (!found_working_tile);


		ten_second_timer.Start();
	}
}

bool GameLevel::LevelLoadSuccessful() const
{
	return successful_level_load;
}

void GameLevel::Free()
{
	if (successful_level_load)
	{
		delete[] level_pixels;
		delete[] level_tile_rects;

		if (level_has_guards)
		{
			delete[] guards;
			delete[] last_guard_side;
			delete[] guard_lamps;
		}
	}
}


void GameLevel::GenerateScene()
{
	Debug::Log("Generating the level scene");

	std::vector<Vector2int> level_tile_positions = GetLevelPixels();

	/* Create a rectangle for each of the tiles */
	level_tile_rects = new Rect[level_tile_positions.size()];
	for (size_t i = 0; i < level_tile_positions.size(); ++i)
	{
		level_tile_rects[i] = Rect(
				tile_size * level_tile_positions[i].x,
				tile_size * level_tile_positions[i].y,
				tile_size, tile_size);
		level_tile_rects[i].color = 0x403e3e;
		level_tile_rects[i].renderingPriority = 2;
		level_scene.AddObject(&level_tile_rects[i]);
	}

	/* Create the player entity */
	GeneratePlayer();

	/* Create the diamond entity */
	GenerateDiamond();

	/* Create guard entities and make a scene for them */
	if (level_has_guards)
		GenerateGuardScene();
}

void GameLevel::GeneratePlayer()
{
	Debug::Log("Spawning the player entity");
	player.rect = Rect(
			(tile_size / 4.0) + (tile_size * player_spawn_point.x),
			(tile_size / 4.0) + (tile_size * player_spawn_point.y),
			tile_size / 2.0, tile_size / 2.0);
	player.rect.color = 0x7ead71;
	player.renderingPriority = 6;
}

void GameLevel::GenerateDiamond()
{
	if (!resources->diamond_texture.isLoaded())
		std::cout << "Problem with the diamond texture!" << std::endl;

	Debug::Log("Generating the diamond entity");
	diamond = Entity("Diamond", {0, 0, 32, 32}, resources->diamond_texture);
	diamond.rect = Rect(
			(tile_size * diamond_spawn_point.x),
			(tile_size * diamond_spawn_point.y),
			tile_size, tile_size);
	diamond.rect.color = 0x3e8cb1;
	diamond.renderingPriority = 5;
}

void GameLevel::GenerateGuardScene()
{
	/* Allocate guard lamps */
	guard_lamps = new Polygon[guard_spawn_points.size()];

	last_guard_side = new int[guard_spawn_points.size()];
	guards = new Entity[guard_spawn_points.size()];
	for (size_t i = 0; i < guard_spawn_points.size(); ++i)
	{
		guards[i].rect = Rect(
			(tile_size / 4.0) + (tile_size * guard_spawn_points[i].x),
			(tile_size / 4.0) + (tile_size * guard_spawn_points[i].y),
			tile_size / 2.0, tile_size / 2.0 );

		guards[i].rect.color = 0xb13e51;
		guard_scene.AddObject(&guards[i]);
		guard_lamp_scene.AddObject(&guard_lamps[i]);

		/* Fill the guard lamps with some data */
		guard_lamps[i] = Polygon({Vector2f(-1, -1), Vector2f(-1, -1), Vector2f(-1, -1)});

		/* Fill last guard sides with some random data */
		last_guard_side[i] = rand.RandomInt(0, 4);
	}

}
