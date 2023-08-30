#pragma once
#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>
#include <vector>
#include "Structs.h"

class RTCounter
{
public:

	static void startTimer(GLuint timerNumber);
	static void stopTimer(GLuint timerNumber);
	static float getFPS(GLuint timerNumber);
	static void updateTimer();
	static void printFPS_Console();
	static float getDeltaTime();

private:
	static float deltaTime;	// Time between current frame and last frame
	static float lastFrameTime; // Time of last frame
	static float lastPrintTime; // Time of last print FPS
	static std::vector<FrameTiming> stopwatch;
};

