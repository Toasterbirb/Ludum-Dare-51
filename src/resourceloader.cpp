#include "ResourceLoader.hpp"

ResourceLoader::ResourceLoader()
{
	Birb::Debug::Log("Loading resources...");
	diamond_texture = Birb::Texture("./res/diamond/diamond.png");

	Birb::Debug::Log("Resources loaded!");
}
