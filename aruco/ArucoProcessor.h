#pragma once
//#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // For glm::value_ptr и glm::to_string
#include "aruco_utilities.hpp"

struct Markers
{
	std::vector<int> ids;
	std::vector<std::vector<cv::Point2f>> corners, rejected;
	std::vector<cv::Vec3d> rvecs;
	std::vector<cv::Vec3d> tvecs;
	std::vector<glm::mat4> viewMatrixes;
};

class ArucoProcessor
{
public:
	ArucoProcessor(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId = cv::aruco::DICT_6X6_250, std::string cameraParams = nullptr, bool showRejected = false);

	~ArucoProcessor() = default;
	ArucoProcessor(const ArucoProcessor&) = default;
	ArucoProcessor& operator=(const ArucoProcessor&) = default;

	bool detectMarkers(const cv::Mat& frame, cv::Mat& frameCopy);

	const Markers& getMarkers() const;

	const cv::Size& getFrameSize() const;

	const glm::mat4& getProjectionMat() const;

	const cv::Mat& getCameraMat() const;

	const cv::Mat& getDistortCoeff() const;

	const cv::Mat& getUndistortMap1() const;

	const cv::Mat& getUndistortMap2() const;

	const float& getFOV() const;

	const float& getFOVdeg() const;

private:
	glm::mat4 createViewMatrix(cv::Mat cameraMatrix, cv::Vec3d rvec, cv::Vec3d tvec);

	glm::mat4 createProjectionMatrix(float FOV, cv::Size frameSize, float nearPlane = 0.1f, float farPlane = 100.0f);

	void initializeObjPoints();

private:
	float markerLength;
	bool showRejected;
	bool estimatePose;
	cv::Size frameSize;
	cv::Mat camMatrix, distCoeffs;
	cv::Mat undistortionMap1, undistortionMap2;
	glm::mat4 projectionMatrix;
	cv::aruco::Dictionary dictionary;
	cv::aruco::DetectorParameters detectorParams;
	cv::aruco::ArucoDetector detector;

	std::shared_ptr<cv::Mat> objPoints;
	Markers markers;

	// Примерные значения для FOV, nearPlane и farPlane
	float FOV = 0.7854f;       // fielf of view in rad
	float FOV_Deg = 45.0f;
	const float nearPlane = 0.1f;  // Ближняя плоскость отсечения
	const float farPlane = 100.0f; // Дальняя плоскость отсечения
};

