#include "Birb2D.hpp"
#include "Gamelevel.hpp"
#include "ResourceLoader.hpp"

using namespace Birb;

int main(int argc, char** argv)
{
	Debug::Log("Creating the window");
	Window window("LD51: Diamond heist", Vector2int(1280, 720), 60, false);
	TimeStep timeStep;
	timeStep.Init(&window);

	Scene level_background;
	Rect background_rect(0, 0, window.dimensions.x, window.dimensions.y);
	background_rect.color = 0x1a1a1a;
	background_rect.renderingPriority = -1;
	level_background.AddObject(&background_rect);

	ResourceLoader resources;

	/* The victory screen stuff */
	Scene victory_scene;
	Font small_font("./res/fonts/fortzilla/Fortzilla.ttf", 24);
	Font big_font("./res/fonts/fortzilla/Fortzilla.ttf", 42);
	Entity congratulation_text("Victory text", Vector2int(128, 128), EntityComponent::Text("Congratulations!", &big_font, &Colors::White));
	Entity congratulation_text_2("Victory text 2", Vector2int(128, 256), EntityComponent::Text("You managed to yoink all of the diamonds\nand survived all of the challenges\nthat this game threw at you\n\nNow go play some other Ludum Dare games ;)", &small_font, &Colors::White));

	victory_scene.AddObject(&congratulation_text);
	victory_scene.AddObject(&congratulation_text_2);

	/* Read all of the levels */
	int current_level = 0;
	const int level_count = 5;
	std::string level_names[level_count] = {
		"level_1",
		"level_2",
		"level_3",
		"level_4",
		"test_level"
	};

	GameLevel game_levels[level_count];
	for (int i = 0; i < level_count; ++i)
		game_levels[i] = GameLevel(level_names[i], 40, resources);

	/* Check if all of the levels loaded up fine */
	for (int i = 0; i < level_count; ++i)
	{
		if (!game_levels[i].LevelLoadSuccessful())
			Debug::Log("Couldn't load level [" + std::to_string(i + 1) + "]", Debug::Type::error);
	}


	Debug::Log("Starting the game loop");
	game_levels[current_level].Activate();

	bool ApplicationRunning = true;
	bool victory_screen = false;

	while (ApplicationRunning)
	{
		timeStep.Start();
		while (timeStep.Running())
		{
			/* Handle input stuff */
			while (window.PollEvents())
			{
				window.EventTick(window.event, &ApplicationRunning);

				if (window.isKeyDown())
				{
					if (!victory_screen)
					{
						switch (window.key_event())
						{
							/* Arrow keys */
							case (Input::KeyCode::ARROW_UP):
								game_levels[current_level].PlayerMove(GameLevel::PlayerMoveDirection::up);
								break;

							case (Input::KeyCode::ARROW_DOWN):
								game_levels[current_level].PlayerMove(GameLevel::PlayerMoveDirection::down);
								break;

							case (Input::KeyCode::ARROW_LEFT):
								game_levels[current_level].PlayerMove(GameLevel::PlayerMoveDirection::left);
								break;

							case (Input::KeyCode::ARROW_RIGHT):
								game_levels[current_level].PlayerMove(GameLevel::PlayerMoveDirection::right);
								break;

							/* WASD */
							case (Input::KeyCode::W):
								game_levels[current_level].PlayerMove(GameLevel::PlayerMoveDirection::up);
								break;

							case (Input::KeyCode::S):
								game_levels[current_level].PlayerMove(GameLevel::PlayerMoveDirection::down);
								break;

							case (Input::KeyCode::A):
								game_levels[current_level].PlayerMove(GameLevel::PlayerMoveDirection::left);
								break;

							case (Input::KeyCode::D):
								game_levels[current_level].PlayerMove(GameLevel::PlayerMoveDirection::right);
								break;

							default:
								break;
						}
					}
				}
			}

			timeStep.Step();
		}

		/* Game logic */
		if (!victory_screen)
		{
			//game_levels[current_level].GuardTick();
			game_levels[current_level].TenSecondTick();

			/* Check if the level was cleared */
			if (game_levels[current_level].level_finished)
			{
				++current_level;

				if (current_level < level_count)
					game_levels[current_level].Activate();
				else
					victory_screen = true;
			}
		}


		window.Clear();
		/* Handle rendering */

		level_background.Render();

		if (!victory_screen)
		{
			game_levels[current_level].LevelGuardLampScene().Render();
			game_levels[current_level].LevelWallScene().Render();
			game_levels[current_level].LevelGuardScene().Render();
			Render::DrawEntity(game_levels[current_level].diamond);
			Render::DrawEntity(game_levels[current_level].player);
		}
		else
		{
			/* Show the victory screen */
			victory_scene.Render();
		}

		/* End of rendering */
		window.Display();

		timeStep.End();
	}

	/* Free the levels */
	for (int i = 0; i < level_count; ++i)
		game_levels[i].Free();

	return 0;
}
