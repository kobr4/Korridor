#pragma once

typedef struct {
	float position[3];
	float color[3];
} T_TextureLightSource;

typedef struct {
	float p1[3];
	float p2[3];
	float p3[3];
	float p4[3];
} T_TextureQuad;

class Texture;
class TextureGenerator {
public :
	static Texture * generateTileTexture(unsigned int width,unsigned int height, unsigned int hoffset,unsigned int hspacing,unsigned int vspacing,
		unsigned int border,unsigned char r, unsigned char g, unsigned char b,unsigned char jr, unsigned char jg, unsigned char jb);
	static Texture * generateWhiteTexture();
	static Texture * generateLightmapTexture(unsigned int width,unsigned int height,T_TextureLightSource * source, T_TextureQuad * quad);
	static void writeTGA(char * filename, unsigned char * map, int width, int height, bool invertRB);
};