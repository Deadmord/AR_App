#pragma once

#include <thread>
#include "ThreadSafeValue.h"
#include "aruco/ArucoProcessor.h"

class ArucoThreadWrapper
{
public:
	ArucoThreadWrapper(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId = cv::aruco::DICT_6X6_250, std::string cameraParams = nullptr, bool showRejected = false);
	
	~ArucoThreadWrapper();

	// �������� ����� ���������� ��� ������������ ����������� � ������������ ��� �� ��� ����� ������������� ����� (� ����������� �� ������� �� ��������)

	void processFrame(const cv::Mat& frameIn);
	void undistortFrame(const cv::Mat& frameIn, cv::Mat& frameOut);
	bool tryGetProcessedFrame(cv::Mat& frameOut);
	bool tryPopProcessedFrame(cv::Mat& frameOut);
	Markers GetDetectedMarkers();
	const cv::Size& GetFrameSize() const;
	const glm::mat4& GetProjectionMat() const;

private:
	void StartThread();
	void StopThread();
	void detectionLoop();

	bool m_running = false;

	ThreadSafeValue<cv::Mat> m_currentFrame;
	ThreadSafeValue<cv::Mat> m_detectedFrame;
	ThreadSafeValue<Markers> m_markers;

	std::shared_ptr <ArucoProcessor> m_arucoProcessor;
	std::thread m_arucoLoopThread;
};


