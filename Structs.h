#pragma once

#include<iostream>
#include <opencv2/opencv.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct TextureData
{
	bool			isOpened = false;
	bool			isImg = false;
	bool			isVideo = false;
	bool			isStream = false;
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
	uchar* data;
};

struct MonitorData
{
	GLFWmonitor* monitor;
	int monitor_X, monitor_Y, monitor_Width, monitor_Height;
};

struct ArUcoSettings
{
	float markerLength;    // 0.035f;
	cv::aruco::PredefinedDictionaryType dictionaryId;
	std::string defaultCameraParams;
	std::string usbCameraParams;
	//std::string defaultCameraParams = "camera_params/camera_paramsUSB01.yml";
	//std::string usbCameraParams = "camera_params/camera_params03.yml";

	bool showRejected;
};

struct FrameTiming
{
	float startTime;
	float stopTime;
	float deltaTime;
	float FPS;
};

struct Markers
{
	std::vector<int> ids;
	std::vector<std::vector<cv::Point2f>> corners, rejected;
	std::vector<cv::Vec3d> rvecs;
	std::vector<cv::Vec3d> tvecs;
	std::vector<glm::mat4> viewMatrixes;
};

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

struct InitState
{
	glm::vec3	positions;
	glm::vec3	axisRotation;
	float		angle;
	float		speed;
	glm::vec3	colorMask;
};