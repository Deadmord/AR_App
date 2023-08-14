#pragma once
#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>


class RTCounter
{
public:
	static void updateTimer()
	{
		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrameTime;
		lastFrameTime = currentFrame;
	}

	static void printFPS_Console()
	{
		float currentTime = static_cast<float>(glfwGetTime());
		if (currentTime - lastPrintTime >= 0.25f)
		{
			std::cout << '\r' << std::fixed << std::setprecision(1) << 1.0f / deltaTime << "  ";
			lastPrintTime = currentTime;
		}
	}

	static float getDeltaTime()
	{
		return deltaTime;
	}

private:
	static float deltaTime;	// Time between current frame and last frame
	static float lastFrameTime; // Time of last frame
	static float lastPrintTime; // Time of last print FPS
};

