#include "font.h"
#include <string.h>

void drawFontD(GLuint texture, struct vector2 vec, struct rect resize, struct rect cut, int tex_size_x, int tex_size_y)
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

void createFont(GLuint fontTexture, int fontSize, struct vector2 pos, const char* text)
{
	struct rect fSize;
	fSize.w = fontSize;
	fSize.h = fontSize;

	struct rect font_rect[256];
	struct vector2 glyph_pos[256];

	int i;
	for (i = 0; i < strlen(text); i++) {

		//printf("%c\n", text[i]);

		glyph_pos[i].x = pos.x + (fontSize*0.75f * i);
		glyph_pos[i].y = pos.y;

		font_rect[i].w = 32;
		font_rect[i].h = 32;

		switch (text[i])
		{
		case ' ':
			font_rect[i].x = 0;
			font_rect[i].y = 0;
			break;
		case 'A':
			font_rect[i].x = 32;
			font_rect[i].y = 64;
			break;
		case 'B':
			font_rect[i].x = 64;
			font_rect[i].y = 64;
			break;
		case 'C':
			font_rect[i].x = 96;
			font_rect[i].y = 64;
			break;
		case 'D':
			font_rect[i].x = 128;
			font_rect[i].y = 64;
			break;
		case 'E':
			font_rect[i].x = 160;
			font_rect[i].y = 64;
			break;
		case 'F':
			font_rect[i].x =192;
			font_rect[i].y = 64;
			break;
		case 'G':
			font_rect[i].x = 224;
			font_rect[i].y = 64;
			break;
		case 'H':
			font_rect[i].x = 256;
			font_rect[i].y = 64;
			break;
		case 'I':
			font_rect[i].x = 288;
			font_rect[i].y = 64;
			break;
		case 'J':
			font_rect[i].x = 320;
			font_rect[i].y = 64;
			break;
		case 'K':
			font_rect[i].x = 352;
			font_rect[i].y = 64;
			break;
		case 'L':
			font_rect[i].x = 384;
			font_rect[i].y = 64;
			break;
		case 'M':
			font_rect[i].x = 416;
			font_rect[i].y = 64;
			break;
		case 'N':
			font_rect[i].x = 448;
			font_rect[i].y = 64;
			break;
		case 'O':
			font_rect[i].x = 480;
			font_rect[i].y = 64;
			break;
		case 'P':
			font_rect[i].x = 0;
			font_rect[i].y = 96;
			break;
		case 'Q':
			font_rect[i].x = 32;
			font_rect[i].y = 96;
			break;
		case 'R':
			font_rect[i].x = 64;
			font_rect[i].y = 96;
			break;
		case 'S':
			font_rect[i].x = 96;
			font_rect[i].y = 96;
			break;
		case 'T':
			font_rect[i].x = 128;
			font_rect[i].y = 96;
			break;
		case 'U':
			font_rect[i].x = 160;
			font_rect[i].y = 96;
			break;
		case 'V':
			font_rect[i].x = 192;
			font_rect[i].y = 96;
			break;
		case 'W':
			font_rect[i].x = 224;
			font_rect[i].y = 96;
			break;
		case 'X':
			font_rect[i].x = 256;
			font_rect[i].y = 96;
			break;
		case 'Y':
			font_rect[i].x = 288;
			font_rect[i].y = 96;
			break;
		case 'Z':
			font_rect[i].x = 320;
			font_rect[i].y = 96;
			break;
		case '(':
			font_rect[i].x = 352;
			font_rect[i].y = 96;
			break;
		case '/':
			font_rect[i].x = 384;
			font_rect[i].y = 96;
			break;
		case ')':
			font_rect[i].x = 416;
			font_rect[i].y = 96;
			break;
		case 'a':

			break;
		case 'b':

			break;
		case 'c':

			break;
		case 'd':

			break;
		case 'e':

			break;
		case 'f':

			break;
		case 'g':

			break;
		case 'h':

			break;
		case 'i':

			break;
		case 'j':

			break;
		case 'k':

			break;
		case 'l':

			break;
		case 'm':

			break;
		case 'n':

			break;
		case 'o':

			break;
		case 'p':

			break;
		case 'q':

			break;
		case 'r':

			break;
		case 's':

			break;
		case 't':

			break;
		case 'u':

			break;
		case 'v':

			break;
		case 'w':

			break;
		case 'x':

			break;
		case 'y':

			break;
		case 'z':

			break;
		case '1':
			font_rect[i].x = 32;
			font_rect[i].y = 32;
			break;
		case '2':
			font_rect[i].x = 64;
			font_rect[i].y = 32;
			break;
		case '3':
			font_rect[i].x = 96;
			font_rect[i].y = 32;
			break;
		case '4':
			font_rect[i].x = 128;
			font_rect[i].y = 32;
			break;
		case '5':
			font_rect[i].x = 160;
			font_rect[i].y = 32;
			break;
		case '6':
			font_rect[i].x = 192;
			font_rect[i].y = 32;
			break;
		case '7':
			font_rect[i].x = 224;
			font_rect[i].y = 32;
			break;
		case '8':
			font_rect[i].x = 256;
			font_rect[i].y = 32;
			break;
		case '9':
			font_rect[i].x = 288;
			font_rect[i].y = 32;
			break;
		case '0':
			font_rect[i].x = 0;
			font_rect[i].y = 32;
			break;
		case '.':

			break;
		case ',':

			break;
		case ':':
			font_rect[i].x = 320;
			font_rect[i].y = 32;
			break;
		case '!':
			font_rect[i].x = 32;
			font_rect[i].y = 0;
			break;
		case '%':

			break;
		case '$':

			break;
		case '&':

			break;
		case '=':

			break;
		case '+':

			break;
		case '-':

			break;
		case '#':

			break;
		}

		drawFontD(fontTexture, glyph_pos[i], fSize, font_rect[i], 512, 512);
	}
}
