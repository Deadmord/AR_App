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
	ArucoProcessor(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId = cv::aruco::DICT_6X6_250, std::string cameraParams = nullptr, bool showRejected = false)
		:markerLength(markerLength), cameraParams(cameraParams), showRejected(showRejected), objPoints(std::make_unique<cv::Mat>(4, 1, CV_32FC3))
	{
		//override cornerRefinementMethod read from config file
		detectorParams.cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX;
		std::cout << "Corner refinement method (0: None, 1: Subpixel, 2:contour, 3: AprilTag 2): " << (int)detectorParams.cornerRefinementMethod << std::endl;		
		dictionary = cv::aruco::getPredefinedDictionary(dictionaryId);
		detector = cv::aruco::ArucoDetector(dictionary, detectorParams);

		estimatePose = !cameraParams.empty();
		if (estimatePose) {
			bool readOk = readCameraParameters(cameraParams, frameSize, camMatrix, distCoeffs);
			if (!readOk) {
				std::cerr << "Invalid camera file" << std::endl;
				throw std::runtime_error("Invalid camera parameters file");   //Refactoring !!! Add tray/catch
			}

			// Calculate the distortion correction for the camera matrix and get the undistortion matrices
			cv::Mat newCamMatrix;
			cv::initUndistortRectifyMap(camMatrix, distCoeffs, cv::Mat(), newCamMatrix, frameSize, CV_16SC2, undistortionMap1, undistortionMap2);

			// Getting focal lenght from camera calibration parameters
			double focal_length_x = camMatrix.at<double>(0, 0); // also could be camera_matrix.at<double>(1, 1); for f_y

			// Calculate Field of View in radians and degrees
			FOV = 2.0 * std::atan2(0.5 * frameSize.height, focal_length_x);
			FOV_Deg = glm::degrees(FOV);
			
			//hardware way: FOV = SD - WD / FL,
			//где FOV - поле зрения; SD - размер датчика; WD - рабочее расстояние; FL - Фокусное расстояние линз.

		}
		initializeObjPoints();
	}

	~ArucoProcessor() = default;
	ArucoProcessor(const ArucoProcessor&) = default;
	ArucoProcessor& operator=(const ArucoProcessor&) = default;

	bool detectMarkers(const cv::Mat& frame, cv::Mat& frameCopy)
	{
		//cv::Mat frameCopy;

		double tick = (double)cv::getTickCount();

		//std::vector< int > ids;
		//std::vector< std::vector< cv::Point2f > > corners, rejected;

		// detect markers and estimate pose
		detector.detectMarkers(frame, markers.corners, markers.ids, markers.rejected);

		size_t  nMarkers = markers.corners.size();
		markers.rvecs.resize(nMarkers); 
		markers.tvecs.resize(nMarkers);
		markers.viewMatrixes.resize(nMarkers);

		if (estimatePose && !markers.ids.empty()) {
			// Calculate pose for each marker
			for (size_t i = 0; i < nMarkers; i++) {
				solvePnP(*objPoints, markers.corners.at(i), camMatrix, distCoeffs, markers.rvecs.at(i), markers.tvecs.at(i));
				markers.viewMatrixes.at(i) = createViewMatrix(camMatrix, markers.rvecs.at(i), markers.tvecs.at(i), frameSize.width, frameSize.height);
			}
		}

		//double currentTime = ((double)cv::getTickCount() - tick) / cv::getTickFrequency();
		//totalTime += currentTime;
		//totalIterations++;
		//if (totalIterations % 30 == 0) {
		//	cout << "Detection Time = " << currentTime * 1000 << " ms "
		//		<< "(Mean = " << 1000 * totalTime / double(totalIterations) << " ms)" << endl;
		//}

		// draw results
		frame.copyTo(frameCopy);
		if (!markers.ids.empty()) {
			cv::aruco::drawDetectedMarkers(frameCopy, markers.corners, markers.ids);

			if (estimatePose) {
				for (unsigned int i = 0; i < markers.ids.size(); i++)
					cv::drawFrameAxes(frameCopy, camMatrix, distCoeffs, markers.rvecs[i], markers.tvecs[i], markerLength * 1.0f, 2);
			}
		}

		if (showRejected && !markers.rejected.empty())
			cv::aruco::drawDetectedMarkers(frameCopy, markers.rejected, cv::noArray(), cv::Scalar(100, 0, 255));

		return !markers.ids.empty();
	}

	const Markers& getMarkers() const
	{
		return markers;
	}

	const cv::Size& getFrameSize() const
	{
		return frameSize;
	}

	const cv::Mat& getCameraMat() const
	{
		return camMatrix;
	}

	const cv::Mat& getDistortCoeff() const
	{
		return distCoeffs;
	}

	const cv::Mat& getUndistortMap1() const
	{
		return undistortionMap1;
	}

	const cv::Mat& getUndistortMap2() const
	{
		return undistortionMap2;
	}

	const float& getFOV() const
	{
		return FOV;
	}

	const float& getFOVdeg() const
	{
		return FOV_Deg;
	}

