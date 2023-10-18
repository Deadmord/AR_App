#pragma once
#include <iostream>
#include <iomanip>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <deque>
#include "Console.h"

struct FrameTiming
{
	float startTime;
	float stopTime;
	float avrDeltaTime; // Avarage time between start and stop
	float FPS;

	float sum;
	std::deque<float> dTimes;
};

class RTCounter
{
public:
	
	static void startTimer(GLuint timerNumber);

	static void stopTimer(GLuint timerNumber);

	static float getFPS(GLuint timerNumber);

	static float getDeltaTime(GLuint timerNumber);

	//------- methods for count FPS in mainloop ----------

	static void StartMainTimer();

	static void StopMainTimer();

	static void updateMainTimer();

	static void printMainFPS_Console();

	static float getMainDeltaTime();

private:
	const static size_t windowSize;	//setting for count avarege freq
	static std::vector<FrameTiming> LocalTimers;	//data for calculate time and freq
	static FrameTiming MainTimer;
	static float lastPrintTime; // Time of last print FPS
};

//// Hight accuracy timer --------------------------------
//
//int64 start = cv::getTickCount();
//// Some load operations
//int64 end = cv::getTickCount();
//double duration = (end - start) / cv::getTickFrequency();
