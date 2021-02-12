#pragma once
#include "DirectionalLight.h"
#include "AppWindow.h"

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

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

	void RenderGui(DirectionalLight* light, int* xOffset, int* yOffset);

	~Gui();
};

