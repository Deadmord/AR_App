#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // For glm::value_ptr è glm::to_string

//*********************** Settings ***********************

// Window Settings
constexpr int		WINDOW_PANEL_HEIGHT = 0;				// 30px for window panel
constexpr glm::vec4 BG_CLR(0.2f, 0.3f, 0.3f, 1.0f);		//default bg color
constexpr glm::vec3 camInitPosition(5.0f, 4.0f, 20.0f);	//Camera start position

// -------------- Windows background color ----------------
constexpr glm::vec4 BG_CLR_W1 = glm::vec4(0.2f, 0.3f, 0.2f, 1.0f);	//window 1 
constexpr glm::vec4 BG_CLR_W2 = glm::vec4(0.2f, 0.2f, 0.3f, 1.0f);	//window 2 
constexpr glm::vec4 BG_CLR_W3 = glm::vec4(0.3f, 0.2f, 0.2f, 1.0f);	//window 3 
constexpr glm::vec4 BG_CLR_W4 = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);	//window 4 

// ArUco settings
extern std::string defaultCameraParams;
extern std::string usbCam00Params;
extern std::string usbCam01Params;
extern std::string usbCam02Params;
extern std::string usbCam03Params;

constexpr float ArUcoMarkerLength = 1.0f;    // 0.035f;s
constexpr cv::aruco::PredefinedDictionaryType ArUcoDictionaryId = cv::aruco::DICT_6X6_250;
constexpr bool ArUcoShowRejected = true;

// ---------------------- Cameras ------------------------
constexpr int defaultCamera = 3;
constexpr int usbCamera_1 = 0;
constexpr int usbCamera_2 = 1;
constexpr int usbCamera_3 = 2;


// Settings
extern int g_setting1;
extern double g_setting2;

// CONSTANTS
constexpr int k_constant1 = 42;
constexpr double k_constant2 = 3.14;

// Global var
extern int g_globalVar;

#endif // CONFIG_H

