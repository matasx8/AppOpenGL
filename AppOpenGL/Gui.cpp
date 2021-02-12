#include "Gui.h"

Gui::Gui() {};

Gui::Gui(AppWindow* window)
{
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window->mainWindow, false);
	ImGui::StyleColorsDark();
}

void Gui::RenderGui(DirectionalLight* light, int* xOffset, int* yOffset)
{
	ImGui_ImplGlfwGL3_NewFrame();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat3("Directioal Light", glm::value_ptr(*light->getDirectionArr()), -100.0f, 100.0f);
	ImGui::SliderFloat("Brightness", light->GetIntensity(), 0.0f, 1.0f);
	ImGui::ColorPicker3("Directional Light Color", glm::value_ptr(*light->GetColour()));
	ImGui::SliderInt("Renderer position x", xOffset, -750, 750);
	ImGui::SliderInt("Renderer position y", yOffset, -750, 750);
	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

Gui::~Gui()
{
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
}
