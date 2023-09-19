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
    updateFrame();
    arucoProcessorPtr = *arucoProcessorPtrToPtr;
    shader->use();
    geometryObject.bindVertexArray();

    if (!currentFrame.empty()) {
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, currentFrame.cols, currentFrame.rows, 0, format, GL_UNSIGNED_BYTE, currentFrame.data);
    }

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = arucoProcessorPtr->getProjectionMat();
    //projection = glm::perspective(glm::radians(camera.Zoom), (float)WinWidth / (float)WinHeight, 0.1f, 100.0f);
    //projection = glm::perspective(glm::radians(42.0f), (float)WinWidth / (float)WinHeight, 0.1f, 100.0f);

    if (showOnMarker /*&& !arucoProcessorPtr->getMarkers().ids.empty()*/)       //if ids list is empty it will be drow objects on top of all markers
    {
        if (markerIds == nullptr)
        {
            for (glm::mat4 view : arucoProcessorPtr->getMarkers().viewMatrixes)       //drow objects for all markers
            {
                drowObject(view, projection, isBackground);
            }
        }
        else
        {
            for (int markerIndex{ 0 }; markerIndex < arucoProcessorPtr->getMarkers().ids.size(); markerIndex++)
            {
                int target = arucoProcessorPtr->getMarkers().ids.at(markerIndex);
                if (std::ranges::any_of(*(markerIds.get()), [target](int value) { return value == target; }))
                {
                    view = arucoProcessorPtr->getMarkers().viewMatrixes.at(markerIndex);
                    drowObject(view, projection, isBackground);
                }

            }
        }
    }
    else
    {
        view = camera.GetViewMatrix();
        drowObject(view, projection, isBackground);
    }
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

void GLObject::updateFrame() {
    cv::Mat newFrame;
    currentFrame = acquisitionWorker->TryGetImage(newFrame);
}

void GLObject::setAcquisitionWorker(std::shared_ptr<AcquisitionWorker> worker) {
    this->acquisitionWorker = worker;
}

void GLObject::setupVideoIDS(std::shared_ptr<AcquisitionWorker> worker, GLenum internalformat, GLenum format, bool rotate, bool isBackground, bool showOnMarker,
    std::shared_ptr<std::vector<int>> markerIds, std::string cameraParams) {

    setAcquisitionWorker(worker);
    if (!acquisitionWorker->OpenCamera()) {
        std::cout << "Cannot open the camera." << std::endl;
        return;
    }

    if (!acquisitionWorker->SetDataStream()) {
        std::cout << "Cannot set data stream" << std::endl;
        return;
    }

    if (!acquisitionWorker->SetRoi(640, 480, 640, 480)) {
        std::cout << "Cannot set ROI" << std::endl;
        return;
    }

    if (!acquisitionWorker->AllocAndAnnounceBuffers()) {
        std::cout << "Cannot allocate buffers" << std::endl;
        return;
    }

    if (!acquisitionWorker->StartAcquisition()) {
        std::cout << "Cannot start acquisition" << std::endl;
        return;
    }

    acquisitionWorker->Start();

    if (this->isStream) //&& turn aruco flag
    {
        // ArUco init
        *arucoProcessorPtrToPtr = std::make_shared<ArucoProcessor>(ArUcoMarkerLength, ArUcoDictionaryId, cameraParams, ArUcoShowRejected);
        arucoProcessorPtr = *arucoProcessorPtrToPtr;
    }
    //------------------------------------------

    this->width = static_cast<int>(this->vidCapture.get(cv::CAP_PROP_FRAME_WIDTH));
    this->height = static_cast<int>(this->vidCapture.get(cv::CAP_PROP_FRAME_HEIGHT));

    this->internalformat = internalformat;
    this->format = format;
    this->rotate = rotate;
    this->isBackground = isBackground;
    this->showOnMarker = showOnMarker;
    this->markerIds = markerIds;

    //---------------------- video texture -------------------
    cv::Mat frameVideo;
    if (this->acquisitionWorker->TryGetImage(frameVideo)) {
        std::cout << "Error: Video stream can't be read or disconnect!" << std::endl;
    }
    std::cout << "Video stream opened successfully!" << std::endl;
    this->data = frameVideo.data;
    this->isOpened = true;
}