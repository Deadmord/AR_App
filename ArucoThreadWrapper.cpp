#include "ArucoThreadWrapper.h"

ArucoThreadWrapper::ArucoThreadWrapper(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId, std::string cameraParams, bool showRejected) : stopThread(false)
{
	arucoProcessor = std::make_shared<ArucoProcessor>(markerLength, dictionaryId, cameraParams, showRejected);
}

void ArucoThreadWrapper::threadFunction()
{
	try
	{
		while (!stopThread)
		{
			cv::Mat frameToProcess;
			{
				std::unique_lock<std::mutex> lock(dataMutex);
				dataCondVar.wait(lock, [this]() {
					return !currentFrame.empty() || stopThread;
					});
				frameToProcess = currentFrame.clone();
				currentFrame.release();
			}
			if (!frameToProcess.empty()) {
				cv::Mat resultFrame;
				arucoProcessor->detectMarkers(frameToProcess, resultFrame);
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::threadFunction: " << e.what() << std::endl;
	}
}

void ArucoThreadWrapper::ProcessFrame(const cv::Mat& frame)
{
	try
	{
		std::lock_guard<std::mutex> lock(dataMutex);
		currentFrame = frame.clone();
		dataCondVar.notify_one();
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::ProcessFrame: " << e.what() << std::endl;
	}
}

const Markers& ArucoThreadWrapper::GetDetectedMarkers() const
{
	try
	{
		std::lock_guard<std::mutex> lock(dataMutex);
		return arucoProcessor->getMarkers();
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::GetDetectedMarkers: " << e.what() << std::endl;
	}
}

void ArucoThreadWrapper::StartThread()
{
	try
	{
		if (!arucoThread.joinable()) {
			stopThread = false;
			arucoThread = std::thread(&ArucoThreadWrapper::threadFunction, this);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::StartThread: " << e.what() << std::endl;
	}
}

void ArucoThreadWrapper::StopThread()
{
	try
	{
		{
			std::lock_guard<std::mutex> lock(dataMutex);
			stopThread = true;
			dataCondVar.notify_one();
		}
		if (arucoThread.joinable()) {
			arucoThread.join();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::StopThread: " << e.what() << std::endl;
	}
}

bool ArucoThreadWrapper::DetectMarkers(const cv::Mat& frame, cv::Mat& frameCopy)
{
	return arucoProcessor->detectMarkers(frame, frameCopy);
}

const cv::Size& ArucoThreadWrapper::GetFrameSize() const
{
	return arucoProcessor->getFrameSize();
}

const glm::mat4& ArucoThreadWrapper::GetProjectionMat() const
{
	return arucoProcessor->getProjectionMat();
}

ArucoThreadWrapper::~ArucoThreadWrapper()
{
	try
	{
		{
			std::lock_guard<std::mutex> lock(dataMutex);
			stopThread = true;
			dataCondVar.notify_one();
		}
		arucoThread.join();
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::~ArucoThreadWrapper: " << e.what() << std::endl;
	}
}
