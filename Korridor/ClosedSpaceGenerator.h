#pragma once

class Texture;

typedef struct {
	Texture * texture;
	Texture * lightMapTexture;
	float * triangleArray;
	unsigned int triangleCount;
} T_SPACE_OBJECT;

class ClosedSpaceGenerator {

public :
	//static void generateSpace(float dimension, float unit_distance, T_SPACE_OBJECT ** spaceArray, unsigned int * spaceCount);	
	static void generateSpace(float dimension, float unit_distance, T_SPACE_OBJECT ** spaceArray, unsigned int * spaceCount,
		unsigned int maxTextureWidth, unsigned int textureWidth);
};