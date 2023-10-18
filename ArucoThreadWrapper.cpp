#include "ArucoThreadWrapper.h"

ArucoThreadWrapper::ArucoThreadWrapper(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId, std::string cameraParams, bool showRejected) : m_running(false)
{
	m_arucoProcessor = std::make_shared<ArucoProcessor>(markerLength, dictionaryId, cameraParams, showRejected);
	StartThread();
}

ArucoThreadWrapper::~ArucoThreadWrapper()
{
	StopThread();
}

void ArucoThreadWrapper::processFrame(const cv::Mat& frameIn)
{
	try
	{
		m_currentFrame.push(frameIn.clone());
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::processFrame: " << e.what() << std::endl;
	}
}

inline void ArucoThreadWrapper::undistortFrame(const cv::Mat& frameIn, cv::Mat& frameOut)
{
	//calc and apply distortion correction, very heavy hendling!!!
	//cv::Mat undistortedFrame;
	//cv::undistort(frameVideo, undistortedFrame, arucoProcessorPtr->getCameraMat(), arucoProcessorPtr->getDistortCoeff());

	//only apply distortion maps, mach more faster!!!
	// попробовать сохранить UndistortMap1/2 в самом классе ArucoThreadWrapper, мб будет быстрее...
	cv::remap(frameIn, frameOut, m_arucoProcessor->getUndistortMap1(), m_arucoProcessor->getUndistortMap2(), cv::INTER_LINEAR);
}

bool ArucoThreadWrapper::tryGetProcessedFrame(cv::Mat& frameOut)
{
	try
	{
		return m_detectedFrame.tryGet(frameOut);
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::tryGetProcessedFrame: " << e.what() << std::endl;
	}
}

bool ArucoThreadWrapper::tryPopProcessedFrame(cv::Mat& frameOut)
{
	try
	{
		return m_detectedFrame.tryPop(frameOut);
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::tryPopProcessedFrame: " << e.what() << std::endl;
	}
}

Markers ArucoThreadWrapper::GetDetectedMarkers()
{
	try
	{
		Markers markers;
		m_markers.waitAndGet(markers);
		return std::move(markers);

		//auto markers = std::make_shared<Markers>();		// option with pointer for reference
		//m_markers.waitAndGet(*markers);
		//return markers;
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::GetDetectedMarkers: " << e.what() << std::endl;
	}
}

const cv::Size& ArucoThreadWrapper::GetFrameSize() const
{
	return m_arucoProcessor->getFrameSize();
}

const glm::mat4& ArucoThreadWrapper::GetProjectionMat() const
{
	return m_arucoProcessor->getProjectionMat();
}

void ArucoThreadWrapper::StartThread()
{
	try
	{
		if (!m_arucoLoopThread.joinable()) {
			m_running = true;
			m_arucoLoopThread = std::thread(&ArucoThreadWrapper::detectionLoop, this);
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
		m_running = false;
		if (m_arucoLoopThread.joinable()) {
			m_arucoLoopThread.join();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::StopThread: " << e.what() << std::endl;
	}
}

void ArucoThreadWrapper::detectionLoop()
{
	try
	{
		while (m_running)
		{
			cv::Mat frameToProcess;
			m_currentFrame.waitAndPop(frameToProcess);
			if (!frameToProcess.empty()) {
				cv::Mat resultFrame;

				m_arucoProcessor->detectMarkers(frameToProcess, resultFrame);
				m_detectedFrame.push(std::move(resultFrame));
				m_markers.push(Markers(m_arucoProcessor->getMarkers()));
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "ArucoThreadWrapper::detectionLoop: " << e.what() << std::endl;
	}
}
