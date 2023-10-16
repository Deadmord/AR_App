#include "ScreanShot.h"

void ScreanShot::makeAndSave(int winId, const cv::Mat& image)
{
	std::string dirName = "ScreanShots_Window-" + std::to_string(winId); // Directory name for current window
	try
	{
		//Try to create directory
		std::filesystem::create_directory(dirName);

		std::string fullPath = dirName + "/img-" + getTimeStr() + ".jpg";

		//Try to write screanshot
		cv::imwrite(fullPath, image);
		Console::green() << "Screan created at " << fullPath << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //Froze screan for 1s
	}
	catch (const std::exception& e)
	{
		Console::red() << "Exception: " << e.what() << std::endl;
		return;
	}
}

std::string ScreanShot::getTimeStr()
{
	auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	// Format time according teamplate
	struct std::tm timeinfo;
	localtime_s(&timeinfo, &currentTime);

	char buffer[20]; // Sufficient for the format "YYYY-MM-DD HH:MM:SS"
	strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H%M%S", &timeinfo);

	//std::string timeStr = std::format("{:%Y-%m-%d %H:%M:%S}", currentTime);
	return buffer;
}
