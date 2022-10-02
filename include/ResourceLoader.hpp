#pragma once
#include "Birb2D.hpp"

class ResourceLoader
{
public:
	ResourceLoader();
	void Free();

	/* Resources */
	Birb::Texture diamond_texture;
	Birb::Texture arrow_texture;
	Birb::Texture key_texture;
	Birb::Texture player_texture;
	Birb::Texture guard_texture;

	Birb::Texture open_gate;
	Birb::Texture closed_gate;

	Birb::Font tiny_font;
	Birb::Font small_font;
	Birb::Font big_font;

	Birb::Audio::SoundFile gate_open_sound;
	Birb::Audio::SoundFile key_pickup_sound;
	Birb::Audio::SoundFile level_clear_sound;

private:
};
