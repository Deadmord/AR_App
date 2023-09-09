#pragma once

#include "camera.h"
#include <GLFW/glfw3.h>

const glm::vec3 camInitPosition(5.0f, 4.0f, 20.0f);

class InputHandler
{
public:
	InputHandler();
	void processInput(GLFWwindow* window, float deltaTime);

	void ProcessMouseMovement(float xoffset, float yoffset);
	void ProcessMouseScroll(float yoffset);
	void mouseCursorCallback(GLFWwindow* window, double xposIn, double yposIn);
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	float lastX = 400.0f;
	float lastY = 300.0f;
	bool firstMouse = true;
	bool leftMouseButtonPressed = false;
	Camera camera;
private:
};

