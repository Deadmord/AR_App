#include "RTCounter.h"

const unsigned int NUMBER_STOPWATCHS = 64;
const size_t RTCounter::windowSize = 25;

float RTCounter::deltaTime = 0.0f;
float RTCounter::lastFrameTime = 0.0f;
float RTCounter::lastPrintTime = 0.0f;
std::vector<FrameTiming> RTCounter::stopwatch(NUMBER_STOPWATCHS);