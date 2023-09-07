#pragma once
#include "camera.h"
//#include "shader.h"
#include <iostream>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // For glm::value_ptr и glm::to_string
//#include "stb_image.h"
//#include "geometryObjects.h"
//#include "geometryData.h"
//#include "aruco/ArucoProcessor.h"
#include "RTCounter.h"
#include "config.h"

class GWindow
{
public:
	GWindow(unsigned int WinID, unsigned int WinWidth, unsigned int WinHeight, unsigned int WinPosX, unsigned int WinPosY, const std::string& name, GLFWmonitor* monitor, glm::vec4 bgColor = BG_CLR);
	~GWindow(){}
	operator GLFWwindow* () const
	{
		return window;
	}

	// Generates OpenGL vertex array and buffer objects arrays with the specified size.
	void addGeometryBuffers(GLsizei size);

	void addGLObject(вся хурма)
	{
		glfwMakeContextCurrent(window);
	}

	void renderFrame(float deltaTime);

	void drowObject(GLsizei objIndex, glm::mat4& viewMat, glm::mat4& projectionMat, bool background = false);

private:
	void showInFrame(const cv::Mat& frame, cv::Size WindSize, cv::Size frameSize, float FPS, std::initializer_list<float> dTimes);

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

public:												//remove it or make private
	// camera
	Camera camera;
	float lastX = 400.0f;
	float lastY = 300.0f;
	bool firstMouse = true;
	bool leftMouseButtonPressed = false;

private:
	GLuint wndID = 0;
	GLuint WinWidth = 800; 
	GLuint WinHeight = 600;
	glm::vec4 bgColor;
	GLsizei objectListSize = 0;
};

