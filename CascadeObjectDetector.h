#pragma once
#include <thread>
#include <opencv2/opencv.hpp>
#include "ThreadSafeValue.h"
#include "Console.h"
#include "RTCounter.h"

const double scaleFactor = 1.1;
const int minNeighbors = 3;
const cv::Scalar frameColor(0, 0, 255);

class CascadeObjectDetector
{
public:
	CascadeObjectDetector(const std::string& path);

	CascadeObjectDetector(const std::list<std::string>& cascadeModelsList);

	~CascadeObjectDetector();

	void processFrame(const cv::Mat& frameIn);

	void drawObjects(cv::Mat& frameOut);

	float getFPS();

private:
	void startThread();

	void stopThread();

	void detectionLoop();


private:
	bool m_running = false;
	std::list <cv::CascadeClassifier> m_cascadeClassifiers;
	std::list <std::vector<cv::Rect>> m_objectsList;
	ThreadSafeValue<cv::Mat> m_currentFrame;
	std::thread m_objectDetectorLoopThread;
	std::mutex m_mutex;							// Mutex for synchronizing access
	std::condition_variable m_loopCondition;	// Conditional variable for waiting for data
	RTCounter FPStimer;

};

