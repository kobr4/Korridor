#include "Texture.h"
#include "TextureGenerator.h"
#include <stdlib.h>
#include <stdio.h>
#include <glm\glm.hpp>

Texture * g_white_texture = NULL;
Texture * TextureGenerator::generateTileTexture(unsigned int width,unsigned int height, unsigned int hoffset,unsigned int hspacing,unsigned int vspacing,unsigned int border
												,unsigned char r, unsigned char g, unsigned char b,unsigned char jr, unsigned char jg, unsigned char jb ) {
	unsigned char * texture_data = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 4);
	unsigned int offset;
	for (int i = 0;i < height;i++) {
		for (int j = 0;j < width;j++) {
		
			if ((j/hspacing) & 1) {
				offset = hoffset;
			} else {
				offset = 0;
			}
			if ( ((i+offset)%vspacing < border)||(j%hspacing<border)) {
				texture_data[(i*width+j)*4 + 0] = jr;
				texture_data[(i*width+j)*4 + 1] = jg;
				texture_data[(i*width+j)*4 + 2] = jb;
				texture_data[(i*width+j)*4 + 3] = 255;
			} else {
				unsigned char roffset = 0;
				unsigned char goffset = 0;
				unsigned char boffset = 0;

				if (hoffset) {
					unsigned int irow = (i+offset) / vspacing;
					unsigned int jrow = j / hspacing;
					srand (irow * jrow);
					roffset = rand()%10;
					goffset = rand()%10;
					boffset = rand()%10;
				}
				texture_data[(i*width+j)*4 + 0] = r + roffset;
				texture_data[(i*width+j)*4 + 1] = g + goffset;
				texture_data[(i*width+j)*4 + 2] = b + boffset;
				texture_data[(i*width+j)*4 + 3] = 255;			
			}	
		}
	}
	
	//TextureGenerator::writeTGA("output.tga",texture_data,width,height,true);
	return new Texture(width,height,(unsigned char*)texture_data);
}

Texture * TextureGenerator::generateWhiteTexture() {
	if (g_white_texture == NULL) {
		unsigned char * pixels = (unsigned char*)malloc(sizeof(unsigned char)*4*4);
		for (int i = 0;i < 16;i++) pixels[i] = 255;
		g_white_texture = new Texture(2,2,pixels);
	}
	return g_white_texture;
}

Texture * TextureGenerator::generateLightmapTexture(unsigned int width,unsigned int height,T_TextureLightSource * source, T_TextureQuad * quad) {
	glm::vec3 x_increment;
	glm::vec3 y_increment;
	glm::vec3 p1 = glm::vec3(quad->p1[0],quad->p1[1],quad->p1[2]);
	glm::vec3 p2 = glm::vec3(quad->p2[0],quad->p2[1],quad->p2[2]);
	glm::vec3 p3 = glm::vec3(quad->p3[0],quad->p3[1],quad->p3[2]);
	glm::vec3 p4 = glm::vec3(quad->p4[0],quad->p4[1],quad->p4[2]);
	glm::vec3 normal = glm::cross(p2-p1,p4-p1);
	glm::vec3 light_position = glm::vec3(source->position[0],source->position[1],source->position[2]);
	unsigned char * texture_data = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 4);	
	
	x_increment = (p2 - p1) / (float)width;
	y_increment = (p4 - p1) / (float)width;
	
	for (int i = 0;i < height;i++) {
		for (int j = 0;j < width;j++) {
			glm::vec3 current = p1 + (float)i*y_increment + (float)j*x_increment;
			glm::vec3 lightdir = light_position - current;
			normal = glm::normalize(normal);
			lightdir = glm::normalize(lightdir);
			float dot = glm::dot(normal,lightdir);
			if (dot > 0.0f) {
				texture_data[(i*width+j)*4 + 0] = (dot * source->color[0]*255 + 30) > 255 ? 255 : dot * source->color[0]*255 + 30;
				texture_data[(i*width+j)*4 + 1] = (dot * source->color[1]*255 + 30) > 255 ? 255 : dot * source->color[1]*255 + 30;
				texture_data[(i*width+j)*4 + 2] = (dot * source->color[2]*255 + 30) > 255 ? 255 : dot * source->color[2]*255 + 30;
				texture_data[(i*width+j)*4 + 3] = 255;
			} else {
				texture_data[(i*width+j)*4 + 0] = 30;
				texture_data[(i*width+j)*4 + 1] = 30;
				texture_data[(i*width+j)*4 + 2] = 30;
				texture_data[(i*width+j)*4 + 3] = 255;
			}
		}
	}

	return new Texture(width,height,(unsigned char*)texture_data);
}

typedef struct {
	unsigned char id_length;
	unsigned char color_map_type;
	unsigned char image_type;
	unsigned char color_map_spec[5];
	unsigned char image_spec[10];
} t_tgaheader;

void TextureGenerator::writeTGA(char * filename, unsigned char * map, int width, int height, bool invertRB) {
	FILE * pFile;
	pFile = fopen (filename , "wb");
	t_tgaheader header;

	header.id_length = 0;
	header.color_map_type = 0;
	header.image_type = 2;
	header.color_map_spec[0] = 0;
	header.color_map_spec[1] = 0;
	header.color_map_spec[2] = 0;
	header.color_map_spec[3] = 0;
	header.color_map_spec[4] = 0;	
	header.image_spec[0] = 0;
	header.image_spec[1] = 0;
	header.image_spec[2] = 0;
	header.image_spec[3] = 0;
	header.image_spec[4] = width & 0xff;
	header.image_spec[5] = (width & 0xff00) >> 8;
	header.image_spec[6] = height & 0xff;
	header.image_spec[7] = (height & 0xff00) >> 8;
	header.image_spec[8] = 32;
	header.image_spec[9] = 0;

	fwrite(&header,sizeof(t_tgaheader),1,pFile);
	
	if (invertRB) {
		unsigned char map4[4];
		for (int i = 0;i < width*height;i++) {
			map4[0] = map[i * 4 + 2];
			map4[1] = map[i * 4 + 1];
			map4[2] = map[i * 4 + 0];
			map4[3] = map[i * 4 + 3];
			fwrite(map4,sizeof(unsigned char)*4,1,pFile);
		}
	} else {
		fwrite(map,sizeof(unsigned char)*width*height*4,1,pFile);
	}
	fclose(pFile);
}