#pragma once
#include <stdio.h>
#include <unordered_map>

#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "AppWindow.h"
#include "Material.h"
#include "Model.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Skybox.h"
#include "Shader.h"
#include "Gui.h"

class Renderer
{
public:
	enum Projection
	{
		Orthographic,
		Perspective
	};

	Renderer(AppWindow* window);

	void Render(float dt);
	void Render2(float dt);
	//should check for somekind of flag, if not all flags set then fail

	//Will set up the Skybox
	//TODO:Make this so other skyboxes work too
	void SetSkybox(std::string skyboxLocation);
	//mandatory, set up lighting
	void SetLighting();
	void SetShaders();
	void SetMaterials();
	void SetImgui(AppWindow* window);
	void SetFBO();

	//will add shader to shadermap
	void AddShader(std::string shaderVertLocation, std::string shaderFragLocation);

	//temporary
	void CreateObjects();
	void CreateModels();
	void CalcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices,
		unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset);

	int* GetXOffset() { return &xOffset; }
	int* GetYOffset() { return &yOffset; }
	int* GetRWW() { return &RenderWindowWidth; }
	int* GETRWH() { return &RenderWindowHeight; }

	~Renderer();

private:
	void RenderPass();
	void DirectionalShadowMapPass(DirectionalLight* light);
	void OmniShadowMapPass(PointLight* light);
	//render meshes
	void RenderScene();


	AppWindow* window;
	int xOffset;
	int yOffset;
	int RenderWindowWidth;
	int RenderWindowHeight;

	bool usingSkybox;
	Skybox skybox;

	glm::mat4 projection;

	std::vector<Mesh*> MeshList;

	//consider using unique pointers so I wouldn't need to loop over the map to delete
	std::unordered_map<std::string, Shader*>* ShaderMap;

	DirectionalLight mainLight;
	PointLight pointLights[MAX_POINT_LIGHTS];
	SpotLight spotLights[MAX_SPOT_LIGHTS];
	int pointLightCount;
	int spotLightCount;

	//need to come up with a better solution
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0,
		uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0, uniformDirectionalLightTransform = 0,
		uniformOmniLightPos = 0, uniformFarPlane = 0;

	unsigned int framebuffer;

	Texture brickTexture;
	Texture dirtTexture;
	Texture plainTexture;

	Material shinyMaterial;
	Material dullMaterial;

	Model xwing;
	Model blackhawk;

	Camera camera;

	Gui* gui;
};

