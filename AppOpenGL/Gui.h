#pragma once
#include "DirectionalLight.h"
#include "AppWindow.h"

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

class Gui
{
public:
	enum Layout
	{
		Basic,
		Light
	};

	Gui(AppWindow* window);

	void RenderGui(DirectionalLight* light);

	~Gui();
};

