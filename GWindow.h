#pragma once
#include "camera.h"
#include "GLObject.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // For glm::value_ptr � glm::to_string

#include "WindowMenu.h"
#include "RTCounter.h"
#include "ScreanShot.h"
#include "config.h"

class GWindow
{
public:
	GWindow(unsigned int WinID, unsigned int WinWidth, unsigned int WinHeight, unsigned int WinPosX, unsigned int WinPosY, const std::string& name, GLFWmonitor* monitor, glm::vec4 bgColor = BG_CLR);
	~GWindow();

	void Close();

	operator GLFWwindow* () const
	{
		return window;
	}

	void addGLObject(const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState, Shader* shaderProgPtr,
		const std::shared_ptr<texture> texture, bool linePolygonMode = false);

	void renderFrame(float deltaTime);

private:
	void showInFrame(cv::Mat& frame, cv::Size WindSize, cv::Size frameSize, float renderFPS, float cameraFPS, float markerFPS, std::initializer_list<float> dTimes);

	static void framebufferSizeCallbackWrapper(GLFWwindow* window, int width, int height);
	static void mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods);
	static void mouseCursorCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
	static void scrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset);

	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void mouseCursorCallback(GLFWwindow* window, double xposIn, double yposIn);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window, float deltaTime);	//remove it or make private

private:
	GLFWwindow* window;

	// camera
	Camera camera;
	float lastX = 400.0f;
	float lastY = 300.0f;
	bool firstMouse = true;
	bool leftMouseButtonPressed = false;

	GLuint wndID = 0;
	GLuint WinWidth = 800; 
	GLuint WinHeight = 600;
	glm::vec4 bgColor;

	WindowMenu windowMenu;

	std::vector<std::shared_ptr<GLObject>> glObjects;
	ArucoThreadWrapper arucoThreadWrapper;

	std::mutex frameMutex;
};

