#pragma once

#include "TextureManager.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> 

#include "InputHandler.h"
#include "RTCounter.h"
#include "FrameAnalyzer.h"

const glm::vec4 BG_CLR(0.2f, 0.3f, 0.3f, 1.0f);		//default bg color

class GWindowCore
{
public:
	GWindowCore(unsigned int WinID, const unsigned int& width, const unsigned int& height, unsigned int WinPosX, unsigned int WinPosY, const std::string& name, GLFWmonitor* monitor, glm::vec4 bgColor_);
	~GWindowCore();

	operator GLFWwindow* () const
	{
		return window;
	}

	void addGeometryBuffers(GLsizei size);
	void setupGeometryObject(GLuint objIndex, const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState);
	void setupShaderProgram(GLuint index, Shader* shaderProgPtr);
	void renderFrame(float deltaTime);
	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void framebufferSizeCallbackWrapper(GLFWwindow* window, int width, int height);
	static void mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods);
	static void mouseCursorCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
	static void scrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);


	TextureManager textureManager;
	InputHandler inputHandler;
	FrameAnalyzer frameAnalyzer;
private:	
	GLuint wndID = 0;
	GLuint WinWidth = 800;
	GLuint WinHeight = 600;
	GLFWwindow* window;
	glm::vec4 bgColor;
};

