#include "RTCounter.h"

const unsigned int NUMBER_STOPWATCHS = 64;
const size_t RTCounter::windowSize = 25;

float RTCounter::lastPrintTime = 0.0f;
std::vector<FrameTiming> RTCounter::LocalTimers(NUMBER_STOPWATCHS);
FrameTiming RTCounter::MainTimer;

//--- Local Timers and FPS ---

void RTCounter::startTimer(GLuint timerNumber)
{
	if (timerNumber < LocalTimers.size())
	{
		LocalTimers[timerNumber].startTime = static_cast<float>(glfwGetTime());
	}
	else
	Console::red() << "Error: Timer doesnt exist!";
}

void RTCounter::stopTimer(GLuint timerNumber)
{
	if (timerNumber < LocalTimers.size())
	{
		LocalTimers[timerNumber].stopTime = static_cast<float>(glfwGetTime());
		float dTime = LocalTimers[timerNumber].stopTime - LocalTimers[timerNumber].startTime;
		LocalTimers[timerNumber].dTimes.push_back(dTime);
		LocalTimers[timerNumber].sum += dTime;

		if (LocalTimers[timerNumber].dTimes.size() > windowSize) {
			LocalTimers[timerNumber].sum -= LocalTimers[timerNumber].dTimes.front();
			LocalTimers[timerNumber].dTimes.pop_front();
		}
		LocalTimers[timerNumber].avrDeltaTime = LocalTimers[timerNumber].sum / LocalTimers[timerNumber].dTimes.size();
		LocalTimers[timerNumber].FPS = 1.0f / LocalTimers[timerNumber].avrDeltaTime;
	}
	else
		Console::red() << "Error: Timer doesnt exist!";
}

float RTCounter::getFPS(GLuint timerNumber)
{
	if (timerNumber < LocalTimers.size())
	{
		return LocalTimers[timerNumber].FPS;
	}
	else
		Console::red() << "Error: Timer doesnt exist!";
	return 0.0f;
}

float RTCounter::getDeltaTime(GLuint timerNumber)
{
	if (timerNumber < LocalTimers.size())
	{
		return LocalTimers[timerNumber].avrDeltaTime;
	}
	else
		Console::red() << "Error: Timer doesnt exist!";
	return 0.0f;
}

//--- Console general FPS timer ---
void RTCounter::StartMainTimer()
{
	MainTimer.startTime = static_cast<float>(glfwGetTime());
}

void RTCounter::StopMainTimer()
{
	MainTimer.stopTime = static_cast<float>(glfwGetTime());
	float dTime = MainTimer.stopTime - MainTimer.startTime;
	MainTimer.dTimes.push_back(dTime);
	MainTimer.sum += dTime;

	if (MainTimer.dTimes.size() > windowSize) {
		MainTimer.sum -= MainTimer.dTimes.front();
		MainTimer.dTimes.pop_front();
	}
	MainTimer.avrDeltaTime = MainTimer.sum / MainTimer.dTimes.size();
	MainTimer.FPS = 1.0f / MainTimer.avrDeltaTime;
}

void RTCounter::updateMainTimer()
{
	MainTimer.stopTime = static_cast<float>(glfwGetTime());
	float dTime = MainTimer.stopTime - MainTimer.startTime;
	MainTimer.startTime = MainTimer.stopTime;	//New timelap start from end of last lap
	MainTimer.dTimes.push_back(dTime);
	MainTimer.sum += dTime;

	if (MainTimer.dTimes.size() > windowSize) {
		MainTimer.sum -= MainTimer.dTimes.front();
		MainTimer.dTimes.pop_front();
	}
	MainTimer.avrDeltaTime = MainTimer.sum / MainTimer.dTimes.size();
	MainTimer.FPS = 1.0f / MainTimer.avrDeltaTime;
}

void RTCounter::printMainFPS_Console()
{
	float currentTime = static_cast<float>(glfwGetTime());
	if (currentTime - lastPrintTime >= 0.33f)
	{
		Console::blue() << '\r' << std::fixed << std::setprecision(1) << MainTimer.FPS << "  ";
		lastPrintTime = currentTime;
	}
}

float RTCounter::getMainDeltaTime()
{
	return MainTimer.avrDeltaTime;
}
