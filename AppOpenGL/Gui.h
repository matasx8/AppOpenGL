#pragma once
#include "DirectionalLight.h"
#include "AppWindow.h"

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#define IM_MAX(_A,_B)       (((_A) >= (_B)) ? (_A) : (_B))

#include <glm/gtc/type_ptr.hpp>

class Gui
{
public:
	enum Layout
	{
		Basic,
		Light
	};
	Gui();

	Gui(AppWindow* window);

	void RenderGui(DirectionalLight* light, int* width, int* height);
	void RenderPlayerWindow(unsigned int FBO, int* width, int* height);

	~Gui();
};

