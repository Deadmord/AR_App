#include "GLObject.h"

GLObject::GLObject(const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState, bool linePolygonMode)
    :geometryObject_(objVBO.size(), objVBO.data(), objEBO.size(), objEBO.data(), objState), shader_(nullptr), arucoThreadWrapperPtr_(nullptr)
{
    geometryObject_.initObjectTexture(linePolygonMode);
    Console::yellow() << "Object created: " << this << std::endl;
}

GLObject::~GLObject()
{
    Console::yellow() << "Object deleted: " << this << " "
        << (texture_ ?
            texture_->isImg() ? "Image"
            : texture_->isVideo() ? "Video"
            : texture_->isStream() ? "Stream"
            : texture_->isIDSPeak() ? "IDSPeak"
            : "NotDefined"
            : "Empty")
        << std::endl;

    if (texture_)
    {
        texture_ = nullptr;
    }
}

void GLObject::setupShaderProgram(Shader* shaderProgPtr)
{
    shader_ = shaderProgPtr;
}

void GLObject::setupArUcoPtr(std::shared_ptr<ArucoThreadWrapper> arucoThreadWrapper)
{
    arucoThreadWrapperPtr_ = arucoThreadWrapper;
}

void GLObject::setupTexture(const std::shared_ptr<texture> texture)
{
    texture_ = texture;

    //----------- Init ArUco and set resolution -----------
    if ((texture_->isStream() || texture_->isIDSPeak()) && texture_->isBackground())
    {
        arucoThreadWrapperPtr_->initAndStartAruco(ArUcoMarkerLength, ArUcoDictionaryId, texture_->getCameraParams(), ArUcoShowRejected);
        texture_->setWidth(arucoThreadWrapperPtr_->getFrameSize().width);
        texture_->setHeight(arucoThreadWrapperPtr_->getFrameSize().height);
    }
}

void GLObject::renderObject(Camera& camera, PrintInFrameCallback printCallback)
{
    shader_->use();
    geometryObject_.bindVertexArray();

    // Checking capturing
    if (!texture_->isOpened())
    {
        if (texture_->isStream() || texture_->isVideo() || texture_->isIDSPeak())
        {
            Console::red() << "Error: Video stream not run. " << texture_ << std::endl;
            return;
        }
        if (texture_ ->isImg())
        {
            Console::red() << "Error: Img can't be open" << texture_ << std::endl;
            return;
        }
        Console::red() << "Error: Unexpected texture type" << texture_ << std::endl;
        return;
    }

    // Update frame
    cv::Mat textureFrame, textureFrameAruco;
    if (texture_->isStream() || texture_->isVideo() || texture_->isIDSPeak())
    {
        texture_->waitAndGet(textureFrame);
    }
    else if (texture_->isImg())
    {
        texture_->waitAndGet(textureFrame);
    }
    else 
    {
        Console::red() << "Error: Unexpected texture type" << texture_ << std::endl;
        return;
    }

    // Process background frame
    if (texture_->isBackground())
    {
        //arucoThreadWrapperPtr_->undistortFrame(textureFrame, textureFrame);
        textureFrameAruco = textureFrame.clone();
        arucoThreadWrapperPtr_->processFrame(textureFrameAruco);
        //while (!arucoThreadWrapperPtr_->tryGetProcessedFrame(textureFrameAruco)) {}
        //if(!arucoThreadWrapperPtr_->tryGetProcessedFrame(textureFrameAruco)) textureFrameAruco = textureFrame;
        printCallback(textureFrame, arucoThreadWrapperPtr_->getFrameSize(), texture_->getFPS(), arucoThreadWrapperPtr_->getFPS());
    }

    // Set OpenGL texture
    if(texture_->isImg())
        glTexImage2D(GL_TEXTURE_2D, 0, texture_->getInternalFormat(), texture_->getWidth(), texture_->getHeight(), 0, texture_->getFormat(), GL_UNSIGNED_BYTE, texture_->getData());
    else
        glTexImage2D(GL_TEXTURE_2D, 0, texture_->getInternalFormat(), texture_->getWidth(), texture_->getHeight(), 0, texture_->getFormat(), GL_UNSIGNED_BYTE, textureFrame.data);
        
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = arucoThreadWrapperPtr_->getProjectionMat();
    //projection = glm::perspective(glm::radians(camera.Zoom), (float)WinWidth / (float)WinHeight, 0.1f, 100.0f);
    //projection = glm::perspective(glm::radians(42.0f), (float)WinWidth / (float)WinHeight, 0.1f, 100.0f);

    // Render OpenGL object
    if (texture_->isShowOnMarker() /*&& !arucoProcessorPtr->getMarkers().ids.empty()*/)       //if ids list is empty it will be drow objects on top of all markers
    {
        Markers markers = arucoThreadWrapperPtr_->getDetectedMarkers();
        if (texture_->getMarkerIds() == nullptr)
        {
            for (glm::mat4 view : markers.viewMatrixes)       //drow objects for all markers
            {
                drowObject(view, projection, texture_->isBackground());
            }
        }
        else
        {
            for (int markerIndex{ 0 }; markerIndex < markers.ids.size(); markerIndex++)
            {
                int target = markers.ids.at(markerIndex);
                if (std::ranges::any_of(*(texture_->getMarkerIds().get()), [target](int value) { return value == target; }))
                {
                    view = markers.viewMatrixes.at(markerIndex);
                    drowObject(view, projection, texture_->isBackground());
                }

            }
        }
    }
    else
    {
        view = camera.GetViewMatrix();
        drowObject(view, projection, texture_->isBackground());
    }
}

void GLObject::drowObject(glm::mat4& viewMat, glm::mat4& projectionMat, bool background)
{
    glm::mat4 model = glm::mat4(1.0f);

    // ------------- render objects copies from objState list ---------------
    std::shared_ptr <const std::vector<InitState>> objState = geometryObject_.getObjStatePtr();

    const GLsizei objSize = geometryObject_.getObjSize();
    for (GLsizei i = 0; i < objState->size(); i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, (*objState)[i].positions);
        float angle = (*objState)[i].angle;
        model = glm::rotate(model, glm::radians(angle), (*objState)[i].axisRotation);
        model = glm::rotate(model, glm::radians((*objState)[i].speed * (float)glfwGetTime()), (*objState)[i].axisRotation);

        shader_->setCordTrans("model", glm::value_ptr(model));
        shader_->setCordTrans("view", glm::value_ptr(viewMat));
        shader_->setCordTrans("projection", glm::value_ptr(projectionMat));
        shader_->setColorMask("colorMask", (*objState)[i].colorMask);
        glDrawElements(GL_TRIANGLES, objSize, GL_UNSIGNED_INT, 0);

    }

    if (background) glClear(GL_DEPTH_BUFFER_BIT);	// first object is background
}

cv::Mat GLObject::getTextureImage()
{
    cv::Mat image;
    texture_->waitAndGet(image);
    return image;
}
