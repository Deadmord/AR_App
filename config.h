#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // For glm::value_ptr � glm::to_string

// ArUco settings
extern std::string defaultCameraParams;
extern std::string usbCam00Params;
extern std::string usbCam01Params;
extern std::string usbCam02Params;
extern std::string usbCam03Params;

constexpr float ArUcoMarkerLength = 1.0f;    // 0.035f;s
constexpr cv::aruco::PredefinedDictionaryType ArUcoDictionaryId = cv::aruco::DICT_6X6_250;
constexpr bool ArUcoShowRejected = true;

// Window Settings
constexpr int		WINDOW_PANEL_HEIGHT = 0;				// 30px for window panel
constexpr glm::vec4 BG_CLR(0.2f, 0.3f, 0.3f, 1.0f);		//default bg color
constexpr glm::vec3 camInitPosition(5.0f, 4.0f, 20.0f);	//Camera start position


// Settings
extern int g_setting1;
extern double g_setting2;

// CONSTANTS
constexpr int k_constant1 = 42;
constexpr double k_constant2 = 3.14;

// Global var
extern int g_globalVar;

#endif // CONFIG_H

