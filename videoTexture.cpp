#include "videoTexture.h"
#include <GLFW/glfw3.h>

videoTexture::~videoTexture()
{
	if (m_running_)
	{
		Console::yellow() << "Video texture CaptureLoop stop!" << "\tLoopThread: " << m_captureLoopThread_->get_id() << "\tTexturePtr: " << this << std::endl;
		stop();
	}
	Console::yellow() << "Video texture deleted!" << "\tTexturePtr: " << this << std::endl;
}

void videoTexture::start()
{
	m_running_ = true;
	m_captureLoopThread_ = std::make_shared<std::thread>(&videoTexture::captureLoop, this);
}

void videoTexture::stop()
{
	m_running_ = false;
	if (m_captureLoopThread_->joinable())
		m_captureLoopThread_->join();
}

uchar* videoTexture::getData() {
	cv::Mat frame;
	currentValue_.waitAndGet(frame);
	return frame.data;
}

float videoTexture::getFPS()
{
	return FPStimer.getFPS();
}

const std::string& videoTexture::getCameraParams() const
{
	return cameraParams_;
}

void videoTexture::setWidth(GLint width)
{
	if (isVideo_ || isStream_)
	{
		vidCapture_.set(cv::CAP_PROP_FRAME_WIDTH, width);
		width_ = static_cast<int>(vidCapture_.get(cv::CAP_PROP_FRAME_WIDTH));
	}


	if (isIDSPeak_)
	{
		workerIDSPtr_->setImageWidth(width);
		width_ = static_cast<int>(workerIDSPtr_->getImageWidth());
	}
}

void videoTexture::setHeight(GLint height)
{
	if (isVideo_ || isStream_)
	{
		vidCapture_.set(cv::CAP_PROP_FRAME_HEIGHT, height);
		height_ = static_cast<int>(vidCapture_.get(cv::CAP_PROP_FRAME_HEIGHT));
	}

	if (isIDSPeak_)
	{
		workerIDSPtr_->setImageHeight(height);
		height_ = static_cast<int>(workerIDSPtr_->getImageHeight());
	}
}

void videoTexture::captureLoop()
{
	float start{ 0 }, stop{ 0 };
	if (isStream_ || isVideo_)
	{
		while (m_running_)
		{
			FPStimer.startTimer();
			try
			{
				// checking
				if (!vidCapture_.isOpened())
				{
					isOpened_ = false;
					throw std::runtime_error("Error: Video stream can't be open");
				}
				cv::Mat frameVideo;
				bool isSuccessStream = vidCapture_.read(frameVideo);
				//while (inputVideo.grab()) {          //actualy grub/retrive doesnt give additional acceleration or any fitures. read() include both operations
				//    inputVideo.retrieve(image);
				//}
				if (!isSuccessStream && isVideo_)
				{
					vidCapture_.set(cv::CAP_PROP_POS_FRAMES, 0); // return to file begin
					isSuccessStream = vidCapture_.read(frameVideo);
				}
				if (!isSuccessStream)
				{
					isOpened_ = false;
					throw std::runtime_error("Error: Video stream can't be read or disconnect! Source: " + streamIndex_ + filePath_);
				}
				else
				{
					if (rotate_)
						cv::rotate(frameVideo, frameVideo, cv::ROTATE_180);

					currentValue_.push(std::move(frameVideo));
					if(false){
						Console::log() << std::setprecision(8);
						stop = static_cast<float>(glfwGetTime());
						Console::log() << "FPS :\t" << 1.0f / (stop - start) << "\tVideo " + (isVideo_ ? filePath_ : ("live: " + std::to_string(streamIndex_))) << std::endl;
						start = stop;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime)); // Sleep for decrise utilization CPU
				}
			}
			catch (const std::exception& e)
			{
				Console::red() << "Exception: " << e.what() << std::endl;
				return;
			}
			FPStimer.stopTimer();
		}
	}
	if (isIDSPeak_)
	{
		while (m_running_)
		{
			FPStimer.startTimer();
			try
			{
				if (!workerIDSPtr_->isRunning())
				{
					throw std::runtime_error("Error: IDS stream is not running!");
				}
				cv::Mat frameVideo;
				bool isSuccessStream = workerIDSPtr_->TryPopImage(frameVideo);

				if (!isSuccessStream)
				{
					throw std::runtime_error("Error: IDS stream can't be read or disconnect!");
				}
				else
				{
					if (rotate_)
						cv::rotate(frameVideo, frameVideo, cv::ROTATE_180);

					//currentValue_.push(std::move(frameVideo));
					currentValue_.push(std::move(frameVideo));	//create a copy instead of moving
					if (false) {
						Console::log() << std::setprecision(8);
						stop = static_cast<float>(glfwGetTime());
						Console::log() << "FPS :\t" << 1.0f / (stop - start) << "\tVideo IDS" << std::endl;
						start = stop;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime)); // Sleep for decrise utilization CPU
				}
			}
			catch (const std::exception& e)
			{
				Console::red() << "Exception: " << e.what() << std::endl;
				return;
			}
			FPStimer.stopTimer();
		}
	}
}
