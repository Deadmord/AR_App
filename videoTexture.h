#pragma once
#include "texture.h"
#include "acquisitionworker.h"
#include "RTCounter.h"


class videoTexture : public texture
{
public:
	//videoTexture() : m_running_(false)
	//{}

	// Open texture file and bind with object
	template<typename T>
		requires std::same_as<T, int> || std::same_as<T, std::string>
	inline videoTexture(const T& videoTexture, GLenum internalformat, GLenum format, bool rotate = false, bool isBackground = false, bool showOnMarker = false, std::shared_ptr<std::vector<int>> markerIds = nullptr, std::string cameraParams = nullptr)
		: m_running_(false)
	{
		if constexpr (std::is_same_v<T, int>) {
			isStream_ = true;
			streamIndex_ = videoTexture;
		}
		else if constexpr (std::is_same_v<T, std::string>) {
			isVideo_ = true;
			filePath_ = videoTexture;
		}
		//cv::VideoCapture vid_captureCamera
		vidCapture_ = cv::VideoCapture(videoTexture);

		//------------------------------------------

		width_ = static_cast<int>(vidCapture_.get(cv::CAP_PROP_FRAME_WIDTH));
		height_ = static_cast<int>(vidCapture_.get(cv::CAP_PROP_FRAME_HEIGHT));

		internalformat_ = internalformat;
		format_ = format;
		rotate_ = rotate;
		isBackground_ = isBackground;
		showOnMarker_ = showOnMarker;
		markerIds_ = markerIds;
		cameraParams_ = cameraParams;

		//---------------------- video texture -------------------
		if (!vidCapture_.isOpened())
		{
			Console::red() << "Error: Video stream can't be open!" << "\tVideoPath: " << videoTexture << std::endl;
		}
		else
		{
			cv::Mat frameVideo;
			//Initialize a boolean to check whether frames are present or not
			if (!vidCapture_.read(frameVideo))
			{
				Console::red() << "Error: Video stream can't be read or disconnect!" << "\tVideoPath: " << videoTexture << std::endl;
			}
			else
			{
				Console::green() << "Video stream opened successfully!" << "\tVideoPath: " << videoTexture << "\tTexturePtr: " << this << std::endl;
				data_ = frameVideo.data;
				isOpened_ = true;

				double frameRate = vidCapture_.get(cv::CAP_PROP_FPS); // Get the current frame rate
				Console::log() << "Frame Rate: " << frameRate << " fps" << std::endl;

				//m_running_ = true;
				////m_captureLoopThread_ = std::make_shared<std::thread>(&GLObject::captureLoop, this);
				//m_captureLoopThread_ = std::make_shared<std::thread>([this]() {captureLoop(); });
				start();
				Console::green() << "VideoCaptureLoopThread" << "\tVideoPath: " << videoTexture << "\tLoopThread: " << m_captureLoopThread_->get_id() << std::endl;
			}
		}
	}

	videoTexture(const std::shared_ptr<AcquisitionWorker> workerPtr, GLenum internalformat, GLenum format, bool rotate = false, bool isBackground = false, bool showOnMarker = false, std::shared_ptr<std::vector<int>> markerIds = nullptr, std::string cameraParams = nullptr)
	{
		isIDSPeak_ = true;
		workerIDSPtr_ = workerPtr;

		//------------------------------------------

		width_ = static_cast<int>(workerIDSPtr_->getImageWidth());
		height_ = static_cast<int>(workerIDSPtr_->getImageHeight());

		internalformat_ = internalformat;
		format_ = format;
		rotate_ = rotate;
		isBackground_ = isBackground;
		showOnMarker_ = showOnMarker;
		markerIds_ = markerIds;
		cameraParams_ = cameraParams;

		cv::Mat frame;
		if (workerPtr == nullptr || !workerPtr->TryPopImage(frame))
		{
			Console::red() << "Error: IDS Video stream can't be read or disconnect!" << "\tIDSPtr: " << workerPtr << "\tTexturePtr: " << this << std::endl;
		}
		else
		{
			Console::green() << "IDS Video stream opened successfully!" << "\tIDSPtr: " << workerPtr << "\tTexturePtr: " << this << std::endl;
			data_ = frame.data;
			isOpened_ = true;

			start();
			Console::green() << "IDS CaptureLoopThread" << "\tIDSPtr: " << workerPtr << "\tLoopThread: " << m_captureLoopThread_->get_id() << std::endl;
		}

	}

	~videoTexture();

	void start() override;

	void stop() override;

	uchar* getData() override;

	float getFPS() override;

	const std::string& getCameraParams() const;

	void setWidth(GLint width) override;

	void setHeight(GLint height) override;

private:

	void captureLoop();

	void OpenDevice();
	void CloseDevice();


	std::string		filePath_;
	GLint			streamIndex_ = 0;
	std::shared_ptr<AcquisitionWorker> workerIDSPtr_;
	std::string		cameraParams_;

	cv::VideoCapture vidCapture_;

	bool			m_running_;
	std::shared_ptr<std::thread> m_captureLoopThread_;
	RTCounter FPStimer;

};

