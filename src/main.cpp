#include "Birb2D.hpp"
#include "Gamelevel.hpp"
#include "ResourceLoader.hpp"

using namespace Birb;

int main(int argc, char** argv)
{
	Debug::Log("Creating the window");
	Window window("LD51: Diamond heist", Vector2int(1280, 720), 75, false);
	TimeStep timeStep;
	timeStep.Init(&window);

	Scene level_background;
	Rect background_rect(0, 0, window.dimensions.x, window.dimensions.y);
	background_rect.color = 0x1a1a1a;
	background_rect.renderingPriority = -1;
	level_background.AddObject(&background_rect);

	ResourceLoader resources;

	/* Read a level */
	GameLevel test_level("test_level", 40, resources);

	Debug::Log("Starting the game loop");
	bool ApplicationRunning = true;
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
					switch (window.key_event())
					{
						case (Input::KeyCode::ARROW_UP):
							test_level.PlayerMove(GameLevel::PlayerMoveDirection::up);
							break;

						case (Input::KeyCode::ARROW_DOWN):
							test_level.PlayerMove(GameLevel::PlayerMoveDirection::down);
							break;

						case (Input::KeyCode::ARROW_LEFT):
							test_level.PlayerMove(GameLevel::PlayerMoveDirection::left);
							break;

						case (Input::KeyCode::ARROW_RIGHT):
							test_level.PlayerMove(GameLevel::PlayerMoveDirection::right);
							break;

						default:
							break;
					}
				}
			}

			timeStep.Step();
		}

		window.Clear();
		/* Handle rendering */

		level_background.Render();
		test_level.LevelWallScene().Render();
		Render::DrawEntity(test_level.diamond);
		Render::DrawEntity(test_level.player);

		/* End of rendering */
		window.Display();

		timeStep.End();
	}

	return 0;
}
