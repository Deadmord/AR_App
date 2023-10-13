#include "videoTexture.h"

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

const std::string& videoTexture::getCameraParams() const
{
	return cameraParams_;
}

void videoTexture::setWidth(GLint width)
{
	if (isVideo_ || isStream_)
		vidCapture_.set(cv::CAP_PROP_FRAME_WIDTH, width);

	if (isIDSPeak_) //реализовать изменение ширины для IDS Peak
		Console::red() << "Error: The setWidth method for IDS video has not been implemented." << std::endl;
}

void videoTexture::setHeight(GLint height)
{
	if (isVideo_ || isStream_)
		vidCapture_.set(cv::CAP_PROP_FRAME_HEIGHT, height);

	if (isIDSPeak_) //реализовать изменение высоты для IDS Peak
		Console::red() << "Error: The setHeight method for IDS video has not been implemented." << std::endl;
}

void videoTexture::captureLoop()
{
	if (isStream_ || isVideo_)
	{
		while (m_running_)
		{
			try
			{
				// checking
				if (!vidCapture_.isOpened())		//может быть заменить на textures[index].isOpened ?
				{
					isOpened_ = false;
					throw std::runtime_error("Error: Video stream can't be open");
				}
				cv::Mat frameVideo;
				bool isSuccessStream = vidCapture_.read(frameVideo);
				//while (inputVideo.grab()) {           использовать для асинхронного захвата видео кадра, наверное лучше разместить в конце метода и сделать исинхронной чтобы выполнялась пока обрабатывается остальные потоки.
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

					//calc and apply distortion correction, very heavy hendling!!!
					//cv::Mat undistortedFrame;
					//cv::undistort(frameVideo, undistortedFrame, arucoProcessorPtr->getCameraMat(), arucoProcessorPtr->getDistortCoeff());

					//only apply distortion maps, mach more faster!!!
					//cv::Mat undistortedFrame;
					//cv::remap(frameVideo, undistortedFrame, arucoProcessorPtr->getUndistortMap1(), arucoProcessorPtr->getUndistortMap2(), cv::INTER_LINEAR);

					//check stream videoframe for aruco markers
					//if (textures[index].isStream && arucoProcessorPtr->detectMarkers(frameVideo, frameVideoAruco))
					//{
					//    glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, frameVideoAruco.data);
					//}
					//else
					//glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, frameVideo.data);

					currentValue_.push(std::move(frameVideo));
				}
			}
			catch (const std::exception& e)
			{
				Console::red() << "Exception: " << e.what() << std::endl;
				return;
			}
		}
	}
	if (isIDSPeak_)
	{
		while (m_running_)
		{
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

					//calc and apply distortion correction, very heavy hendling!!!
					//cv::Mat undistortedFrame;
					//cv::undistort(frameVideo, undistortedFrame, arucoProcessorPtr->getCameraMat(), arucoProcessorPtr->getDistortCoeff());

					//only apply distortion maps, mach more faster!!!
					//cv::Mat undistortedFrame;
					//cv::remap(frameVideo, undistortedFrame, arucoProcessorPtr->getUndistortMap1(), arucoProcessorPtr->getUndistortMap2(), cv::INTER_LINEAR);

					//check stream videoframe for aruco markers
					//if (textures[index].isStream && arucoProcessorPtr->detectMarkers(frameVideo, frameVideoAruco))
					//{
					//    glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, frameVideoAruco.data);
					//}
					//else
					//glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, frameVideo.data);

					currentValue_.push(std::move(frameVideo));
				}
			}
			catch (const std::exception& e)
			{
				Console::red() << "Exception: " << e.what() << std::endl;
				return;
			}
		}
	}
}
