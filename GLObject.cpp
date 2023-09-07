#include "GLObject.h"

GLObject::GLObject(const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState)
    :geometryObject(objVBO.size(), objVBO.data(), objEBO.size(), objEBO.data(), objState)
{}

void GLObject::setupImgTexture(const std::string& imgTexture, GLenum internalformat, GLenum format, bool isBackground, bool showOnMarker, std::shared_ptr<std::vector<int>> markerIds)
{
    this->isImg = true;
    this->filePath = imgTexture;

    this->data = stbi_load(imgTexture.c_str(), &(this->width), &(this->height), &(this->nrChannels), 0);
    this->internalformat = internalformat;
    this->format = format;
    this->isBackground = isBackground;
    this->showOnMarker = showOnMarker;
    this->markerIds = markerIds;

    //---------------------- video texture -------------------
    if (!this->data)
    {
        std::cout << "Error: Img can't be open! Source: " << imgTexture << std::endl;
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, this->internalformat, this->width, this->height, 0, format, GL_UNSIGNED_BYTE, this->data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Img opened successfully!" << std::endl;
        this->isOpened = true;

    }
    //stbi_image_free(data);
}

void GLObject::setupShaderProgram(Shader* shaderProgPtr)
{
    shader = shaderProgPtr;
}