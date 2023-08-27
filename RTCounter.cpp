#include "RTCounter.h"

const unsigned int NUMBER_STOPWATCHS = 10;

float RTCounter::deltaTime = 0.0f;
float RTCounter::lastFrameTime = 0.0f;
float RTCounter::lastPrintTime = 0.0f;
std::vector<FrameTiming> RTCounter::stopwatch(NUMBER_STOPWATCHS);