#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include "ShadowMap.h"

class Light
{
public:
	Light();
	Light(GLuint shadowWidth, GLuint shadowHeight,
		GLfloat red, GLfloat green, GLfloat blue, 
		GLfloat aIntensity, GLfloat dIntensity);

	float* GetIntensity() { return &diffuseIntensity; };
	glm::vec3* GetColour() { return &colour; }

	ShadowMap* GetShadowMap() { return shadowMap; }
	~Light();
protected:
	glm::vec3 colour;
	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;

	glm::mat4 lightProj;

	ShadowMap* shadowMap;
};

