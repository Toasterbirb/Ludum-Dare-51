#include "Gamelevel.hpp"

using namespace Birb;

bool GameLevel::CheckColor(Birb::Color color, Birb::Color target_color) const
{
	if (color.r == target_color.r &&
		color.g == target_color.g &&
		color.b == target_color.b)
		return true;

	return false;
}

GameLevel::GameLevel(std::string level_name, int tile_size, ResourceLoader& resources)
:tile_size(tile_size), resources(resources)
{
	successful_level_load = false;

	std::string level_path = "./res/levels/" + level_name + "/";
	std::string level_file_path = level_path + "level_pixels.bmp";

	SDL_Surface* level_surface = SDL_LoadBMP(level_file_path.c_str());
	if (!level_surface)
	{
		Debug::Log("Couldn't load the level file [" + level_file_path + "]!", Debug::Type::error);
	}

	int w=0, h=0, p=0;
	Uint8* pixels = Resources::CopyTexturePixels(level_surface, &w, &h, &p);

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
			}
		}

		successful_level_load = true;
	}
	else
	{
		return;
	}

	/* Print out pixels */
	for (int j = 0; j < level_dimensions.y; ++j)
	{
		for (int i = 0; i < level_dimensions.x; ++i)
		{
			if (level_pixels[utils::FlatIndex({ i, j }, { level_dimensions.x, level_dimensions.y })] == Vector2int({ -1, -1 }))
			{
				std::cout << " ";
			}
			else
			{
				std::cout << "x";
			}
		}
		std::cout << "\n";
	}

	/* Generate the level scene */
	GenerateScene();
}

GameLevel::~GameLevel()
{
	delete level_pixels;

	if (successful_level_load)
		delete[] level_tile_rects;
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
	for (size_t i = 0; i < level_scene.GetObjects().size(); ++i)
	{
		if (Physics::RectCollision(test_pos, level_tile_rects[i]))
			return;
	}

	player.rect = test_pos;
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
}

void GameLevel::GeneratePlayer()
{
	Debug::Log("Generating the player entity");
	player.rect = Rect(
			(tile_size / 4.0) + (tile_size * player_spawn_point.x),
			(tile_size / 4.0) + (tile_size * player_spawn_point.y),
			tile_size / 2.0, tile_size / 2.0);
	player.rect.color = 0x7ead71;
	player.renderingPriority = 6;
}

void GameLevel::GenerateDiamond()
{
	if (!resources.diamond_texture.isLoaded())
		std::cout << "Problem with the diamond texture!" << std::endl;

	Debug::Log("Generating the diamond entity");
	diamond = Entity("Diamond", {0, 0, 32, 32}, resources.diamond_texture);
	diamond.rect = Rect(
			(tile_size * diamond_spawn_point.x),
			(tile_size * diamond_spawn_point.y),
			tile_size, tile_size);
	diamond.rect.color = 0x3e8cb1;
	diamond.renderingPriority = 5;
}
