#include "Gui.h"

Gui::Gui() {};

Gui::Gui(AppWindow* window)
{
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window->mainWindow, false);
	ImGui::StyleColorsDark();
}

void Gui::RenderGui(DirectionalLight* light, int* width, int* height)
{
	ImGui::Begin("Random Settings so far");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat3("Directioal Light", glm::value_ptr(*light->getDirectionArr()), -100.0f, 100.0f);
	ImGui::SliderFloat("Brightness", light->GetIntensity(), 0.0f, 1.0f);
	ImGui::ColorPicker3("Directional Light Color", glm::value_ptr(*light->GetColour()));
	ImGui::Text("width: %i  height %i", *width, *height);
	ImGui::End();
}
static void RetainRatio(ImGuiSizeCallbackData* data) { data->DesiredSize = ImVec2(data->DesiredSize.x, data->DesiredSize.x / ((float) 1295/755)); }

void Gui::RenderPlayerWindow(unsigned int FBO, int* width, int* height)
{
	ImGui::SetNextWindowPos(ImVec2(0,0));
	ImGui::SetNextWindowSize(ImVec2(*width + 15, *height + 35));
	ImGui::SetNextWindowSizeConstraints(ImVec2(1280 + 15, 720 + 35), ImVec2(FLT_MAX, FLT_MAX),
		RetainRatio);
	ImGui::Begin("Player Window");
	ImGui::GetWindowDrawList()->AddImage((void*)FBO, ImVec2(ImGui::GetCursorScreenPos()),
		ImVec2(ImGui::GetCursorScreenPos().x + *width, ImGui::GetCursorScreenPos().y + *height), ImVec2(0, 1), ImVec2(1, 0));
	auto size = ImGui::GetWindowSize();
	*width = size.x - 15;
	*height = size.y - 35;
	ImGui::End();
}

Gui::~Gui()
{
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
}
