#include "RTCounter.h"

const unsigned int NUMBER_STOPWATCHS = 64;
const size_t RTCounter::windowSize = 25;

float RTCounter::deltaTime = 0.0f;
float RTCounter::lastFrameTime = 0.0f;
float RTCounter::lastPrintTime = 0.0f;
std::vector<FrameTiming> RTCounter::stopwatch(NUMBER_STOPWATCHS);

bool RTCounter::isValidTimer(GLuint timerNumber) {
    if (timerNumber >= stopwatch.size()) {
        std::cerr << "Error: Timer doesn't exist!" << std::endl;
        return false;
    }
    return true;
}

void RTCounter::startTimer(GLuint timerNumber) {
    if (!isValidTimer(timerNumber)) return;

    stopwatch[timerNumber].startTime = static_cast<float>(glfwGetTime());
}

void RTCounter::stopTimer(GLuint timerNumber) {
    if (!isValidTimer(timerNumber)) return;

    float currentTime = static_cast<float>(glfwGetTime());
    float dTime = currentTime - stopwatch[timerNumber].startTime;
    stopwatch[timerNumber].dTimes.push_back(dTime);
    stopwatch[timerNumber].sum += dTime;

    if (stopwatch[timerNumber].dTimes.size() > windowSize) {
        stopwatch[timerNumber].sum -= stopwatch[timerNumber].dTimes.front();
        stopwatch[timerNumber].dTimes.pop_front();
    }
    stopwatch[timerNumber].avrDeltaTime = stopwatch[timerNumber].sum / stopwatch[timerNumber].dTimes.size();
    stopwatch[timerNumber].FPS = 1.0f / stopwatch[timerNumber].avrDeltaTime;
}

float RTCounter::getFPS(GLuint timerNumber) {
    if (!isValidTimer(timerNumber)) return 0.0f;

    return stopwatch[timerNumber].FPS;
}

float RTCounter::getDeltaTime(GLuint timerNumber) {
    if (!isValidTimer(timerNumber)) return 0.0f;

    return stopwatch[timerNumber].avrDeltaTime;
}

void RTCounter::updateTimer() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrameTime;
    lastFrameTime = currentFrame;
}

void RTCounter::printFPS_Console() {
    float currentTime = static_cast<float>(glfwGetTime());
    if (currentTime - lastPrintTime >= 0.25f) {
        std::cout << '\r' << std::fixed << std::setprecision(1) << 1.0f / deltaTime << "  ";
        lastPrintTime = currentTime;
    }
}


float RTCounter::getDeltaTime()
{
	return deltaTime;
}
