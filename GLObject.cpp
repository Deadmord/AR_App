#include "GLObject.h"

GLObject::GLObject(const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState, bool linePolygonMode = false)
    :geometryObject(objVBO.size(), objVBO.data(), objEBO.size(), objEBO.data(), objState)
{
    geometryObject.initObjectTexture(linePolygonMode);
}

void GLObject::setupShaderProgram(Shader* shaderProgPtr)
{
    shader = shaderProgPtr;
}

void GLObject::setupImgTexture(const std::string& imgTexture, GLenum internalformat, GLenum format, bool rotate = false, bool isBackground, bool showOnMarker, std::shared_ptr<std::vector<int>> markerIds)
{
    this->isImg = true;
    this->filePath = imgTexture;

    this->data = stbi_load(imgTexture.c_str(), &(this->width), &(this->height), &(this->nrChannels), 0);
    this->internalformat = internalformat;
    this->format = format;
    this->rotate = rotate;
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

void GLObject::renderObject()
{
    shader->use();
    geometryObject.bindVertexArray();

    if (isStream || isVideo)
    {
        // checking
        if (!vidCapture.isOpened())		//может быть заменить на textures[index].isOpened ?
        {
            std::cout << "Error: Video stream can't be open" << std::endl;
            isOpened = false;
            return;
        }
        cv::Mat frameVideo, frameVideoAruco;
        bool isSuccessStream = vidCapture.read(frameVideo);
        //while (inputVideo.grab()) {           использовать для асинхронного захвата видео кадра, наверное лучше разместить в конце метода и сделать исинхронной чтобы выполнялась пока обрабатывается остальные потоки.
        //    inputVideo.retrieve(image);
        //}
        if (!isSuccessStream && isVideo)
        {
            vidCapture.set(cv::CAP_PROP_POS_FRAMES, 0); // return to file begin
            isSuccessStream = vidCapture.read(frameVideo);
        }
        if (!isSuccessStream)
        {
            std::cout << "Error: Video stream can't be read or disconnect! Source: " << streamIndex << filePath << std::endl;
            isOpened = false;
            return;
        }
        else
        {
            if (rotate)
                cv::rotate(frameVideo, frameVideo, cv::ROTATE_180);

            //calc and apply distortion correction, very heavy hendling!!!
            //cv::Mat undistortedFrame;
            //cv::undistort(frameVideo, undistortedFrame, arucoProcessorPtr->getCameraMat(), arucoProcessorPtr->getDistortCoeff());

            //only apply distortion maps, mach more faster!!!
            //cv::Mat undistortedFrame;
            //cv::remap(frameVideo, undistortedFrame, arucoProcessorPtr->getUndistortMap1(), arucoProcessorPtr->getUndistortMap2(), cv::INTER_LINEAR);

            //check stream videoframe for aruco markers
            //if (textures[index].isStream && arucoProcessorPtr->detectMarkers(frameVideo, frameVideoAruco))
            //{
            //    glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, frameVideoAruco.data);
            //}
            //else
            //glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, frameVideo.data);

            if (isStream && isBackground)
            {
                arucoProcessorPtr->detectMarkers(frameVideo, frameVideo);
                showInFrame(frameVideo, cv::Size(WinWidth, WinHeight), arucoProcessorPtr->getFrameSize(), RTCounter::getFPS(wndID), { RTCounter::getDeltaTime((4 * 1) + wndID), RTCounter::getDeltaTime((4 * 2) + wndID), RTCounter::getDeltaTime((4 * 3) + wndID), RTCounter::getDeltaTime(wndID) });
            }
            glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, frameVideo.data);
        }
    }
    if (textures[index].isImg)
    {
        // checking
        if (!textures[index].isOpened)		//может быть заменить на textures[index].isOpened ?
        {
            std::cout << "Error: Img can't be open" << std::endl;
            break;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, textures[index].data);
    }
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = arucoProcessorPtr->getProjectionMat();
    //projection = glm::perspective(glm::radians(camera.Zoom), (float)WinWidth / (float)WinHeight, 0.1f, 100.0f);
    //projection = glm::perspective(glm::radians(42.0f), (float)WinWidth / (float)WinHeight, 0.1f, 100.0f);

    if (textures[index].showOnMarker /*&& !arucoProcessorPtr->getMarkers().ids.empty()*/)
    {
        if (textures[index].markerIds == nullptr)
        {
            for (glm::mat4 view : arucoProcessorPtr->getMarkers().viewMatrixes)       //drow objects for all markers
            {
                drowObject(index, view, projection, textures[index].isBackground);
            }
        }
        else
        {
            for (int markerIndex{ 0 }; markerIndex < arucoProcessorPtr->getMarkers().ids.size(); markerIndex++)
            {
                int target = arucoProcessorPtr->getMarkers().ids.at(markerIndex);
                if (std::ranges::any_of(*(textures[index].markerIds.get()), [target](int value) { return value == target; }))
                {
                    view = arucoProcessorPtr->getMarkers().viewMatrixes.at(markerIndex);
                    drowObject(index, view, projection, textures[index].isBackground);
                }

            }
        }
    }
    else
    {
        view = camera.GetViewMatrix();
        drowObject(index, view, projection, textures[index].isBackground);
    }
}

void GLObject::drowObject(GLsizei objIndex, glm::mat4& viewMat, glm::mat4& projectionMat, bool background)
{
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