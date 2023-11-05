#include "CascadeObjectDetector.h"

CascadeObjectDetector::CascadeObjectDetector(const std::string& path) : m_running(false)
{
	cv::CascadeClassifier cascadeClassifier;
	if (cascadeClassifier.load(path))
	{
		m_cascadeClassifiers.push_back(cascadeClassifier);
		startThread();
	}
	else {
		Console::red() << "Error load cascade classifier model: " << path << std::endl;
	}
}

CascadeObjectDetector::CascadeObjectDetector(const std::list<std::string>& cascadeModelsList) : m_running(false)
{
	for (std::string model : cascadeModelsList)
	{
		cv::CascadeClassifier cascadeClassifier;
		if (cascadeClassifier.load(model))
		{
			m_cascadeClassifiers.push_back(cascadeClassifier);
		}
		else {
			Console::red() << "Error load cascade classifier model: " << model << std::endl;
		}
	}
	if (!m_cascadeClassifiers.empty())
	{
		startThread();
	}
}

CascadeObjectDetector::~CascadeObjectDetector()
{
	stopThread();
}

void CascadeObjectDetector::processFrame(const cv::Mat& frameIn)
{
	try
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_currentFrame.push(frameIn.clone());
		lock.unlock();
		m_loopCondition.notify_one();
	}
	catch (const std::exception& e)
	{
		Console::red() << "CascadeObjectDetector::processFrame exception: " << e.what() << std::endl;
	}
}

void CascadeObjectDetector::drawObjects(cv::Mat& frameOut)
{
	// drouing rectangles for detected objects
	for (std::vector<cv::Rect> objects : m_objectsList)
	{
		for (const cv::Rect& object : objects) {
			cv::rectangle(frameOut, object, frameColor, 2);
		}
	}
}

float CascadeObjectDetector::getFPS()
{
	return FPStimer.getFPS();
}

void CascadeObjectDetector::startThread()
{
	try
	{
		if (!m_objectDetectorLoopThread.joinable()) {
			m_running = true;
			m_objectDetectorLoopThread = std::thread(&CascadeObjectDetector::detectionLoop, this);
			Console::green() << "CascadeObject detectionLoop started" << "\tLoopThread: " << m_objectDetectorLoopThread.get_id() << "\tThreadWrapperPtr: " << this << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		Console::red() << "CascadeObjectDetector::StartThread: " << e.what() << std::endl;
	}
}

void CascadeObjectDetector::stopThread()
{
	try
	{
		m_running = false;
		m_loopCondition.notify_one();
		if (m_objectDetectorLoopThread.joinable()) {
			Console::yellow() << "CascadeObject detectionLoop stopped" << "\tLoopThread: " << m_objectDetectorLoopThread.get_id() << "\tThreadWrapperPtr: " << this << std::endl;
			m_objectDetectorLoopThread.join();
		}
	}
	catch (const std::exception& e)
	{
		Console::red() << "CascadeObjectDetector::StopThread: " << e.what() << std::endl;
	}
}

void CascadeObjectDetector::detectionLoop()
{
	try
	{
		while (m_running)
		{
			FPStimer.startTimer();
			cv::Mat frameToProcess;
			if (m_currentFrame.tryPop(frameToProcess)) {

				// Convertion frame to gray for detection improve
				cv::Mat gray;
				cv::cvtColor(frameToProcess, gray, cv::COLOR_BGR2GRAY);

				m_objectsList.clear();
				// detection
				for (cv::CascadeClassifier cascadeClassifier : m_cascadeClassifiers)
				{
					std::vector<cv::Rect> objects;
					cascadeClassifier.detectMultiScale(gray, objects, scaleFactor, minNeighbors);

					std::unique_lock<std::mutex> lock(m_mutex);
					m_objectsList.push_back(std::move(objects));
				}

			}
			else
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_loopCondition.wait(lock);
			}
			FPStimer.stopTimer();
		}
	}
	catch (const std::exception& e)
	{
		Console::red() << "CascadeObjectDetector::detectionLoop: " << e.what() << std::endl;
	}
}
