#include "ArucoProcessor.h"

// constant
glm::mat4 INVERSE_MATRIX
	(1.0, 0.0, 0.0, 0.0,
	0.0, -1.0, 0.0, 0.0,
	0.0, 0.0, -1.0, 0.0,
	0.0, 0.0, 0.0, 1.0);

ArucoProcessor::ArucoProcessor(float markerLength, cv::aruco::PredefinedDictionaryType dictionaryId, std::string cameraParams, bool showRejected)
	:markerLength(markerLength), showRejected(showRejected), objPoints(std::make_unique<cv::Mat>(4, 1, CV_32FC3))
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
		FOV = 2.0f * std::atan2(0.5f * frameSize.height, focal_length_x);
		FOV_Deg = glm::degrees(FOV);

		//hardware way: FOV = SD - WD / FL,
		//��� FOV - ���� ������; SD - ������ �������; WD - ������� ����������; FL - �������� ���������� ����.

		projectionMatrix = createProjectionMatrix(FOV, frameSize, nearPlane, farPlane);
	}
	initializeObjPoints();
}

bool ArucoProcessor::detectMarkers(const cv::Mat& frame, cv::Mat& frameCopy)
{
	try {
		detector.detectMarkers(frame, markers.corners, markers.ids, markers.rejected);
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception Aruco: " << e.what() << std::endl;
	}

	//use to track IDs that have been discovered previously
	std::unordered_set<int> detectedMarkerIdsSet(detectedMarkerIds.begin(), detectedMarkerIds.end());
	for (int id : markers.ids) {
		if (!detectedMarkerIdsSet.count(id)) {
			detectedMarkerIds.push_back(id);
			detectedMarkerIdsSet.insert(id);
		}
	}

	sortDetectedMarkers(markers.ids, markers.corners);

	size_t  nMarkers = markers.corners.size();
	markers.rvecs.resize(nMarkers);
	markers.tvecs.resize(nMarkers);
	markers.viewMatrixes.resize(nMarkers);

	if (estimatePose && !markers.ids.empty()) {
		// Calculate pose for each marker
		for (size_t i = 0; i < nMarkers; i++) {
			solvePnP(*objPoints, markers.corners.at(i), camMatrix, distCoeffs, markers.rvecs.at(i), markers.tvecs.at(i));
			cv::Vec3d averaged_rvec, averaged_tvec;
			addValueAndAverage(markers.rvecs.at(i), markers.tvecs.at(i), averaged_rvec, averaged_tvec);

			markers.rvecs.at(i) = averaged_rvec;
			markers.tvecs.at(i) = averaged_tvec;

			markers.viewMatrixes.at(i) = createViewMatrix(camMatrix, averaged_rvec, averaged_tvec);
		}
	}

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

glm::mat4 ArucoProcessor::createViewMatrix(cv::Mat& cameraMatrix, cv::Vec3d& rvec, cv::Vec3d& tvec) {
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

	//std::cout << "Transformation Matrix:\n" << glm::to_string(glmTransformationMatrix) << "\n\n";    //Print matrix example
}

inline glm::mat4 ArucoProcessor::createProjectionMatrix(float FOV, cv::Size& frameSize, float nearPlane, float farPlane)
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

void ArucoProcessor::addValueAndAverage(const cv::Vec3d& new_rvec, const cv::Vec3d& new_tvec, cv::Vec3d& averaged_rvec, cv::Vec3d& averaged_tvec)
{
	//add new values
	rvecs_history.push_back(new_rvec);
	tvecs_history.push_back(new_tvec);

	//delete old values if there are more than N of them
	if (rvecs_history.size() > N) 
	{
		rvecs_history.pop_front();
		tvecs_history.pop_front();
	}

	//calculate average values
	averaged_rvec = std::accumulate(rvecs_history.begin(), rvecs_history.end(), cv::Vec3d(0, 0, 0)) / double(rvecs_history.size());
	averaged_tvec = std::accumulate(tvecs_history.begin(), tvecs_history.end(), cv::Vec3d(0, 0, 0)) / double(tvecs_history.size());
}

void ArucoProcessor::sortDetectedMarkers(std::vector<int>& ids, std::vector<std::vector<cv::Point2f>>& corners)
{
	//use to map marker IDs to their angles to quickly sort markers in the order they were originally discovered
	std::unordered_map<int, std::vector<cv::Point2f>> idToCornersMap;
	for (size_t i = 0; i < ids.size(); ++i) {
		idToCornersMap[ids[i]] = corners[i];
	}

	size_t index = 0;
	for (int detectedId : detectedMarkerIds) {
		if (idToCornersMap.count(detectedId)) {
			ids[index] = detectedId;
			corners[index] = idToCornersMap[detectedId];
			++index;
		}
	}
}