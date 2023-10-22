#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include "stb_image.h"
#include "camera.h"
#include "shader.h"				//move it up if problem
#include "geometryObject.h"
#include "geometryData.h"
#include "aruco/ArucoProcessor.h"
#include "acquisitionworker.h"
#include "config.h"
#include "ArucoThreadWrapper.h"

using PrintInFrameCallback = std::function<void(const cv::Mat& frame, cv::Size frameSize)>;

class GLObject
{
public:

	GLObject(const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState, bool linePolygonMode = false);

	void setupShaderProgram(Shader* shaderProgPtr);

	void setupArUcoPtr(std::shared_ptr<ArucoThreadWrapper> arucoThreadWrapper)
	{
		this->arucoThreadWrapperPtr = arucoThreadWrapper;
	}

	// Open texture file and bind with object
	template<typename T>
		requires std::same_as<T, int> || std::same_as<T, std::string>
	inline void setupVideoTexture(const T & videoTexture, GLenum internalformat, GLenum format, bool rotate = false, bool isBackground = false, bool showOnMarker = false, std::shared_ptr<std::vector<int>> markerIds = nullptr, std::string cameraParams = nullptr)
	{
		if constexpr (std::is_same_v<T, int>) {
			this->isStream = true;
			this->streamIndex = videoTexture;
		}
		else if constexpr (std::is_same_v<T, std::string>) {
			this->isVideo = true;
			this->filePath = videoTexture;
		}
		//cv::VideoCapture vid_captureCamera
		this->vidCapture = cv::VideoCapture(videoTexture);

		//----------- Init ArUco and set resolution -----------
		if (this->isStream) //&& turn aruco flag
		{
			// ArUco init
			//*arucoProcessorPtrToPtr = std::make_shared<ArucoProcessor>(ArUcoMarkerLength, ArUcoDictionaryId, cameraParams, ArUcoShowRejected);
			//arucoProcessorPtr = *arucoProcessorPtrToPtr;
			arucoThreadWrapperPtr->initAruco(ArUcoMarkerLength, ArUcoDictionaryId, cameraParams, ArUcoShowRejected);
			this->vidCapture.set(cv::CAP_PROP_FRAME_WIDTH, arucoThreadWrapperPtr->GetFrameSize().width);
			this->vidCapture.set(cv::CAP_PROP_FRAME_HEIGHT, arucoThreadWrapperPtr->GetFrameSize().height);
			arucoThreadWrapperPtr->StartThread();
		}
		//------------------------------------------

		this->width = static_cast<int>(this->vidCapture.get(cv::CAP_PROP_FRAME_WIDTH));
		this->height = static_cast<int>(this->vidCapture.get(cv::CAP_PROP_FRAME_HEIGHT));

		this->internalformat = internalformat;
		this->format = format;
		this->rotate = rotate;
		this->isBackground = isBackground;
		this->showOnMarker = showOnMarker;
		this->markerIds = markerIds;

		//---------------------- video texture -------------------
		if (!this->vidCapture.isOpened())
		{
			std::cerr << "Error: Video stream can't be open! Source: " << videoTexture << std::endl;
		}
		else
		{
			cv::Mat frameVideo;
			//Initialize a boolean to check whether frames are present or not
			if (!this->vidCapture.read(frameVideo))
			{
				std::cerr << "Error: Video stream can't be read or disconnect! Source: " << videoTexture << std::endl;
			}
			else
			{
				std::cout << "Video stream opened successfully!" << std::endl;
				this->data = frameVideo.data;
				this->isOpened = true;
			}
		}
	}

	void setupIDSPeakTexture(const std::shared_ptr<AcquisitionWorker> workerPtr, GLenum internalformat, GLenum format, bool rotate = false, bool isBackground = false, bool showOnMarker = false, std::shared_ptr<std::vector<int>> markerIds = nullptr, std::string cameraParams = nullptr)
	{
		this->isIDSPeak = true;
		this->workerIDSPtr = workerPtr;

		//----------- Init ArUco and set resolution -----------
		if (this->isIDSPeak) //&& turn aruco flag
		{
			// ArUco init
			//*arucoProcessorPtrToPtr = std::make_shared<ArucoProcessor>(ArUcoMarkerLength, ArUcoDictionaryId, cameraParams, ArUcoShowRejected);
			//arucoProcessorPtr = *arucoProcessorPtrToPtr;
			arucoThreadWrapperPtr->initAruco(ArUcoMarkerLength, ArUcoDictionaryId, cameraParams, ArUcoShowRejected);
			arucoThreadWrapperPtr->StartThread();
		}
		//------------------------------------------

		this->width = static_cast<int>(this->workerIDSPtr->getImageWidth());
		this->height = static_cast<int>(this->workerIDSPtr->getImageHeight());

		this->internalformat = internalformat;
		this->format = format;
		this->rotate = rotate;
		this->isBackground = isBackground;
		this->showOnMarker = showOnMarker;
		this->markerIds = markerIds;

		cv::Mat frame;
		workerPtr->TryGetImage(frame);
		std::cout << "Video stream opened successfully!" << std::endl;
		this->data = frame.data;
		this->isOpened = true;
	}

	void setupImgTexture(const std::string& imgTexture, GLenum internalformat, GLenum format, bool rotate = false, bool isBackground = false, bool showOnMarker = false, std::shared_ptr<std::vector<int>> markerIds = nullptr);

	void renderObject(Camera& camera, PrintInFrameCallback printCallback);

	void drowObject(glm::mat4& viewMat, glm::mat4& projectionMat, bool background = false);
	~GLObject();
private:

	void processVideoStream(const PrintInFrameCallback& printCallback);
	void processIDSStream(const PrintInFrameCallback& printCallback);
	void processFrame(cv::Mat& frame, const PrintInFrameCallback& printCallback);
	void processImage();
	void renderMarkers(Camera& camera, glm::mat4& projection);
	void renderOnMarkers(glm::mat4& view, glm::mat4& projection);

	GeometryObject geometryObject;
	Shader* shader;
	//Textures property
	bool			isOpened = false;
	bool			isImg = false;
	bool			isVideo = false;
	bool			isStream = false;
	bool			isIDSPeak = false;
	bool			rotate = false;
	bool			isBackground = false;
	bool			showOnMarker = false;
	std::string		filePath;
	GLint			streamIndex = 0;
	GLint			width;
	GLint			height;
	GLint			nrChannels = 3;
	GLenum			internalformat;
	GLenum			format;
	cv::VideoCapture vidCapture;
	uchar*			data;
	std::shared_ptr<AcquisitionWorker> workerIDSPtr;
	std::shared_ptr<std::vector<int>> markerIds;

	//std::shared_ptr<ArucoProcessor> arucoProcessorPtr;
	//std::shared_ptr<ArucoProcessor>* arucoProcessorPtrToPtr;
	std::shared_ptr<ArucoThreadWrapper> arucoThreadWrapperPtr;
};

