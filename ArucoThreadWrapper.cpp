#include "ArucoThreadWrapper.h"
#include <GLFW/glfw3.h>

ArucoThreadWrapper::ArucoThreadWrapper() : m_arucoProcessor(nullptr), m_running(false)
{
	Console::yellow() << "ArucoThreadWrapper created: " << this << std::endl;
}

ArucoThreadWrapper::ArucoThreadWrapper(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId, std::string cameraParams, bool showRejected) : m_running(false)
{
	Console::yellow() << "ArucoThreadWrapper created: " << this << std::endl;
	m_arucoProcessor = std::make_shared<ArucoProcessor>(markerLength, dictionaryId, cameraParams, showRejected);
	StartThread();
}

ArucoThreadWrapper::~ArucoThreadWrapper()
{
	StopThread();
	Console::yellow() << "ArucoThreadWrapper deleted: " << this << std::endl;
}

void ArucoThreadWrapper::initAndStartAruco(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId, std::string cameraParams, bool showRejected)
{
	m_arucoProcessor = std::make_shared<ArucoProcessor>(markerLength, dictionaryId, cameraParams, showRejected);
	StartThread();
}

void ArucoThreadWrapper::processFrame(cv::Mat& frameIn)
{
	try
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_currentFrame.push(std::move(frameIn));
		lock.unlock();
		m_loopCondition.notify_one();
	}
	catch (const std::exception& e)
	{
		Console::red() << "ArucoThreadWrapper::processFrame: " << e.what() << std::endl;
	}
}

void ArucoThreadWrapper::undistortFrame(const cv::Mat& frameIn, cv::Mat& frameOut)
{
	try
	{
		//calc and apply distortion correction, very heavy hendling!!!
		//cv::Mat undistortedFrame;
		//cv::undistort(frameIn, frameOut, m_arucoProcessor->getCameraMat(), m_arucoProcessor->getDistortCoeff());

		//only apply distortion maps, mach more faster!!!
		// попробовать сохранить UndistortMap1/2 в самом классе ArucoThreadWrapper, мб будет быстрее...
		cv::remap(frameIn, frameOut, m_arucoProcessor->getUndistortMap1(), m_arucoProcessor->getUndistortMap2(), cv::INTER_LINEAR);
	}
	catch (const std::exception& e)
	{
		Console::red() << "ArucoThreadWrapper::undistortFrame: " << e.what() << std::endl;
	}
}

bool ArucoThreadWrapper::tryGetProcessedFrame(cv::Mat& frameOut)
{
	try
	{
		return m_detectedFrame.tryGet(frameOut);
	}
	catch (const std::exception& e)
	{
		Console::red() << "ArucoThreadWrapper::tryGetProcessedFrame: " << e.what() << std::endl;
		return false;
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
		Console::red() << "ArucoThreadWrapper::tryPopProcessedFrame: " << e.what() << std::endl;
		return false;
	}
}

Markers ArucoThreadWrapper::getDetectedMarkers()
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
		Console::red() << "ArucoThreadWrapper::GetDetectedMarkers: " << e.what() << std::endl;
		return Markers();
	}
}

const cv::Size& ArucoThreadWrapper::getFrameSize() const
{
	return m_arucoProcessor->getFrameSize();
}

const glm::mat4& ArucoThreadWrapper::getProjectionMat() const
{
	return m_arucoProcessor->getProjectionMat();
}

float ArucoThreadWrapper::getFPS()
{
	return FPStimer.getFPS();
}

void ArucoThreadWrapper::StartThread()
{
	try
	{
		if (!m_arucoLoopThread.joinable()) {
			m_running = true;
			m_arucoLoopThread = std::thread(&ArucoThreadWrapper::detectionLoop, this);
			Console::green() << "Aruco detectionLoop started" << "\tLoopThread: " << m_arucoLoopThread.get_id() << "\tThreadWrapperPtr: " << this << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		Console::red() << "ArucoThreadWrapper::StartThread: " << e.what() << std::endl;
	}
}

void ArucoThreadWrapper::StopThread()
{
	try
	{
		m_running = false;
		m_loopCondition.notify_one();
		if (m_arucoLoopThread.joinable()) {
			Console::yellow() << "Aruco detectionLoop stopped" << "\tLoopThread: " << m_arucoLoopThread.get_id() << "\tThreadWrapperPtr: " << this << std::endl;
			m_arucoLoopThread.join();
		}
	}
	catch (const std::exception& e)
	{
		Console::red() << "ArucoThreadWrapper::StopThread: " << e.what() << std::endl;
	}
}

void ArucoThreadWrapper::detectionLoop()
{
	try
	{
		while (m_running)
		{
			FPStimer.startTimer();
			cv::Mat frameToProcess;
			if (m_currentFrame.tryPop(frameToProcess)) {
				//Console::green() << static_cast<float>(glfwGetTime()) << "\t\t\tThread: " << std::this_thread::get_id() << std::endl;
				cv::Mat resultFrame;

				m_arucoProcessor->detectMarkers(frameToProcess, resultFrame);
				m_detectedFrame.push(std::move(resultFrame));
				m_markers.push(Markers(m_arucoProcessor->getMarkers()));
			}
			else
			{
				//Console::red() << static_cast<float>(glfwGetTime()) << "\tWating detectionLoop: "<< std::this_thread::get_id() << std::endl;
				std::unique_lock<std::mutex> lock(m_mutex);
				m_loopCondition.wait(lock);
			}
			FPStimer.stopTimer();
		}
	}
	catch (const std::exception& e)
	{
		Console::red() << "ArucoThreadWrapper::detectionLoop: " << e.what() << std::endl;
	}
}
