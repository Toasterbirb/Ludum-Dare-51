#include "Birb2D.hpp"
#include "Gamelevel.hpp"
#include "ResourceLoader.hpp"

using namespace Birb;

const int level_count = 11;

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
	int tile_size = window.dimensions.y / 18;

	/* The victory screen stuff */
	Scene victory_scene;
	Entity congratulation_text("Victory text", Vector2int(128, 128), EntityComponent::Text("Congratulation!", &resources.big_font, &Colors::White));
	Entity congratulation_text_2("Victory text 2", Vector2int(128, 256), EntityComponent::Text("You managed to yoink all of the diamonds\nand survived all of the challenges\nthat this game threw at you\n\nNow go play some other Ludum Dare games ;)", &resources.small_font, &Colors::White));
	Color pass_timer_color(0xabff93);
	Entity pass_timer_text("Pass timer text", Vector2int(128, 512), EntityComponent::Text("No time for you cheater :/", &resources.big_font, &pass_timer_color));

	victory_scene.AddObject(&congratulation_text);
	victory_scene.AddObject(&congratulation_text_2);
	victory_scene.AddObject(&pass_timer_text);

	/* Timer text */
	Entity ten_second_timer_text("Timer text", Vector2int(window.dimensions.x - 150, window.dimensions.y - 32), EntityComponent::Text("", &resources.small_font, &Colors::White));

	/** Tutorial scenes **/

	/* Level 1 */
	Scene tutorial_scene_1;
	Entity level_1_tutorial_text("Tutorial text level 1", Vector2int(320, 400), EntityComponent::Text("You can move around with the Arrow Keys or WASD", &resources.small_font, &Colors::White));
	tutorial_scene_1.AddObject(&level_1_tutorial_text);

	Entity level_1_tutorial_text2("Tutorial text2 level 1", Vector2int(760, 198), EntityComponent::Text("Your task is to steal\nthese shiny things", &resources.tiny_font, &Colors::White));
	tutorial_scene_1.AddObject(&level_1_tutorial_text2);

	Entity diamond_arrow("Diamond arrow level 1", Rect(935, 207, 64, 22), resources.arrow_texture);
	tutorial_scene_1.AddObject(&diamond_arrow);

	/* Level 2 */
	Scene tutorial_scene_2;
	Entity level_2_tutorial_text("Tutorial text level 2", Vector2int(380, 500), EntityComponent::Text("Avoid the wandering guards!\n\nIf they notice you, you'll have to run\nback to the start...", &resources.small_font, &Colors::White));
	tutorial_scene_2.AddObject(&level_2_tutorial_text);

	/* Level 3 */
	Scene tutorial_scene_3;
	Entity level_3_tutorial_text("Tutorial text level 3", Vector2int(355, (int)ten_second_timer_text.rect.y), EntityComponent::Text("Magical things happen when this clock hits the 10th second ->", &resources.small_font, &Colors::White));
	tutorial_scene_3.AddObject(&level_3_tutorial_text);

	/* Level 4 */
	Scene tutorial_scene_4;
	Entity level_4_tutorial_text("Tutorial text level 4", Vector2int(250, 20), EntityComponent::Text("You can pick up these keys and use them to force gates open.\nA key is consumed when you walk trough a closed gate!", &resources.tiny_font, &Colors::White));
	Entity key_arrow("Key arrow level 4", Rect(180, 60, 64, 22), resources.arrow_texture);
	key_arrow.angle = 150;
	tutorial_scene_4.AddObject(&level_4_tutorial_text);
	tutorial_scene_4.AddObject(&key_arrow);

	/** Key counter **/
	Scene key_counter;
	Entity key_counter_sprite("Key counter sprite", Rect(1000, ten_second_timer_text.rect.y - 6, tile_size, tile_size), resources.key_texture);
	Entity key_counter_text("Key counter text", Vector2int(980, (int)ten_second_timer_text.rect.y + 3), EntityComponent::Text("0", &resources.tiny_font, &Colors::White));

	key_counter.AddObject(&key_counter_sprite);
	key_counter.AddObject(&key_counter_text);


	/* Read all of the levels */
	int current_level = 0;

	/* Custom current level */
	if (argc == 2)
		current_level = std::stoi(argv[1]);


	GameLevel game_levels[level_count];
	for (int i = 0; i < level_count; ++i)
		game_levels[i] = GameLevel("level_" + std::to_string(i + 1), tile_size, resources);

	/* Check if all of the levels loaded up fine */
	for (int i = 0; i < level_count; ++i)
	{
		if (!game_levels[i].LevelLoadSuccessful())
			Debug::Log("Couldn't load level [" + std::to_string(i + 1) + "]", Debug::Type::error);
	}


	Debug::Log("Starting the game loop");
	game_levels[current_level].Activate();

	Timer pass_timer;

	/* Only start the timer if the player starts the game
	 * all the way from the beginning */
	if (current_level == 0)
		pass_timer.Start();


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
			ten_second_timer_text.SetText(game_levels[current_level].timer_text());

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
			game_levels[current_level].LevelGateScene().Render();
			game_levels[current_level].LevelKeyScene().Render();

			/* Tutorial scenes */
			switch (current_level)
			{
				case (0):
					tutorial_scene_1.Render();
					break;

				case (1):
					tutorial_scene_2.Render();
					break;

				case (2):
					tutorial_scene_3.Render();
					break;

				case (3):
					tutorial_scene_4.Render();
					break;

				default:
					break;
			}

			game_levels[current_level].LevelGuardScene().Render();
			Render::DrawEntity(game_levels[current_level].diamond);
			Render::DrawEntity(game_levels[current_level].player);

			/* Draw the timer to the bottom right corner of the screen */
			Render::DrawEntity(ten_second_timer_text);

			/* Draw the key counter whenever the player has any keys */
			if (game_levels[current_level].KeyCount() > 0)
			{
				key_counter_text.SetText(std::to_string(game_levels[current_level].KeyCount()));
				key_counter.Render();
			}
		}
		else
		{
			/* If the pass timer is still running, stop it and set the victory text */
			if (pass_timer.running)
			{
				std::cout << "Timer running..." << std::endl;
				pass_timer.Stop();
				pass_timer_text.SetText("Total time spent: " + pass_timer.DigitalFormat());
			}

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

	resources.Free();

	return 0;
}
