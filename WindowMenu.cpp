#include "WindowMenu.h"

WindowMenu::WindowMenu() : helpShow_(false),  metricsShow_(false), arUcoShow_(false), cascadeDetectorShow_(false), yoloDetectorShow_(false) {}

void WindowMenu::F1Toggle() {
    helpShow_ = !helpShow_;
}

void WindowMenu::F2Toggle() {
    metricsShow_ = !metricsShow_;
}

void WindowMenu::F3Toggle() {
    arUcoShow_ = !arUcoShow_;
}

void WindowMenu::F4Toggle() {
    cascadeDetectorShow_ = !cascadeDetectorShow_;
}

void WindowMenu::F5Toggle() {
    yoloDetectorShow_ = !yoloDetectorShow_;
}

bool WindowMenu::isMetricsShown() const {
    return metricsShow_;
}

bool WindowMenu::isArUcoShown() const {
    return arUcoShow_;
}

bool WindowMenu::isCascadeDetectorShown() const {
    return cascadeDetectorShow_;
}

bool WindowMenu::isYOLODetectorShown() const {
    return yoloDetectorShow_;
}

void WindowMenu::displayMenu(cv::Mat& frame) {
    std::string menu = "F2 - Speed metrics; F3 - ArUco markers; F4 - Cascade object detection; F5 - YOLO object detection.";
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 2;
    cv::Point textOrg(10, frame.rows - 20);
    cv::Scalar greenColor(0, 255, 0);
    cv::Scalar grayColor(128, 128, 128);

    if (metricsShow_) {
        cv::putText(frame, "F2 - Metrics", textOrg, fontFace, fontScale, greenColor, thickness, 8);
    }
    else {
        cv::putText(frame, "F2 - Metrics", textOrg, fontFace, fontScale, grayColor, thickness, 8);
    }

    if (arUcoShow_) {
        cv::putText(frame, "F3 - ArUco", textOrg + cv::Point(120,0), fontFace, fontScale, greenColor, thickness, 8);
    }
    else {
        cv::putText(frame, "F3 - ArUco", textOrg + cv::Point(120, 0), fontFace, fontScale, grayColor, thickness, 8);
    }

    if (cascadeDetectorShow_) {
        cv::putText(frame, "F4 - Cascade", textOrg + cv::Point(240, 0), fontFace, fontScale, greenColor, thickness, 8);
    }
    else {
        cv::putText(frame, "F4 - Cascade", textOrg + cv::Point(240, 0), fontFace, fontScale, grayColor, thickness, 8);
    }

    if (yoloDetectorShow_) {
        cv::putText(frame, "F5 - YOLO", textOrg + cv::Point(360, 0), fontFace, fontScale, greenColor, thickness, 8);
    }
    else {
        cv::putText(frame, "F5 - YOLO", textOrg + cv::Point(360, 0), fontFace, fontScale, grayColor, thickness, 8);
    }

    cv::putText(frame, "Space - scrShot", textOrg + cv::Point(470, 0), fontFace, fontScale, grayColor, thickness, 8);
}