#pragma once
#include <string>
#include <opencv2/opencv.hpp>

class WindowMenu {
public:
    WindowMenu();

    void F1Toggle();
    void F2Toggle();
    void F3Toggle();
    void F4Toggle();
    void F5Toggle();

    bool isMetricsShown() const;
    bool isArUcoShown() const;
    bool isCascadeDetectorShown() const;
    bool isYOLODetectorShown() const;

    void displayMenu(cv::Mat& frame);

private:
    bool helpShow_;
    bool metricsShow_;
    bool arUcoShow_;
    bool cascadeDetectorShow_;
    bool yoloDetectorShow_;
};

