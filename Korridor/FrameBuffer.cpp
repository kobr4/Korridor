#include "FrameBuffer.h"
#include <stdlib.h>
#include <GL/glew.h>
#include "Texture.h"
#include "Sprite.h"
#include "Shader.h"
#include <stdio.h>


void FrameBuffer::bind(){
	if (this->fb == 0) {
		do_register();
	}

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRb);	

	// check status
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		exit(0);
	}
}

void FrameBuffer::unbind(int screenWidth, int screenHeight){
	glViewport(0, 0, screenWidth, screenHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::do_register(){
		glewInit();
		glGenFramebuffers(1, &this->fb);
		glGenRenderbuffers(1, &this->depthRb);
		glBindRenderbuffer(GL_RENDERBUFFER, this->depthRb);
		glGenTextures(1, &this->renderTex);
		glBindTexture(GL_TEXTURE_2D, this->renderTex);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);

		pixels = (unsigned char *)calloc(sizeof(char),4 * width * height);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		glBindRenderbuffer(GL_RENDERBUFFER, this->depthRb);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, this->width, this->height);	

		// check status
		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			exit(0);
		}
}

Texture * FrameBuffer::getTexture() {
	return new Texture(this->width,this->height,this->renderTex);
}

void FrameBuffer::draw(unsigned int width,unsigned int height) {
	if (this->sprite == NULL) {
		this->sprite = new Sprite(this->getTexture(),(float)(width == 0? this->width : width),(float)(this->height == 0? this->height : height),0,0,1,1);
	}

	this->sprite->draw();
}

void FrameBuffer::blur() {
	if (this->backFb == NULL) {
		this->backFb = new FrameBuffer(this->width,this->height);
	}

	Shader * verticalBlurShader = Shader::createBuiltin(SHADER_BLUR_VERTICAL);
	verticalBlurShader->setProjectionMatrixToOrtho(this->width,this->height);
	verticalBlurShader->setModelViewMatrixToIdentity();
	
	verticalBlurShader->bind_attributes();
	this->backFb->bind();
	this->draw();
	this->backFb->unbind(this->width,this->height);
	verticalBlurShader->unbind();


	Shader * horizontalBlurShader = Shader::createBuiltin(SHADER_BLUR_HORIZONTAL);
	horizontalBlurShader->setProjectionMatrixToOrtho(this->width,this->height);
	horizontalBlurShader->setModelViewMatrixToIdentity();
	
	horizontalBlurShader->bind_attributes();
	this->bind();
	this->backFb->draw();
	this->unbind(this->width,this->height);
	horizontalBlurShader->unbind();
}
