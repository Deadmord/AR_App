#include "FrameAnalyzer.h"

FrameAnalyzer::FrameAnalyzer() {}

void FrameAnalyzer::showInFrame(const cv::Mat& frame, cv::Size WindSize, cv::Size frameSize, float FPS) {
    std::ostringstream vector_to_marker;

    vector_to_marker.str(std::string());
    vector_to_marker << std::setprecision(4) << "WindwRes: " << std::setw(2) << WindSize.width << " x " << WindSize.height;
    cv::putText(frame, vector_to_marker.str(), cv::Point(10, 25), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(32, 32, 240), 2);

    vector_to_marker.str(std::string());
    vector_to_marker << std::setprecision(4) << "FrameRes: " << std::setw(4) << frameSize.width << " x " << frameSize.height;
    cv::putText(frame, vector_to_marker.str(), cv::Point(250, 25), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(32, 240, 32), 2);

    vector_to_marker.str(std::string());
    vector_to_marker << std::setprecision(4) << "FPS: " << std::setw(6) << FPS;
    cv::putText(frame, vector_to_marker.str(), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(220, 16, 220), 2);
}
