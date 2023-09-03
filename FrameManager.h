#pragma once

#include "TextureManager.h"
#include "InputHandler.h"
#include <opencv2/opencv.hpp>
#include "RTCounter.h"

class FrameManager
{
public:

	FrameManager();

	void showInFrame(const cv::Mat& frame, cv::Size WindSize, cv::Size frameSize, float FPS);
	void renderFrame(float deltaTime, TextureManager& textureManager, InputHandler& inputHandler, int wndID, GLFWwindow* window, glm::vec4 bgColor, GLuint WinWidth, GLuint WinHeight);

};

