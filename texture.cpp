#include "texture.h"

bool texture::tryGet(cv::Mat& outputValue) {
    return currentValue_.tryGet(outputValue);
}

void texture::waitAndGet(cv::Mat& outputValue) {
    return currentValue_.waitAndGet(outputValue);
}

bool texture::tryPop(cv::Mat& outputValue) {
    return currentValue_.tryPop(outputValue);
}

void texture::waitAndPop(cv::Mat& outputValue) {
    return currentValue_.waitAndPop(outputValue);
}

bool texture::isOpened() const {
    return isOpened_;
}

bool texture::isImg() const {
    return isImg_;
}

bool texture::isVideo() const {
    return isVideo_;
}

bool texture::isStream() const {
    return isStream_;
}

bool texture::isIDSPeak() const {
    return isIDSPeak_;
}

bool texture::isBackground() const {
    return isBackground_;
}

bool texture::isShowOnMarker() const {
    return showOnMarker_;
}

GLint texture::getWidth() const {
    return width_;
}

GLint texture::getHeight() const {
    return height_;
}

GLint texture::getNrChannels() const {
    return nrChannels_;
}

GLenum texture::getInternalFormat() const {
    return internalformat_;
}

GLenum texture::getFormat() const {
    return format_;
}

std::shared_ptr<std::vector<int>> texture::getMarkerIds() const {
    return markerIds_;
}
