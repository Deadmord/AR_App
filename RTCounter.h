#pragma once
#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>
#include <vector>
#include <deque>

struct FrameTiming
{
	float startTime;
	float stopTime;
	float avrDeltaTime;
	float FPS;

	float sum;
	std::deque<float> dTimes;
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
			float dTime = stopwatch[timerNumber].stopTime - stopwatch[timerNumber].startTime;
			stopwatch[timerNumber].dTimes.push_back(dTime);
			stopwatch[timerNumber].sum += dTime;

			if (stopwatch[timerNumber].dTimes.size() > windowSize) {
				stopwatch[timerNumber].sum -= stopwatch[timerNumber].dTimes.front();
				stopwatch[timerNumber].dTimes.pop_front();
			}
			stopwatch[timerNumber].avrDeltaTime = stopwatch[timerNumber].sum / stopwatch[timerNumber].dTimes.size();
			stopwatch[timerNumber].FPS = 1.0f / stopwatch[timerNumber].avrDeltaTime;
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

	static float getDeltaTime(GLuint timerNumber)
	{
		if (timerNumber < stopwatch.size())
		{
			return stopwatch[timerNumber].avrDeltaTime;
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
	const static size_t windowSize;
	static std::vector<FrameTiming> stopwatch;
};

