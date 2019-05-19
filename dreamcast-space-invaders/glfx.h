#include <kos.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

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

void draw(GLuint texture, struct vector2 vec, int w, int h);

void draw_sprite(GLuint texture, struct vector2 vec, struct rect resize);

void draw_spritesheet(GLuint texture, struct vector2 vec, struct rect resize, struct rect cut, int tex_size_x, int tex_size_y);