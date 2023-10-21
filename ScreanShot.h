#pragma once
#include <filesystem>
#include <chrono>
#include <format>
#include "texture.h"

class ScreanShot
{
public:
	static void makeAndSave(int winId, const cv::Mat& image);

private:
	static std::string getTimeStr();
};

