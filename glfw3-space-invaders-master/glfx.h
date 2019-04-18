#pragma once
#include <GL/glew.h>
//#include <GLFW/glfw3.h>

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

struct color
{
	int r;
	int g;
	int b;
};

void initGL(int screen_w, int screen_h);
GLuint load_texture(const char* filename);
unsigned char* load_texture_raw(const char* filename);
void draw(GLuint texture, vector2 vec);
void draw(GLuint texture, vector2 vec, rect resize);
void draw(GLuint texture, vector2 vec, rect resize, rect cut, int tex_size_x, int tex_size_y);