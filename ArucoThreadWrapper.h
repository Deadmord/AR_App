#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include "aruco/ArucoProcessor.h"

class ArucoThreadWrapper
{
private:
	std::thread arucoThread;
	mutable std::mutex dataMutex;
	std::condition_variable dataCondVar;
	bool stopThread = false;
	cv::Mat currentFrame;

	void threadFunction();
	std::shared_ptr <ArucoProcessor> arucoProcessor;

public:
	ArucoThreadWrapper(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId = cv::aruco::DICT_6X6_250, std::string cameraParams = nullptr, bool showRejected = false);
	void ProcessFrame(const cv::Mat& frame);
	const Markers& GetDetectedMarkers() const;
	void StartThread();
	void StopThread();
	bool DetectMarkers(const cv::Mat& frame, cv::Mat& frameCopy);
	const cv::Size& GetFrameSize() const;
	const glm::mat4& GetProjectionMat() const;
	~ArucoThreadWrapper();
};

