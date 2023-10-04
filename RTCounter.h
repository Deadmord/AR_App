#pragma once
#include <iostream>
#include <iomanip>
#include <glad/glad.h>
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
	static bool isValidTimer(GLuint timerNumber);
	static void startTimer(GLuint timerNumber);

	static void stopTimer(GLuint timerNumber);

	static float getFPS(GLuint timerNumber);

	static float getDeltaTime(GLuint timerNumber);

	//------- methods for count FPS in mainloop ----------

	static void updateTimer();

	static void printFPS_Console();

	static float getDeltaTime();

private:
	const static size_t windowSize;	//setting for count avarege freq
	static std::vector<FrameTiming> stopwatch;	//data for calculate time and freq

	static float deltaTime;	// Time between current frame and last frame
	static float lastFrameTime; // Time of last frame
	static float lastPrintTime; // Time of last print FPS
};

