#pragma once

#include<GLFW/glfw3.h>
#include<iostream>
#include "Structs.h"

class WindowManager
{
private:

	GLFWwindow* window;
	int width;
	int height;

	static bool glfwInitialized;
	static int windowCount;

public:
	WindowManager(const int& width, const int& height, unsigned int WinPosX, unsigned int WinPosY, const char* title, GLFWwindow* share = nullptr);
	static void FramebufferSizeCallback(GLFWwindow* window, int new_width, int new_height);
	void MakeContextCurrent();
	bool Close() const;
	void SwapBuffers();
	void PollEvents();
	int GetWidth() const;
	int GetHeight() const;
	GLFWwindow* GetWindow() const;

	~WindowManager();
};