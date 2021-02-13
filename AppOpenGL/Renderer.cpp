#include "Renderer.h"
#include "Skybox.h"

Renderer::Renderer(AppWindow* window)
{
	this->window = window;
	xOffset = 200;
	yOffset = 150;
	RenderWindowWidth = 1280;
	RenderWindowHeight = 720;
	usingSkybox = false;

	projection = glm::perspective(glm::radians(60.0f), (GLfloat)window->getBufferWidth() / window->getBufferHeigt(), 0.1f, 100.0f);

	//ShaderMap = new std::unordered_map<std::string, Shader>();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	pointLightCount = 0; spotLightCount = 0;
}

//Renderer::Renderer(Projection type)
//{
//	switch (type)
//	{
//	case Renderer::Orthographic:
//		printf("Orthographic projection not implemented yet!");
//		return;
//
//	case Renderer::Perspective:
//
//
//		break;
//	}
//}

void Renderer::Render(float dt, unsigned int fbo)
{
	// Get + Handle user input events
	glfwPollEvents();
	camera.keyControl(window->getKeys(), dt);
	camera.mouseControl(window->getXChange(), window->getYchange());

	DirectionalShadowMapPass(&mainLight);
	for (size_t i = 0; i < pointLightCount; i++)
	{
		OmniShadowMapPass(&pointLights[i]);
	}

	for (size_t i = 0; i < spotLightCount; i++)
	{
		OmniShadowMapPass(&spotLights[i]);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glEnable(GL_DEPTH_TEST);

	RenderPass();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplGlfwGL3_NewFrame();
	gui->RenderPlayerWindow(fbo, &RenderWindowWidth, &RenderWindowHeight);
	gui->RenderGui(&mainLight, &RenderWindowWidth, &RenderWindowHeight);
	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}


void Renderer::RenderPass()
{
	glViewport(0, 0, RenderWindowWidth, RenderWindowHeight);//make dynamic
	//Clear window
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(usingSkybox)
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);


	shader->UseShader();//optimize this

	uniformModel = shader->GetModelLocation();
	uniformProjection = shader->GetProjectionLocation();
	uniformView = shader->GetViewLocation();
	uniformEyePosition = shader->GetEyePositionLocation();
	uniformSpecularIntensity = shader->GetSpecularIntensityLocation();
	uniformShininess = shader->GetShininessLocation();


	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	shader->SetDirectionalLight(&mainLight);
	shader->SetPointLights(pointLights, pointLightCount, 3, 0);
	shader->SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
	shader->SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

	mainLight.GetShadowMap()->Read(GL_TEXTURE2);
	shader->SetTexture(1);
	shader->SetDirectionalShadowMap(2);

	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.3f;
	spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

	shader->Validate();

	RenderScene();//s
}

void Renderer::DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader->UseShader();

	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader->GetModelLocation();
	directionalShadowShader->SetDirectionalLightTransform(&light->CalculateLightTransform());

	directionalShadowShader->Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::OmniShadowMapPass(PointLight* light)
{
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	omniShadowShader->UseShader();

	uniformModel = omniShadowShader->GetModelLocation();
	uniformOmniLightPos = omniShadowShader->GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader->GetFarPlaneLocation();

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader->SetLightMatrices(light->CalculateLightTransform());

	omniShadowShader->Validate();

	RenderScene();

//	ImGui_ImplGlfwGL3_NewFrame();
//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//	//GLint drawFboId = 0, readFboId = 0;
////	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
//
//	//ImGui::GetWindowDrawList()->AddImage((void*) drawFboId, ImVec2(ImGui::GetCursorScreenPos()),
//	//	ImVec2(ImGui::GetCursorScreenPos().x + window->getBufferWidth() / 2, ImGui::GetCursorScreenPos().y + window->getBufferHeigt() / 2), ImVec2(0, 1), ImVec2(1, 0));
//	ImGui::Render();
//	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderScene()
{
	glm::mat4 model(1.0f);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	brickTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	MeshList[0]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTexture.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	MeshList[1]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	MeshList[2]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 10.0f));
	model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	xwing.RenderModel();



	model = glm::mat4(1.0f);
	//model = glm::rotate(model, -blackhawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-8.0f, 2.0f, 0.0f));
	model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	blackhawk.RenderModel();
}

void Renderer::SetSkybox(const std::string skyboxLocation)
{
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);
	usingSkybox = true;
}

void Renderer::SetLighting()
{
	mainLight = DirectionalLight(2048, 2048,
		1.0f, 1.0f, 1.0f,
		0.1f, 0.3f,
		0.0f, -15.0f, -10.0f);

	pointLights[0] = PointLight(1024, 1024,
		0.01f, 100.0f,
		0.0f, 0.0f, 1.0f,
		0.5f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	pointLights[1] = PointLight(1024, 1024,
		0.01f, 100.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 1.0f,
		-4.0f, 2.0f, 0.0f,
		0.3f, 0.1f, 0.1f);
	pointLightCount++;


	spotLights[0] = SpotLight(1024, 1024,
		0.01f, 100.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[1] = SpotLight(1024, 1024,
		0.01f, 100.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, -1.5f, 0.0f,
		-100.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
}

void Renderer::SetShaders()
{
	shader = new Shader();
	shader->CreateFromFiles("Shaders/shader_vertex.shader", "Shaders/shader_fragment.shader");

	directionalShadowShader = new Shader();
	directionalShadowShader->CreateFromFiles("Shaders/directional_shadow_map_vertex.shader", "Shaders/directional_shadow_map_fragment.shader");
	omniShadowShader = new Shader();
	omniShadowShader->CreateFromFiles("Shaders/omni_shadow_map_vertex.shader", "Shaders/omni_shadow_map_geom.shader", "Shaders/omni_shadow_map_fragment.shader");
}

void Renderer::SetMaterials()
{
	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);
}

void Renderer::SetImgui(AppWindow* window)
{
	gui = new Gui(window);
}

void Renderer::AddShader(std::string shaderVertLocation, std::string shaderFragLocation)
{
	Shader shader;//find out if create can be optimized
	shader.CreateFromFiles(shaderVertLocation.c_str(), shaderFragLocation.c_str());

	std::string shaderName = shaderVertLocation.substr(shaderVertLocation.rfind("/") + 1);//implement later to del .
	//might want to handle if assigning a shader that already exists
	//ShaderMap->insert_or_assign(shaderName, shader);
}

void Renderer::CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};
	GLfloat vertices[] =
	{//                       u    v      normals
		-1.0f, -1.0f, -0.6f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] =
	{
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] =
	{
		-10.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f, 10.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f, 0.0f, 10.0f, 0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f, 10.0f, 10.0f, 0.0f, -1.0f, 0.0f
	};

	CalcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	MeshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	MeshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	MeshList.push_back(obj3);
}

void Renderer::CreateModels()
{
	xwing = Model();
	xwing.LoadModel("Models/x-wing.obj");

	blackhawk = Model();
	blackhawk.LoadModel("Models/uh60.obj");
}

void Renderer::CalcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices,
	unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

Renderer::~Renderer()
{
	delete shader;
	delete omniShadowShader;
	delete directionalShadowShader;
	delete gui;
}
