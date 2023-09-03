#pragma once

#include <opencv2/opencv.hpp>

class FrameAnalyzer
{
public:

	FrameAnalyzer();

	void showInFrame(const cv::Mat& frame, cv::Size WindSize, cv::Size frameSize, float FPS);


};

