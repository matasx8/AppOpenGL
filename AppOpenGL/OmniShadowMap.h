#pragma once
#include "ShadowMap.h"

class OmniShadowMap : public ShadowMap
{
public:
	OmniShadowMap();

	virtual bool Init(unsigned int width, unsigned int height);

	virtual void Write();

	virtual void Read(GLenum textureUnit);

	~OmniShadowMap();
};

