#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "AppWindow.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Gui.h"
#include "Model.h"
#include "Skybox.h"
#include "Renderer.h"


AppWindow mainWindow;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;


int main()
{
	mainWindow = AppWindow();
	mainWindow.Initialise();

	Renderer* renderer = new Renderer(&mainWindow);
	renderer->SetMaterials();
	renderer->CreateObjects();
	renderer->CreateModels();
	renderer->SetShaders();
	renderer->SetLighting();
	renderer->SetSkybox("temp");
	renderer->SetImgui(&mainWindow);


	//Gui gui = Gui(&mainWindow);
	//Loop untill window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		renderer->Render(deltaTime);

	//	gui.RenderGui(&mainLight);

		mainWindow.swapBuffers();
	}
	glfwTerminate();

	return 0;
}