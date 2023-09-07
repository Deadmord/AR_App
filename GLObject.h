#pragma once
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include "stb_image.h"
#include "shader.h"				//move it up if problem
#include "geometryObject.h"
#include "geometryData.h"
#include "aruco/ArucoProcessor.h"

class GLObject
{
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
// Vertex Array Object sould has next form: float[] {PosX, PosY, PosZ, ColR, ColG, ColB, TextX, TextY, ...}.
// Element Buffer Object should include indices of triangle vertices in the correct sequence.
	GLObject(const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState);

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
			arucoProcessorPtr = std::make_unique<ArucoProcessor>(ArUcoMarkerLength, ArUcoDictionaryId, cameraParams, ArUcoShowRejected);

			this->vidCapture.set(cv::CAP_PROP_FRAME_WIDTH, arucoProcessorPtr->getFrameSize().width);
			this->vidCapture.set(cv::CAP_PROP_FRAME_HEIGHT, arucoProcessorPtr->getFrameSize().height);
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
			std::cout << "Error: Video stream can't be open! Source: " << videoTexture << std::endl;
		}
		else
		{
			cv::Mat frameVideo;
			//Initialize a boolean to check whether frames are present or not
			if (!this->vidCapture.read(frameVideo))
			{
				std::cout << "Error: Video stream can't be read or disconnect! Source: " << videoTexture << std::endl;
			}
			else
			{
				std::cout << "Video stream opened successfully!" << std::endl;
				this->data = frameVideo.data;
				this->isOpened = true;
			}
		}
	}

	void setupImgTexture(const std::string& imgTexture, GLenum internalformat, GLenum format, bool isBackground = false, bool showOnMarker = false, std::shared_ptr<std::vector<int>> markerIds = nullptr);

	void setupShaderProgram(Shader* shaderProgPtr);

private:
	GeometryObject geometryObject;
	Shader* shader;

	//Textures property
	bool			isOpened = false;
	bool			isImg = false;
	bool			isVideo = false;
	bool			isStream = false;
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
	std::shared_ptr<std::vector<int>> markerIds;

	std::unique_ptr<ArucoProcessor> arucoProcessorPtr;
};

