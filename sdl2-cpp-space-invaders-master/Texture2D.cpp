#include "Texture2D.h"


Texture2D::Texture2D()
{
}

Texture2D::~Texture2D()
{
}

void Texture2D::Load(std::string file)
{
	sprite = IMG_Load(file.c_str());
}