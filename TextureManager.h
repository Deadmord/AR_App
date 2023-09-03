#pragma once

#include <glad/glad.h>
#include<string>
#include<iostream>
#include<opencv2/opencv.hpp>
#include "stb_image.h"
#include "aruco/ArucoProcessor.h"
#include "geometryObjects.h"
#include "shader.h"

class TextureManager
{
public:
	TextureManager() {}
	//~TextureManager(); 

	template<typename T>
	requires std::same_as<T, int> || std::same_as<T, std::string>
	inline void setupVideoTexture(GLuint index, const T& videoTexture, GLenum internalformat, GLenum format, ArUcoSettings& arUcoSettings, bool isBackground = false, bool showOnMarker = false, std::string cameraParams = nullptr)
	{
		if constexpr (std::is_same_v<T, int>) {
			textures[index].isStream = true;
			textures[index].streamIndex = videoTexture;
		}
		else if constexpr (std::is_same_v<T, std::string>) {
			textures[index].isVideo = true;
			textures[index].filePath = videoTexture;
		}
		//cv::VideoCapture vid_captureCamera
		textures[index].vidCapture = cv::VideoCapture(videoTexture);

		//----------- Init ArUco and set resolution -----------
		if (textures[index].isStream) //&& turn aruco flag
		{
			// ArUco init
			arucoProcessorPtr = std::make_unique<ArucoProcessor>(arUcoSettings.markerLength, arUcoSettings.dictionaryId, cameraParams, arUcoSettings.showRejected);

			textures[index].vidCapture.set(cv::CAP_PROP_FRAME_WIDTH, arucoProcessorPtr->getFrameSize().width);
			textures[index].vidCapture.set(cv::CAP_PROP_FRAME_HEIGHT, arucoProcessorPtr->getFrameSize().height);
		}
		//------------------------------------------

		textures[index].width = static_cast<int>(textures[index].vidCapture.get(cv::CAP_PROP_FRAME_WIDTH));
		textures[index].height = static_cast<int>(textures[index].vidCapture.get(cv::CAP_PROP_FRAME_HEIGHT));

		textures[index].internalformat = internalformat;
		textures[index].format = format;

		textures[index].isBackground = isBackground;
		textures[index].showOnMarker = showOnMarker;

		//---------------------- video texture -------------------
		if (!textures[index].vidCapture.isOpened())
		{
			std::cout << "Error: Video stream can't be open! Source: " << videoTexture << std::endl;
		}
		else
		{
			cv::Mat frameVideo;
			//Initialize a boolean to check whether frames are present or not
			if (!textures[index].vidCapture.read(frameVideo))
			{
				std::cout << "Error: Video stream can't be read or disconnect! Source: " << videoTexture << std::endl;
			}
			else
			{
				std::cout << "Video stream opened successfully!" << std::endl;
				textures[index].data = frameVideo.data;
				textures[index].isOpened = true;
			}
		}
	}

	void setupImgTexture(GLuint index, const std::string& imgTexture, GLenum internalformat, GLenum format, bool isBackground = false, bool showOnMarker = false);
	void addGeometryBuffers(GLsizei size);
	void setupGeometryObject(GLuint objIndex, const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState);
	void drawObject(GLsizei objIndex, glm::mat4& viewMat, glm::mat4& projectionMat, bool background);

	std::unique_ptr<ArucoProcessor> arucoProcessorPtr;
	GeometryObjects geometryObjects;
	std::vector<TextureData> textures;
	std::vector<Shader*> shaders;
	GLsizei objectListSize = 0;

};