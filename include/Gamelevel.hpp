#pragma once
#include <string>
#include "Birb2D.hpp"
#include "ResourceLoader.hpp"

struct Gate
{
	Birb::Entity entity;
	bool is_open = false;

	Birb::Texture& open_gate;
	Birb::Texture& closed_gate;

	Gate(ResourceLoader& resources, Birb::Vector2Int spawn_point, int tile_size, Birb::Random rand, bool rotated_gate = false)
	:open_gate(resources.open_gate), closed_gate(resources.closed_gate)
	{
		/* Decide if the gate should start as open or closed */
		int gate_starts_open = rand.RandomInt(0, 2);
		if (gate_starts_open)
		{
			is_open = true;
			entity.sprite = open_gate;
		}
		else
		{
			is_open = false;
			entity.sprite = closed_gate;
		}

		/* Set some base entity values */
		entity.rect = Birb::Rect(spawn_point.x * tile_size, spawn_point.y * tile_size, tile_size, tile_size);

		if (rotated_gate)
			entity.angle = 90;
	}

	void Open()
	{
		is_open = true;
		entity.sprite = open_gate;
	}

	void Close()
	{
		is_open = false;
		entity.sprite = closed_gate;
	}
};

struct Key
{
	Birb::Entity entity;

	Key(ResourceLoader& resources, Birb::Vector2Int spawn_point, int tile_size)
	{
		entity.rect = Birb::Rect(spawn_point.x * tile_size, spawn_point.y * tile_size, tile_size, tile_size);
		entity.sprite = resources.key_texture;
	}
};

class GameLevel
{
public:
	GameLevel();
	GameLevel(std::string level_name, int tile_size, ResourceLoader& resources);

	void Activate(); ///< Starts the level timer
	void Deactivate(); ///< Stops the level timer and AI

	std::vector<Birb::Vector2Int> GetLevelPixels() const;
	Birb::Scene LevelWallScene() const;
	Birb::Scene LevelGuardScene() const;
	Birb::Scene LevelGuardLampScene() const;
	Birb::Scene LevelGateScene() const;
	Birb::Scene LevelKeyScene() const;
	void GuardTick();

	Birb::Entity player;
	Birb::Entity diamond;
	bool level_finished;
	std::string timer_text() const;

	/* Player controls */
	enum PlayerMoveDirection
	{
		left, right, up, down
	};
	void PlayerMove(PlayerMoveDirection direction);
	void TenSecondTick();
	bool LevelLoadSuccessful() const;
	int KeyCount() const;
	void Free(); ///< Free all heap allocated stuff

private:
	bool CheckColor(Birb::Color color, Birb::Color target_color) const;
	void GenerateScene();
	void GeneratePlayer();
	void GenerateDiamond();
	void GenerateGuardScene();
	void GenerateGateScene();
	void GenerateKeyScene();
	void StartGuards();
	void StopGuards();
	int tile_size;

	ResourceLoader* resources;

	/* Level wall details and stuff */
	Birb::Vector2Int level_dimensions;
	Birb::Vector2Int* level_pixels;
	Birb::Scene level_scene;
	bool successful_level_load;

	Birb::Rect* level_tile_rects;

	/* Player stuff */
	void PlayerGuardCollisionCheck();
	void PlayerDiamondCollisionCheck();
	void PlayerKeyCollisionCheck();
	Birb::Vector2Int player_spawn_point;
	int player_next_to_gate;

	/* Diamond stuff */
	Birb::Vector2Int diamond_spawn_point;

	/* Guard stuff */
	bool level_has_guards;
	Birb::Random rand;
	std::vector<Birb::Vector2Int> guard_spawn_points;
	Birb::Scene guard_scene;
	Birb::Entity* guards;
	Birb::Polygon* guard_lamps;
	Birb::Scene guard_lamp_scene;
	int* last_guard_side;
	Birb::Timer guard_tick_timer;

	/* Gates */
	std::vector<Gate> gates;
	Birb::Scene gate_scene;

	/* Keys */
	std::vector<Key> keys;
	Birb::Scene key_scene;
	int key_count;

	/* The magical 10-second LD jam theme things */
	Birb::Timer ten_second_timer;

};
