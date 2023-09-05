#include "RTCounter.h"

const unsigned int NUMBER_STOPWATCHS = 64;
const size_t RTCounter::windowSize = 25;

float RTCounter::deltaTime = 0.0f;
float RTCounter::lastFrameTime = 0.0f;
float RTCounter::lastPrintTime = 0.0f;
std::vector<FrameTiming> RTCounter::stopwatch(NUMBER_STOPWATCHS);

void RTCounter::startTimer(GLuint timerNumber)
{
	if (timerNumber < stopwatch.size())
	{
		stopwatch[timerNumber].startTime = static_cast<float>(glfwGetTime());
	}
	else
		std::cout << "Error: Timer doesnt exist!";
}

void RTCounter::stopTimer(GLuint timerNumber)
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

float RTCounter::getFPS(GLuint timerNumber)
{
	if (timerNumber < stopwatch.size())
	{
		return stopwatch[timerNumber].FPS;
	}
	else
		std::cout << "Error: Timer doesnt exist!";
	return 0.0f;
}

float RTCounter::getDeltaTime(GLuint timerNumber)
{
	if (timerNumber < stopwatch.size())
	{
		return stopwatch[timerNumber].avrDeltaTime;
	}
	else
		std::cout << "Error: Timer doesnt exist!";
	return 0.0f;
}

void RTCounter::updateTimer()
{
	// per-frame time logic
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrameTime;
	lastFrameTime = currentFrame;
}

void RTCounter::printFPS_Console()
{
	float currentTime = static_cast<float>(glfwGetTime());
	if (currentTime - lastPrintTime >= 0.25f)
	{
		std::cout << '\r' << std::fixed << std::setprecision(1) << 1.0f / deltaTime << "  ";
		lastPrintTime = currentTime;
	}
}

float RTCounter::getDeltaTime()
{
	return deltaTime;
}
