#include "GLObject.h"

GLObject::GLObject(const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState, bool linePolygonMode)
    :geometryObject(objVBO.size(), objVBO.data(), objEBO.size(), objEBO.data(), objState)
{
    geometryObject.initObjectTexture(linePolygonMode);
}

void GLObject::setupShaderProgram(Shader* shaderProgPtr)
{
    shader = shaderProgPtr;
}

void GLObject::setupImgTexture(const std::string& imgTexture, GLenum internalformat, GLenum format, bool rotate, bool isBackground, bool showOnMarker, std::shared_ptr<std::vector<int>> markerIds)
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

void GLObject::renderObject(Camera& camera, PrintInFrameCallback printCallback)
{
    arucoProcessorPtr = *arucoProcessorPtrToPtr;
    shader->use();
    geometryObject.bindVertexArray();

    if (isStream || isVideo)
    {
        processVideoStream(printCallback);
    }
    else if (isIDSPeak)
    {
        processIDSStream(printCallback);
    }
    else if (isImg)
    {
        processImage();
    }

    glm::mat4 projection = arucoProcessorPtr->getProjectionMat();
    renderMarkers(camera, projection);
}

void GLObject::drowObject(glm::mat4& viewMat, glm::mat4& projectionMat, bool background)
{
    glm::mat4 model = glm::mat4(1.0f);

    // ------------- render objects copies from objState list ---------------
    std::shared_ptr <const std::vector<InitState>> objState = geometryObject.getObjStatePtr();

    const GLsizei objSize = geometryObject.getObjSize();
    for (GLsizei i = 0; i < objState->size(); i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, (*objState)[i].positions);
        float angle = (*objState)[i].angle;
        model = glm::rotate(model, glm::radians(angle), (*objState)[i].axisRotation);
        model = glm::rotate(model, glm::radians((*objState)[i].speed * (float)glfwGetTime()), (*objState)[i].axisRotation);

        shader->setCordTrans("model", glm::value_ptr(model));
        shader->setCordTrans("view", glm::value_ptr(viewMat));
        shader->setCordTrans("projection", glm::value_ptr(projectionMat));
        shader->setColorMask("colorMask", (*objState)[i].colorMask);
        glDrawElements(GL_TRIANGLES, objSize, GL_UNSIGNED_INT, 0);

    }

    if (background) glClear(GL_DEPTH_BUFFER_BIT);	// first object is background
}

void GLObject::processVideoStream(const PrintInFrameCallback& printCallback)
{
    if (!vidCapture.isOpened()) {
        std::cout << "Error: Video stream can't be open" << std::endl;
        isOpened = false;
        return;
    }

    cv::Mat frameVideo;
    bool isSuccessStream = vidCapture.read(frameVideo);

    if (!isSuccessStream && isVideo) {
        vidCapture.set(cv::CAP_PROP_POS_FRAMES, 0); // return to file begin
        isSuccessStream = vidCapture.read(frameVideo);
    }

    if (!isSuccessStream) {
        std::cout << "Error: Video stream can't be read or disconnect! Source: " << streamIndex << filePath << std::endl;
        isOpened = false;
        return;
    }

    processFrame(frameVideo, printCallback);
}

void GLObject::processIDSStream(const PrintInFrameCallback& printCallback)
{
    cv::Mat frameVideo;
    bool isSuccessStream = workerIDSPtr->TryGetImage(frameVideo);

    if (!isSuccessStream) {
        std::cout << "Error: IDS stream can't be read or disconnect! Source: " << std::endl;
        return;
    }

    processFrame(frameVideo, printCallback);
}

void GLObject::processFrame(cv::Mat& frame, const PrintInFrameCallback& printCallback)
{
    if (rotate) {
        cv::rotate(frame, frame, cv::ROTATE_180);
    }

    if ((isStream || isIDSPeak) && isBackground) {
        arucoProcessorPtr->detectMarkers(frame, frame);
        printCallback(frame, arucoProcessorPtr->getFrameSize());
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, frame.data);
}

void GLObject::processImage()
{
    if (!isOpened) {
        std::cout << "Error: Img can't be open" << std::endl;
        return;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
}

void GLObject::renderOnMarkers(glm::mat4& view, glm::mat4& projection)
{
    const auto& markers = arucoProcessorPtr->getMarkers();

    if (markerIds == nullptr) {
        for (glm::mat4 view : markers.viewMatrixes) { // рисуем объекты для всех маркеров
            drowObject(view, projection, isBackground);
        }
    }
    else {
        for (int markerIndex{ 0 }; markerIndex < markers.ids.size(); markerIndex++) {
            int target = markers.ids.at(markerIndex);
            if (std::ranges::any_of(*(markerIds.get()), [target](int value) { return value == target; })) {
                view = markers.viewMatrixes.at(markerIndex);
                drowObject(view, projection, isBackground);
            }
        }
    }
}

void GLObject::renderMarkers(Camera& camera, glm::mat4& projection)
{
    glm::mat4 view = glm::mat4(1.0f);
    const auto& markers = arucoProcessorPtr->getMarkers();

    if (showOnMarker) {
        renderOnMarkers(view, projection);
    }
    else {
        view = camera.GetViewMatrix();
        drowObject(view, projection, isBackground);
    }
}