private:
	glm::mat4 createViewMatrix(cv::Mat cameraMatrix, cv::Vec3d rvec, cv::Vec3d tvec, int width, int height) {
		// Convert rotation vector to rotation matrix
		cv::Mat rotationMatrix;
		cv::Rodrigues(rvec, rotationMatrix);
		std::cout << "Rotation Matrix:\n" << rotationMatrix << std::endl;

		// Create a 4x4 transformation matrix by combining rotation and translation
		cv::Mat transformationMatrix = cv::Mat::eye(4, 4, CV_64F); // Identity matrix
		rotationMatrix.copyTo(transformationMatrix(cv::Rect(0, 0, 3, 3))); // Copy rotation part

		// Copy translation vector into the transformation matrix
		transformationMatrix.at<double>(0, 3) = tvec[0];
		transformationMatrix.at<double>(1, 3) = tvec[1];
		transformationMatrix.at<double>(2, 3) = tvec[2];
		//rotationMatrix.copyTo(transformationMatrix.rowRange(0, 3).colRange(0, 3));	
		//cv::Mat translationMat = (cv::Mat_<double>(3, 1) << tvec[0], tvec[1], tvec[2]);
		//translationMat.copyTo(transformationMatrix.rowRange(0, 3).col(3));
		std::cout << "Transformation Matrix:\n" << transformationMatrix << std::endl;


		//------------ my eperiments -----------
		//glm::mat4 glmTransformationMatrix;
		glm::mat4 glmTransformationMatrix = glm::make_mat4x4(transformationMatrix.ptr<double>());
		//memcpy(glm::value_ptr(glmTransformationMatrix), transformationMatrix.data, sizeof(double) * 16);
		std::cout << "view:\n" << glm::to_string(glmTransformationMatrix) << "\n\n";    //убрать

		// constant
		glm::mat4 INVERSE_MATRIX	(1.0, 0.0, 0.0, 0.0,
									0.0, -1.0, 0.0, 0.0,
									0.0, 0.0, -1.0, 0.0,
									0.0, 0.0, 0.0, 1.0);
		glmTransformationMatrix = glmTransformationMatrix * INVERSE_MATRIX;
		glmTransformationMatrix = glm::transpose(glmTransformationMatrix);

		return glmTransformationMatrix;
		//-------------------------------------


		// Create OpenGL-style projection matrix
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(FOV), float(width) / float(height), nearPlane, farPlane);

		return projectionMatrix;
	}

	void initializeObjPoints() {
		// Set coordinate system
		(*objPoints).ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-markerLength / 2.f, markerLength / 2.f, 0);
		(*objPoints).ptr<cv::Vec3f>(0)[1] = cv::Vec3f(markerLength / 2.f, markerLength / 2.f, 0);
		(*objPoints).ptr<cv::Vec3f>(0)[2] = cv::Vec3f(markerLength / 2.f, -markerLength / 2.f, 0);
		(*objPoints).ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-markerLength / 2.f, -markerLength / 2.f, 0);
	}

private:
	float markerLength;
	cv::aruco::Dictionary dictionary;
	bool estimatePose;
	std::string cameraParams;				//Refactoring можно убрать строку с адресом фаила параметров т.к. он не нужен после чтения этих параметров
	cv::Mat camMatrix, distCoeffs;
	cv::Mat undistortionMap1, undistortionMap2;
	bool showRejected;

	cv::aruco::DetectorParameters detectorParams;
	cv::aruco::ArucoDetector detector;

	//cv::Mat objPoints = cv::Mat(4, 1, CV_32FC3);
	std::unique_ptr<cv::Mat> objPoints;
	Markers markers;
	cv::Size frameSize;
	// Примерные значения для FOV, nearPlane и farPlane
	float FOV = 0.0f;       // Угол обзора в градусах
	float FOV_Deg = 0.0f;
	const float nearPlane = 0.1f;  // Ближняя плоскость отсечения
	const float farPlane = 100.0f; // Дальняя плоскость отсечения
};

