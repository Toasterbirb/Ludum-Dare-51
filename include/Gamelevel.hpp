#pragma once
#include <string>
#include "Birb2D.hpp"
#include "ResourceLoader.hpp"

class GameLevel
{
public:
	GameLevel(std::string level_name, int tile_size, ResourceLoader& resources);
	~GameLevel();
	std::vector<Birb::Vector2int> GetLevelPixels() const;
	Birb::Scene LevelWallScene() const;

	Birb::Entity player;
	Birb::Entity diamond;

	/* Player controls */
	enum PlayerMoveDirection
	{
		left, right, up, down
	};
	void PlayerMove(PlayerMoveDirection direction);

private:
	bool CheckColor(Birb::Color color, Birb::Color target_color) const;
	void GenerateScene();
	void GeneratePlayer();
	void GenerateDiamond();
	int tile_size;

	ResourceLoader& resources;

	/* Level wall details and stuff */
	Birb::Vector2int level_dimensions;
	Birb::Vector2int* level_pixels;
	Birb::Scene level_scene;
	bool successful_level_load;

	Birb::Rect* level_tile_rects;

	/* Player stuff */
	Birb::Vector2int player_spawn_point;

	/* Diamond stuff */
	Birb::Vector2int diamond_spawn_point;
};
