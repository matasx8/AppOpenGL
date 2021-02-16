#include "AppWindow.h"

AppWindow::AppWindow()
{
	width = 0;
	height = 0;

	xChange = 0.0f;
	yChange = 0.0f;

	mHandleMouse = true;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

int AppWindow::Initialise()
{
	//init glfw
	if (!glfwInit())
	{
		printf("GLFW Initialization failed!");
		glfwTerminate();
		return 1;
	}

	//Set up GLFW window props
	//OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//3.3
	//core profile = no backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//forward compat

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	mainWindow = glfwCreateWindow(mode->width, mode->height, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	//Get Buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	//Handle key + Mouse Input
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW init failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	//Setup ViewPort size
	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSetWindowUserPointer(mainWindow, this);//this class
}

void AppWindow::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat AppWindow::getXChange()
{
	//if (mHandleMouse)
	//	return 0.0f;
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat AppWindow::getYchange()
{
	//if (mHandleMouse)
	//	return 0.0f;
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}
void AppWindow::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{

	AppWindow* theWindow = static_cast<AppWindow*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		//glfwSetWindowShouldClose(window, GL_TRUE);
		mHandleMouse = !mHandleMouse;
		if(mHandleMouse)
		glfwSetInputMode(theWindow->mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(theWindow->mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
		}
	}
}

void AppWindow::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	if (mHandleMouse)
		return;

	AppWindow* theWindow = static_cast<AppWindow*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;

	//printf("x:%.6f, y:%.6f\n", theWindow->xChange, theWindow->yChange);
}

AppWindow::~AppWindow()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}

bool AppWindow::mHandleMouse = true;