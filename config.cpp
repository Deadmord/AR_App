#include "config.h"

// ArUco Settings
std::string defaultCameraParams = "camera_params/camera_params03.yml";
std::string usbCam00Params = "camera_params/camera_paramsUSB01.yml";
std::string usbCam01Params = "camera_params/camera_paramsUSBsetCam02new.yml";
std::string usbCam02Params = "camera_params/camera_paramsUSBsetCam02new.yml";
std::string usbCam03Params = "camera_params/camera_paramsUSBsetCam02new.yml";
std::string idsCamParams = "camera_params/camera_paramsIDS_U3-3566XLE_med.yml";
//std::string defaultCameraParams = "camera_params/camera_paramsUSB01.yml";
//std::string usbCameraParams = "camera_params/camera_params03.yml";

// Binning IDS Cam
std::string selector = "Sensor";
int horizontal = 2;
int vertical = 2;

// Cascade detector
std::list<std::string> cascadeModelsList{ 
	"C:/src/opencv_src/opencv/data/haarcascades/haarcascade_frontalface_default.xml",
	"C:/src/opencv_src/opencv/data/haarcascades/haarcascade_upperbody.xml",
	"C:/src/opencv_src/opencv/data/haarcascades/haarcascade_lowerbody.xml",
	"C:/src/opencv_src/opencv/data/haarcascades/haarcascade_fullbody.xml"
};