#include "Smoother.h"

void Smoother::addValueAndAverage(const cv::Vec3d& newValue) {

	cv::Vec3d intermediateValue = alpha * newValue + (1 - alpha) * prevSmoothedValue;

	if (valueHistory.size() == maxHistorySize) {
		avgValue -= valueHistory.front();
		valueHistory.pop_front();
	}

	avgValue += intermediateValue;
	valueHistory.push_back(intermediateValue);

	smoothedValue = avgValue / static_cast<double>(valueHistory.size());

	prevSmoothedValue = smoothedValue;
}

cv::Vec3d Smoother::getSmoothedValue() const {

	return this->smoothedValue;
}
