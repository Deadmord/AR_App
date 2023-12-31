#include "ArucoProcessor.h"

// constant
glm::mat4 INVERSE_MATRIX
	(1.0, 0.0, 0.0, 0.0,
	0.0, -1.0, 0.0, 0.0,
	0.0, 0.0, -1.0, 0.0,
	0.0, 0.0, 0.0, 1.0);

ArucoProcessor::ArucoProcessor(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId, std::string cameraParams, bool showRejected)
	:markerLength(markerLength), showRejected(showRejected), objPoints(std::make_unique<cv::Mat>(4, 1, CV_32FC3)), positionSmother(1.0f, 1.0f, 1, 5)
{
	try
	{
		Console::yellow() << "ArucoProcessor created: " << this << std::endl;
		//override cornerRefinementMethod read from config file
		detectorParams.cornerRefinementMethod = cv::aruco::CORNER_REFINE_CONTOUR;
		Console::log() << "Corner refinement method (0: None, 1: Subpixel, 2:contour, 3: AprilTag 2): " << (int)detectorParams.cornerRefinementMethod << std::endl;
		dictionary = cv::aruco::getPredefinedDictionary(dictionaryId);
		detector = cv::aruco::ArucoDetector(dictionary, detectorParams);

		estimatePose = !cameraParams.empty();
		if (estimatePose) {
			bool readOk = readCameraParameters(cameraParams, frameSize, camMatrix, distCoeffs);
			if (!readOk) {
				Console::red() << "Invalid camera file" << std::endl;
				throw std::runtime_error("Invalid camera parameters file");   //Refactoring !!! Add tray/catch
			}

			// Calculate the distortion correction for the camera matrix and get the undistortion matrices
			cv::Mat newCamMatrix;
			cv::initUndistortRectifyMap(camMatrix, distCoeffs, cv::Mat(), newCamMatrix, frameSize, CV_16SC2, undistortionMap1, undistortionMap2);

			// Getting focal lenght from camera calibration parameters
			double focal_length_x = camMatrix.at<double>(0, 0); // also could be camera_matrix.at<double>(1, 1); for f_y

			// Calculate Field of View in radians and degrees
			FOV = 2.0f * static_cast<float>(std::atan2(0.5f * frameSize.height, focal_length_x));
			FOV_Deg = glm::degrees(FOV);

			//hardware way: FOV = SD - WD / FL,
			//��� FOV - ���� ������; SD - ������ �������; WD - ������� ����������; FL - �������� ���������� ����.

			projectionMatrix = createProjectionMatrix(FOV, frameSize, nearPlane, farPlane);
		}
		initializeObjPoints();
	}
	catch (const std::exception& e)
	{
		Console::red() << "ArucoProcessor::ArucoProcessor exception: " << e.what() << std::endl;
	}
}

ArucoProcessor::~ArucoProcessor()
{
	Console::yellow() << "ArucoProcessor deleted: " << this << std::endl;
}

bool ArucoProcessor::detectMarkers(const cv::Mat& frame, cv::Mat& frameCopy)
{
	// detect markers and estimate pose
	try {
		detector.detectMarkers(std::move(frame), markers.corners, markers.ids, markers.rejected);
		size_t  nMarkers = markers.corners.size();
		markers.rvecs.resize(nMarkers);
		markers.tvecs.resize(nMarkers);
		markers.viewMatrixes.resize(nMarkers);

		if (estimatePose && !markers.ids.empty()) {
			// Calculate pose for each marker
			for (size_t i = 0; i < nMarkers; i++) {
				solvePnP(*objPoints, markers.corners.at(i), camMatrix, distCoeffs, markers.rvecs.at(i), markers.tvecs.at(i));
			}
		}

		positionSmother.processMarkers(markers);

		if (!markers.ids.empty()) {
			size_t  nMarkers = markers.corners.size();
			// Calculate View Matrix for each marker
			for (size_t i = 0; i < nMarkers; i++) {
				markers.viewMatrixes.at(i) = createViewMatrix(markers.rvecs.at(i), markers.tvecs.at(i));
			}
		}

		// draw results
		if (&frame != &frameCopy)
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
	catch (const std::exception& e)
	{
		Console::red() << "Exception Aruco: " << e.what() << std::endl;
		return false;
	}
}

const Markers& ArucoProcessor::getMarkers() const
{
	return markers;
}

const cv::Size& ArucoProcessor::getFrameSize() const
{
	return frameSize;
}

const glm::mat4& ArucoProcessor::getProjectionMat() const
{
	return projectionMatrix;
}

const cv::Mat& ArucoProcessor::getCameraMat() const
{
	return camMatrix;
}

const cv::Mat& ArucoProcessor::getDistortCoeff() const
{
	return distCoeffs;
}

const cv::Mat& ArucoProcessor::getUndistortMap1() const
{
	return undistortionMap1;
}

const cv::Mat& ArucoProcessor::getUndistortMap2() const
{
	return undistortionMap2;
}

const float& ArucoProcessor::getFOV() const
{
	return FOV;
}

const float& ArucoProcessor::getFOVdeg() const
{
	return FOV_Deg;
}

glm::mat4 ArucoProcessor::createViewMatrix(const cv::Vec3d& rvec, const cv::Vec3d& tvec) {
	// Convert rotation vector to rotation matrix
	cv::Mat rotationMatrix;
	cv::Rodrigues(rvec, rotationMatrix);

	// Create a 4x4 transformation matrix by combining rotation and translation
	cv::Mat transformationMatrix = cv::Mat::eye(4, 4, CV_64F); // Identity matrix
	rotationMatrix.copyTo(transformationMatrix(cv::Rect(0, 0, 3, 3))); // Copy rotation part

	// Copy translation vector into the transformation matrix
	transformationMatrix.at<double>(0, 3) = tvec[0];
	transformationMatrix.at<double>(1, 3) = tvec[1];
	transformationMatrix.at<double>(2, 3) = tvec[2];

	// Transform to glm matrix
	glm::mat4 glmTransformationMatrix = glm::make_mat4x4(transformationMatrix.ptr<double>());
	glmTransformationMatrix = glmTransformationMatrix * INVERSE_MATRIX;
	glmTransformationMatrix = glm::transpose(glmTransformationMatrix);

	return glmTransformationMatrix;

	//Console::log() << "Transformation Matrix:\n" << glm::to_string(glmTransformationMatrix) << std::endl << std::endl;    //Print matrix example
}

inline glm::mat4 ArucoProcessor::createProjectionMatrix(float FOV, cv::Size frameSize, float nearPlane, float farPlane)
{
	// Create OpenGL-style projection matrix
	glm::mat4 glmProjectionMatrix = glm::perspective(FOV, float(frameSize.width) / float(frameSize.height), nearPlane, farPlane);

	return glmProjectionMatrix;
}

void ArucoProcessor::initializeObjPoints() {
	// Set coordinate system
	(*objPoints).ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-markerLength / 2.f, markerLength / 2.f, 0);
	(*objPoints).ptr<cv::Vec3f>(0)[1] = cv::Vec3f(markerLength / 2.f, markerLength / 2.f, 0);
	(*objPoints).ptr<cv::Vec3f>(0)[2] = cv::Vec3f(markerLength / 2.f, -markerLength / 2.f, 0);
	(*objPoints).ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-markerLength / 2.f, -markerLength / 2.f, 0);
}
