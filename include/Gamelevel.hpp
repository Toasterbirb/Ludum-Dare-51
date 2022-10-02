#pragma once
#include <string>
#include "Birb2D.hpp"
#include "ResourceLoader.hpp"

class GameLevel
{
public:
	GameLevel();
	GameLevel(std::string level_name, int tile_size, ResourceLoader& resources);

	void Activate(); ///< Starts the level timer
	void Deactivate(); ///< Stops the level timer and AI

	std::vector<Birb::Vector2int> GetLevelPixels() const;
	Birb::Scene LevelWallScene() const;
	Birb::Scene LevelGuardScene() const;
	Birb::Scene LevelGuardLampScene() const;
	void GuardTick();

	Birb::Entity player;
	Birb::Entity diamond;
	bool level_finished;

	/* Player controls */
	enum PlayerMoveDirection
	{
		left, right, up, down
	};
	void PlayerMove(PlayerMoveDirection direction);
	void TenSecondTick();
	bool LevelLoadSuccessful() const;
	void Free(); ///< Free all heap allocated stuff

private:
	bool CheckColor(Birb::Color color, Birb::Color target_color) const;
	void GenerateScene();
	void GeneratePlayer();
	void GenerateDiamond();
	void GenerateGuardScene();
	void StartGuards();
	void StopGuards();
	int tile_size;

	ResourceLoader* resources;

	/* Level wall details and stuff */
	Birb::Vector2int level_dimensions;
	Birb::Vector2int* level_pixels;
	Birb::Scene level_scene;
	bool successful_level_load;

	Birb::Rect* level_tile_rects;

	/* Player stuff */
	void PlayerGuardCollisionCheck();
	void PlayerDiamondCollisionCheck();
	Birb::Vector2int player_spawn_point;

	/* Diamond stuff */
	Birb::Vector2int diamond_spawn_point;

	/* Guard stuff */
	bool level_has_guards;
	Birb::Random rand;
	std::vector<Birb::Vector2int> guard_spawn_points;
	Birb::Scene guard_scene;
	Birb::Entity* guards;
	Birb::Polygon* guard_lamps;
	Birb::Scene guard_lamp_scene;
	int* last_guard_side;
	Birb::Timer guard_tick_timer;

	/* The magical 10-second LD jam theme things */
	Birb::Timer ten_second_timer;

};
