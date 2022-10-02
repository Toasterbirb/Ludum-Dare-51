#include "ResourceLoader.hpp"

ResourceLoader::ResourceLoader()
{
	Birb::Debug::Log("Loading resources...");

	/* Textures */
	diamond_texture = Birb::Texture("./res/diamond/diamond.png");
	arrow_texture 	= Birb::Texture("./res/arrow.png");
	key_texture 	= Birb::Texture("./res/key.png");
	player_texture 	= Birb::Texture("./res/player.png");
	guard_texture 	= Birb::Texture("./res/guard.png");

	open_gate 	= Birb::Texture("./res/gate_open.png");
	closed_gate = Birb::Texture("./res/gate_closed.png");

	/* Fonts */
	tiny_font.LoadFont("./res/fonts/fortzilla/Fortzilla.ttf", 16);
	small_font.LoadFont("./res/fonts/fortzilla/Fortzilla.ttf", 24);
	big_font.LoadFont("./res/fonts/fortzilla/Fortzilla.ttf", 42);

	/* Audio files */
	gate_open_sound = Birb::Audio::SoundFile("./res/sfx/gate_open.wav");
	key_pickup_sound = Birb::Audio::SoundFile("./res/sfx/key_pickup.wav");
	level_clear_sound = Birb::Audio::SoundFile("./res/sfx/level_clear.wav");
	magic_sound = Birb::Audio::SoundFile("./res/sfx/magic.wav");


	Birb::Debug::Log("Resources loaded!");
}

void ResourceLoader::Free()
{
	gate_open_sound.free();
	key_pickup_sound.free();
	level_clear_sound.free();
}
