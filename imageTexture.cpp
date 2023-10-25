#include "imageTexture.h"

imageTexture::imageTexture(const std::string& imgTexture, GLenum internalformat, GLenum format, bool rotate, bool isBackground, bool showOnMarker, std::shared_ptr<std::vector<int>> markerIds)
{
    isImg_ = true;
    filePath_ = imgTexture;

    data_ = stbi_load(imgTexture.c_str(), &(width_), &(height_), &(nrChannels_), 0);
    internalformat_ = internalformat;
    format_ = format;
    rotate_ = rotate;
    isBackground_ = isBackground;
    showOnMarker_ = showOnMarker;
    markerIds_ = markerIds;

    if (!data_)
    {
        Console::red() << "Error: Img can't be open! Source: " << imgTexture << std::endl;
    }
    else
    {
        //glTexImage2D(GL_TEXTURE_2D, 0, internalformat_, width_, height_, 0, format_, GL_UNSIGNED_BYTE, data_);
        //glGenerateMipmap(GL_TEXTURE_2D);
        Console::green() << "Img opened successfully!\t" << "ImagePath: " << imgTexture << "\tTexturePtr: " << this << std::endl;
        isOpened_ = true;

        currentValue_.push(cv::Mat(height_, width_, CV_8UC3, data_)); //TODO: тут нужно исправить

    }
    //stbi_image_free(data_);
}

imageTexture::~imageTexture()
{
    Console::yellow() << "Image texture deleted!" << "\tTexturePtr: " << this << std::endl;
}

uchar* imageTexture::getData() {
    return data_;
}

float imageTexture::getFPS()
{
    return 0.0f;
}

void imageTexture::setWidth(GLint width)
{
    if (isImg_) //реализовать изменение ширины для Img
        Console::red() << "Error: The setWidth method for imageTexture has not been implemented." << std::endl;
}

void imageTexture::setHeight(GLint height)
{
    if (isImg_) //реализовать изменение высоты для Img
        Console::red() << "Error: The setHeight method for imageTexture has not been implemented." << std::endl;
}

const std::string& imageTexture::getCameraParams() const
{
    return "";
    Console::red() << "CameraParams not exist for imageTexture." << std::endl;
}
