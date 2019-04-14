#include "Texture2D.h"

Texture2D::Texture2D()
{

}

Texture2D::~Texture2D()
{
	texture.create(1, 1);
	sprite.setTexture(texture);
}

void Texture2D::Load(std::string file)
{
	if (!texture.loadFromFile(file))
	{
		// error...
		std::cout << "Couldn't load image file: " << file << std::endl;
	}
	//texture.setSmooth(true);
	sprite.setTexture(texture);
}