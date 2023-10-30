#pragma once

#include <opencv2/opencv.hpp>

class Smoother
{
public:
	Smoother(double alpha_, size_t maxHistorySize_);

	void addValueAndAverage(const cv::Vec3d& newValue);
	cv::Vec3d getSmoothedValue() const;

private:
	double alpha;
	size_t maxHistorySize;
	cv::Vec3d prevSmoothedValue = cv::Vec3d(0, 0, 0);
	cv::Vec3d smoothedValue = cv::Vec3d(0, 0, 0);
	cv::Vec3d avgValue = cv::Vec3d(0, 0, 0);
	std::deque<cv::Vec3d> valueHistory;
};

