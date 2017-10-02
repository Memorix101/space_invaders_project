#pragma once
#include <GLFW/glfw3.h>
#include <SOIL.h>

struct vector2
{
	int x;
	int y;
};

struct rect
{
	int x;
	int y;
	int w;
	int h;
};


void initGL(int screen_w, int screen_h);

void draw(GLuint texture, vector2 vec);

void draw(GLuint texture, vector2 vec, rect resize);

void draw(GLuint texture, vector2 vec, rect resize, rect cut, int tex_size_x, int tex_size_y);