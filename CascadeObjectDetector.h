#pragma once
#include <thread>
#include <opencv2/opencv.hpp>
#include "ThreadSafeValue.h"
#include "Console.h"
#include "RTCounter.h"

class CascadeObjectDetector
{
	const double scaleFactor = 1.1;
	const int minNeighbors = 3;

	// Text parameters.
	const float FONT_SCALE = 0.6f;
	const int FONT_FACE = cv::FONT_HERSHEY_SIMPLEX;
	const int THICKNESS_1 = 1;
	const int THICKNESS_2 = 2;

	// Color.
	const cv::Scalar RED = cv::Scalar(0, 0, 255);

public:
	CascadeObjectDetector(const std::string& path);

	CascadeObjectDetector(const std::list<std::string>& cascadeModelPathList);

	~CascadeObjectDetector();

	void processFrame(const cv::Mat& frameIn);

	void showObjects(cv::Mat& frameOut);

	float getFPS();

private:
	void startThread();

	void stopThread();

	void detectionLoop();


private:
	bool m_running = false;
	std::list <cv::CascadeClassifier> m_cascadeClassifiers;
	ThreadSafeValue<std::list <std::vector<cv::Rect>>> m_objectsList;
	ThreadSafeValue<cv::Mat> m_currentFrame;
	std::thread m_objectDetectorLoopThread;
	std::mutex m_mutex;							// Mutex for synchronizing access
	std::condition_variable m_loopCondition;	// Conditional variable for waiting for data
	RTCounter FPStimer;

};

