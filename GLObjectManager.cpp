#include "GLObjectManager.h"

void GLObjectManager::setupGeometryObject(GLuint objIndex, const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState)
{
    glfwMakeContextCurrent(window);
    geometryObjects.addObject(objIndex, objVBO.size(), objVBO.data(), objEBO.size(), objEBO.data(), objState);
}

void GLObjectManager::setupImgTexture(GLuint index, const std::string& imgTexture, GLenum internalformat, GLenum format, bool isBackground, bool showOnMarker, std::shared_ptr<std::vector<int>> markerIds)
{
    textures[index].isImg = true;
    textures[index].filePath = imgTexture;

    textures[index].data = stbi_load(imgTexture.c_str(), &textures[index].width, &textures[index].height, &textures[index].nrChannels, 0);
    textures[index].internalformat = internalformat;
    textures[index].format = format;
    textures[index].isBackground = isBackground;
    textures[index].showOnMarker = showOnMarker;
    textures[index].markerIds = markerIds;

    //---------------------- video texture -------------------
    if (!textures[index].data)
    {
        std::cout << "Error: Img can't be open! Source: " << imgTexture << std::endl;
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, textures[index].width, textures[index].height, 0, format, GL_UNSIGNED_BYTE, textures[index].data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Img opened successfully!" << std::endl;
        textures[index].isOpened = true;

    }
    //stbi_image_free(data);
}

void GLObjectManager::setupShaderProgram(GLuint index, Shader* shaderProgPtr)
{
    shaders[index] = shaderProgPtr;
}