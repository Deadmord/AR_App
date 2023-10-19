#pragma once
#include "camera.h"
//#include "shader.h"
#include "GLObject.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // For glm::value_ptr è glm::to_string
//#include "stb_image.h"
//#include "geometryObjects.h"
//#include "geometryData.h"
//#include "aruco/ArucoProcessor.h"
#include "ArucoThreadWrapper.h"

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

	void addGLObject(const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState, Shader* shaderProgPtr,
		const std::string& texturePath, GLenum internalformat, GLenum format, bool linePolygonMode = false, bool rotate = false, bool isBackground = false, bool showOnMarker = false, std::shared_ptr<std::vector<int>> markerIds = nullptr, std::string cameraParams = nullptr);
	void addGLObject(const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState, Shader* shaderProgPtr,
		const std::shared_ptr<AcquisitionWorker> workerPtr, GLenum internalformat, GLenum format, bool linePolygonMode = false, bool rotate = false, bool isBackground = false, bool showOnMarker = false, std::shared_ptr<std::vector<int>> markerIds = nullptr, std::string cameraParams = nullptr);

	void renderFrame(float deltaTime);

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
	void setupTextureBasedOnExtension(GLObject& obj, const std::string& ext, const std::string& path, GLenum internalformat, GLenum format, bool rotate, bool isBackground, bool showOnMarker, std::shared_ptr<std::vector<int>> markerIds, std::string& cameraParams);
	void throwError(const std::string& message, const std::string& detail);
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

	std::vector<GLObject> glObjects;
	ArucoThreadWrapper arucoThreadWrapper;
};

