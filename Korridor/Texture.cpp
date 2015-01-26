#include "Texture.h"
#include <GL/glew.h>

void Texture::bind() {
	if (this->textureName == 0) {
		do_register();
	}
	glBindTexture(GL_TEXTURE_2D, this->textureName); 
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0); 
}

void Texture::update() {
	if (this->textureName != 0) {
		glBindTexture(GL_TEXTURE_2D, this->textureName); 
				
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
	}
}

void Texture::do_register() {
	glGenTextures(1, &this->textureName); 
	this->update();
}

void Texture::packTexture(Texture * texture,int top_x,int top_y) {
	for (int i = 0;i < texture->width;i++) { 
		for (int j = 0;j < texture->height;j++) {
			int x = i + top_x;
			int y = j + top_y;
			this->pixels[(x+ y*this->width)*4] = texture->pixels[(i+j*texture->width)*4];
			this->pixels[(x+ y*this->width)*4+1] = texture->pixels[(i+j*texture->width)*4+1];
			this->pixels[(x+ y*this->width)*4+2] = texture->pixels[(i+j*texture->width)*4+2];
			this->pixels[(x+ y*this->width)*4+3] = texture->pixels[(i+j*texture->width)*4+3];
		}
	}
}