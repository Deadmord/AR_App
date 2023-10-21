#pragma once

#include <thread>
#include "ThreadSafeValue.h"
#include "aruco/ArucoProcessor.h"

class ArucoThreadWrapper
{
public:
	ArucoThreadWrapper();
	ArucoThreadWrapper(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId = cv::aruco::DICT_6X6_250, std::string cameraParams = nullptr, bool showRejected = false);
	~ArucoThreadWrapper();

	// Добавить метод андистории для выравнивания изображения и использовать его до или после распознования меток (в зависимости от влияния на точность)
	void initAndStartAruco(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId = cv::aruco::DICT_6X6_250, std::string cameraParams = nullptr, bool showRejected = false);
	void processFrame(cv::Mat& frameIn);
	void undistortFrame(const cv::Mat& frameIn, cv::Mat& frameOut);
	bool tryGetProcessedFrame(cv::Mat& frameOut);
	bool tryPopProcessedFrame(cv::Mat& frameOut);
	Markers getDetectedMarkers();
	const cv::Size& getFrameSize() const;
	const glm::mat4& getProjectionMat() const;

private:
	void StartThread();
	void StopThread();
	void detectionLoop();

	bool m_running = false;

	ThreadSafeValue<cv::Mat> m_currentFrame;
	ThreadSafeValue<cv::Mat> m_detectedFrame;
	ThreadSafeValue<Markers> m_markers;
	std::shared_ptr <ArucoProcessor> m_arucoProcessor = nullptr;
	std::thread m_arucoLoopThread;
	std::mutex m_mutex;							// Mutex for synchronizing access
	std::condition_variable m_loopCondition;	// Conditional variable for waiting for data
};


