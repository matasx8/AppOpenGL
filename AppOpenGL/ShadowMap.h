#pragma once

#include <stdio.h>

#include <GL/glew.h>

class ShadowMap
{
public:
	ShadowMap();

	virtual bool Init(unsigned int width, unsigned int height);

	virtual void Write();

	virtual void Read(GLenum textureUnit);

	GLuint GetShadowWidth() { return shadowWidth; }
	GLuint GetShadowHeight() { return shadowHeight; }

	GLuint GetFBO() { return FBO; };

	~ShadowMap();

protected:
	GLuint FBO, shadowMap;
	GLuint shadowWidth, shadowHeight;
};

