#include "RTCounter.h"

const unsigned int NUMBER_STOPWATCHS = 64;
const size_t RTCounter::windowSize = 50;

float RTCounter::lastPrintTime = 0.0f;
std::vector<FrameTiming> RTCounter::LocalTimers(NUMBER_STOPWATCHS);
FrameTiming RTCounter::MainTimer;

//--- Local Timers and FPS ---
bool RTCounter::isValidTimer(GLuint timerNumber) {
	if (timerNumber >= LocalTimers.size()) {
		Console::red() << "Error: Timer doesn't exist! Num: " << timerNumber << std::endl;
		return false;
	}
	return true;
}

void RTCounter::startTimer(GLuint timerNumber)
{
	if (!isValidTimer(timerNumber)) return;

	LocalTimers[timerNumber].startTime = static_cast<float>(glfwGetTime());
}

void RTCounter::stopTimer(GLuint timerNumber)
{
	if (!isValidTimer(timerNumber)) return;

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

float RTCounter::getFPS(GLuint timerNumber)
{
	if (!isValidTimer(timerNumber)) return 0.0f;

	return LocalTimers[timerNumber].FPS;
}

float RTCounter::getDeltaTime(GLuint timerNumber)
{
	if (!isValidTimer(timerNumber)) return 0.0f;

	return LocalTimers[timerNumber].avrDeltaTime;
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
		Console::blue() << '\r' << std::fixed << std::setprecision(1) << MainTimer.FPS << "  " << std::ends;
		lastPrintTime = currentTime;
	}
}

float RTCounter::getMainDeltaTime()
{
	return MainTimer.avrDeltaTime;
}

//------- methods timer instance ----------

void RTCounter::startTimer()
{
	timer.startTime = static_cast<float>(glfwGetTime());
}

void RTCounter::stopTimer()
{
	timer.stopTime = static_cast<float>(glfwGetTime());
	float dTime = timer.stopTime - timer.startTime;
	timer.dTimes.push_back(dTime);
	timer.sum += dTime;

	if (timer.dTimes.size() > windowSize) {
		timer.sum -= timer.dTimes.front();
		timer.dTimes.pop_front();
	}
	timer.avrDeltaTime = timer.sum / timer.dTimes.size();
	timer.FPS = 1.0f / timer.avrDeltaTime;
}

float RTCounter::getFPS()
{
	return timer.FPS;
}

float RTCounter::getDeltaTime()
{
	return timer.avrDeltaTime;
}