#pragma once
#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>
#include <vector>

struct FrameTiming
{
	float startTime;
	float stopTime;
	float deltaTime;
	float FPS;
};

class RTCounter
{
public:

	static void startTimer(GLuint timerNumber)
	{
		if (timerNumber < stopwatch.size())
		{
			stopwatch[timerNumber].startTime = static_cast<float>(glfwGetTime());
		}
		else
			std::cout << "Error: Timer doesnt exist!";
	}

	static void stopTimer(GLuint timerNumber)
	{
		if (timerNumber < stopwatch.size())
		{
			stopwatch[timerNumber].stopTime = static_cast<float>(glfwGetTime());
			stopwatch[timerNumber].deltaTime = stopwatch[timerNumber].stopTime - stopwatch[timerNumber].startTime;
			stopwatch[timerNumber].FPS = 1.0f / stopwatch[timerNumber].deltaTime;
		}
		else
			std::cout << "Error: Timer doesnt exist!";
	}

	static float getFPS(GLuint timerNumber)
	{
		if (timerNumber < stopwatch.size())
		{
			return stopwatch[timerNumber].FPS;
		}
		else
			std::cout << "Error: Timer doesnt exist!";
		return 0.0f;
	}

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
	static std::vector<FrameTiming> stopwatch;
};

