#include "TextureManager.h"


void TextureManager::setupImgTexture(GLuint index, const std::string& imgTexture, GLenum internalformat, GLenum format, bool isBackground, bool showOnMarker)
{
    textures[index].isImg = true;
    textures[index].filePath = imgTexture;

    textures[index].data = stbi_load(imgTexture.c_str(), &textures[index].width, &textures[index].height, &textures[index].nrChannels, 0);
    textures[index].internalformat = internalformat;
    textures[index].format = format;
    textures[index].isBackground = isBackground;
    textures[index].showOnMarker = showOnMarker;

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

void TextureManager::addGeometryBuffers(GLsizei size)
{
    geometryObjects.setSize(size);
    geometryObjects.initObjectTexture(false);
    textures.resize(size);
    shaders.resize(size);
    objectListSize = size;
}

void TextureManager::setupGeometryObject(GLuint objIndex, const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState)
{
    geometryObjects.addObject(objIndex, objVBO.size(), objVBO.data(), objEBO.size(), objEBO.data(), objState);
}

void TextureManager::drawObject(GLsizei objIndex, glm::mat4& viewMat, glm::mat4& projectionMat, bool background) {
    glm::mat4 model = glm::mat4(1.0f);

    // ------------- render objects copies from objState list ---------------
    std::shared_ptr <const std::vector<InitState>> objState = geometryObjects.getObjStatePtr(objIndex);

    const GLsizei objSize = geometryObjects.getObjSize(objIndex);
    for (GLsizei i = 0; i < objState->size(); i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, (*objState)[i].positions);
        float angle = (*objState)[i].angle;
        model = glm::rotate(model, glm::radians(angle), (*objState)[i].axisRotation);
        model = glm::rotate(model, glm::radians((*objState)[i].speed * (float)glfwGetTime()), (*objState)[i].axisRotation);

        shaders[objIndex]->setCordTrans("model", glm::value_ptr(model));
        shaders[objIndex]->setCordTrans("view", glm::value_ptr(viewMat));
        shaders[objIndex]->setCordTrans("projection", glm::value_ptr(projectionMat));
        shaders[objIndex]->setColorMask("colorMask", (*objState)[i].colorMask);
        glDrawElements(GL_TRIANGLES, objSize, GL_UNSIGNED_INT, 0);

    }

    if (background) glClear(GL_DEPTH_BUFFER_BIT);	// first object is background
}