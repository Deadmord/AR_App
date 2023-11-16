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
std::list<std::string> cascadeModelList{ 
	"./Cascade/haarcascades/haarcascade_frontalface_default.xml",
	"./Cascade/haarcascades/haarcascade_upperbody.xml",
	"./Cascade/haarcascades/haarcascade_lowerbody.xml",
	"./Cascade/haarcascades/haarcascade_fullbody.xml"
};

// YOLO detector
std::string classList = "./YOLO/YOLOv5_classes/coco.names";
std::string YOLOModel = "./YOLO/YOLOv5_models/yolov5s6.onnx";