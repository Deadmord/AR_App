#include "CascadeObjectDetector.h"

CascadeObjectDetector::CascadeObjectDetector(const std::string& path) : m_running(false)
{
	cv::Ptr<cv::cuda::CascadeClassifier> cascadeClassifier = cv::cuda::CascadeClassifier::create(path);

	if (cascadeClassifier->empty())
	{
		Console::red() << "Error load cascade classifier model: " << path << std::endl;
	}
	else
	{
		cascadeClassifier->setScaleFactor(scaleFactor);
		cascadeClassifier->setMinNeighbors(minNeighbors);
		m_cascadeClassifiers.push_back(cascadeClassifier);
		startThread();
	}
}

CascadeObjectDetector::CascadeObjectDetector(const std::list<std::string>& cascadeModelPathList) : m_running(false)
{
	for (std::string model : cascadeModelPathList)
	{
		cv::Ptr<cv::cuda::CascadeClassifier> cascadeClassifier = cv::cuda::CascadeClassifier::create(model);
		if (cascadeClassifier->empty())
		{
			Console::red() << "Error load cascade classifier model: " << model << std::endl;
		}
		else
		{
			cascadeClassifier->setScaleFactor(scaleFactor);
			cascadeClassifier->setMinNeighbors(minNeighbors);
			m_cascadeClassifiers.push_back(cascadeClassifier);
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

void CascadeObjectDetector::processFrame(const cv::UMat& frameIn)
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

void CascadeObjectDetector::showObjects(cv::UMat& frameOut)
{
	std::list <std::vector<cv::Rect>> objectsList;
	if (m_objectsList.tryGet(objectsList))
	{
		int index{0};
		// drouing rectangles for detected objects
		for (std::vector<cv::Rect> objects : objectsList)
		{
			for (const cv::Rect& object : objects) {
				if(index == 0) cv::rectangle(frameOut, object, RED, 2);
				else if (index == 1) cv::rectangle(frameOut, object, GREEN, 2);
				else if (index == 2) cv::rectangle(frameOut, object, BLUE, 2);
				else if (index == 3) cv::rectangle(frameOut, object, YELLOW, 2);
			}
			index++;
		}
	}

	std::string fps = cv::format("FPScscd: %.2f ", getFPS());
	putText(frameOut, fps, cv::Point(480, 100), FONT_FACE, FONT_SCALE, RED, THICKNESS_2);
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
			cv::UMat frameToProcess;
			if (m_currentFrame.tryPop(frameToProcess)) 
			{
				// Convertion frame to gpuMat
				cv::cuda::GpuMat frame_gpu(frameToProcess);
				// Convertion frame to gray for detection improve
				cv::cuda::cvtColor(frame_gpu, frame_gpu, cv::COLOR_RGB2GRAY);
				std::list <std::vector<cv::Rect>> objectsList;
				// detection loop
				for (cv::Ptr<cv::cuda::CascadeClassifier> cascadeClassifier : m_cascadeClassifiers)
				{
					std::unique_lock<std::mutex> lock(m_mutex);
					cv::cuda::GpuMat objectsGpu;
					cascadeClassifier->detectMultiScale(frame_gpu, objectsGpu);

					std::vector<cv::Rect> objects;
					cascadeClassifier->convert(objectsGpu, objects);
					objectsList.push_back(std::move(objects));
				}
				m_objectsList.push(std::move(objectsList));
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
