#include "glfx.h"
#include <cstdio>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void initGL(int screen_w, int screen_h)
{
	// init OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screen_w, screen_h, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
}

GLuint load_texture(const char* filename)
{
	int width, height, nrChannels;
	unsigned char* texture_data = stbi_load(filename, &width, &height, &nrChannels, 4);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (texture_data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Failed to load texture\n");
	}

	stbi_image_free(texture_data);
	return texture;
}

unsigned char* load_texture_raw(const char* filename)
{
	int width, height, nrChannels;
	//stbi_image_free(texture_data);
	return stbi_load(filename, &width, &height, &nrChannels, 4);
}

GLFWimage load_texture_to_GLFWimage(const char* filename)
{
	int width, height, nrChannels;
	GLFWimage image;
	image.pixels = stbi_load("rd/icon.png", &image.width, &image.height, 0, 4); //rgba channels 
	return image;
}

void texture_free(unsigned char* texture)
{
	stbi_image_free(texture);
}

void draw(GLuint texture, vector2 vec)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);

	int w, h;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
	//glClear(GL_COLOR_BUFFER_BIT);

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