#include "glfx.h"
#include <cstdio>

void initGL(int screen_w, int screen_h)
{
	// init OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screen_w, screen_h, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
}


void draw(GLuint texture, vector2 vec)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int w, h;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

	glPushMatrix();

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(vec.x, vec.y); //100 100 // 1 - lead vec pos

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(vec.x + w, vec.y); //300 100 // 2

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(vec.x + w, vec.y + h); //300 300 // 3

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(vec.x, vec.y + h); //100 300  // 4

	glEnd();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void draw(GLuint texture, vector2 vec, rect resize)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glPushMatrix();

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(vec.x, vec.y); //100 100 // 1 - lead vec pos

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(vec.x + resize.w, vec.y); //300 100 // 2

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(vec.x + resize.w, vec.y + resize.h); //300 300 // 3

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(vec.x, vec.y + resize.h); //100 300  // 4

	glEnd();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void draw(GLuint texture, vector2 vec, rect resize, rect cut, int tex_size_x, int tex_size_y)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glPushMatrix();

	float u = (((float)cut.w / (float)tex_size_x) * 100.f) / 100.f;
	//printf("%f\n", u / 100.f);

	float ux = (((float)cut.x / (float)tex_size_x) * 100.f) / 100.f;
	//printf("%f\n", ux / 100.f);

	float v = (float)cut.h / (float)tex_size_y;
	//printf("%f\n", v);

	float uy = (float)cut.y / (float)tex_size_y;
	//printf("%f\n", uy);

	glBegin(GL_QUADS);

	glTexCoord2f(ux, 0.f + uy);
	glVertex2f(vec.x, vec.y); //100 100 // 1 - lead vec pos

	glTexCoord2f(u + ux, 0.f + uy);
	glVertex2f(vec.x + resize.w, vec.y); //300 100 // 2

	glTexCoord2f(u + ux, v + uy);
	glVertex2f(vec.x + resize.w, vec.y + resize.h); //300 300 // 3

	glTexCoord2f(ux, v + uy);
	glVertex2f(vec.x, vec.y + resize.h); //100 300  // 4

	glEnd();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}