#pragma once
#include <glad/glad.h>
#include <opencv2/opencv.hpp>
#include <memory>
#include "ThreadSafeValue.h"
#include "config.h"
#include "Console.h"


class texture
{
public:

    virtual void start() = 0;

    virtual void stop() = 0;

	virtual uchar* getData() = 0;

    virtual float getFPS() = 0;

    virtual const std::string& getCameraParams() const = 0;

    virtual void setWidth(GLint width) = 0;

    virtual void setHeight(GLint height) = 0;

    bool tryGet(cv::Mat& outputValue);

    void waitAndGet(cv::Mat& outputValue);

    bool tryPop(cv::Mat& outputValue);

    void waitAndPop(cv::Mat& outputValue);

    bool isOpened() const;

    bool isImg() const;

    bool isVideo() const;

    bool isStream() const;

    bool isIDSPeak() const;

    bool isBackground() const;

    bool isShowOnMarker() const;

    GLint getWidth() const;

    GLint getHeight() const;

    GLint getNrChannels() const;

    GLenum getInternalFormat() const;

    GLenum getFormat() const;

    std::shared_ptr<std::vector<int>> getMarkerIds() const;

protected:
    ThreadSafeValue<cv::Mat> currentValue_;

	//Textures property
	bool			isOpened_ = false;
	bool			isImg_ = false;
	bool			isVideo_ = false;
	bool			isStream_ = false;
	bool			isIDSPeak_ = false;
	bool			rotate_ = false;
	bool			isBackground_ = false;
	bool			showOnMarker_ = false;
	GLint			width_;
	GLint			height_;
	GLint			nrChannels_ = 3;
	GLenum			internalformat_;
	GLenum			format_;
	uchar*			data_;
	std::shared_ptr<std::vector<int>> markerIds_;
};

