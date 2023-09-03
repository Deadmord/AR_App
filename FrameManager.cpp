#include "FrameManager.h"

FrameManager::FrameManager() {}

void FrameManager::showInFrame(const cv::Mat& frame, cv::Size WindSize, cv::Size frameSize, float FPS) {
    std::ostringstream vector_to_marker;

    vector_to_marker.str(std::string());
    vector_to_marker << std::setprecision(4) << "WindwRes: " << std::setw(2) << WindSize.width << " x " << WindSize.height;
    cv::putText(frame, vector_to_marker.str(), cv::Point(10, 25), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(32, 32, 240), 2);

    vector_to_marker.str(std::string());
    vector_to_marker << std::setprecision(4) << "FrameRes: " << std::setw(4) << frameSize.width << " x " << frameSize.height;
    cv::putText(frame, vector_to_marker.str(), cv::Point(250, 25), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(32, 240, 32), 2);

    vector_to_marker.str(std::string());
    vector_to_marker << std::setprecision(4) << "FPS: " << std::setw(6) << FPS;
    cv::putText(frame, vector_to_marker.str(), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(220, 16, 220), 2);
}

void FrameManager::renderFrame(float deltaTime, TextureManager& textureManager, InputHandler& inputHandler, int wndID, GLFWwindow* window, glm::vec4 bgColor, GLuint WinWidth, GLuint WinHeight) {
    RTCounter::startTimer(wndID);

    glfwMakeContextCurrent(window);

    // input hendling
    inputHandler.processInput(window, deltaTime);

    // rendering commands here											
    glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    for (GLsizei index{ 0 }; index < textureManager.objectListSize; index++)
    {
        textureManager.shaders[index]->use();
        textureManager.geometryObjects.bindVertexArray(index);

        if (textureManager.textures[index].isStream || textureManager.textures[index].isVideo)
        {
            // checking
            if (!textureManager.textures[index].vidCapture.isOpened())		//может быть заменить на textures[index].isOpened ?
            {
                std::cout << "Error: Video stream can't be open" << std::endl;
                textureManager.textures[index].isOpened = false;
                break;
            }
            cv::Mat frameVideo, frameVideoAruco;
            bool isSuccessStream = textureManager.textures[index].vidCapture.read(frameVideo);
            //while (inputVideo.grab()) {           использовать для асинхронного захвата видео кадра, наверное лучше разместить в конце метода и сделать исинхронной чтобы выполнялась пока обрабатывается остальные потоки.
            //    inputVideo.retrieve(image);
            //}
            if (!isSuccessStream && textureManager.textures[index].isVideo)
            {
                textureManager.textures[index].vidCapture.set(cv::CAP_PROP_POS_FRAMES, 0); // return to file begin
                isSuccessStream = textureManager.textures[index].vidCapture.read(frameVideo);
            }
            if (!isSuccessStream)
            {
                std::cout << "Error: Video stream can't be read or disconnect! Source: " << textureManager.textures[index].streamIndex << textureManager.textures[index].filePath << std::endl;
                textureManager.textures[index].isOpened = false;
                break;
            }
            else
            {
                if (textureManager.textures[index].isStream)
                 showInFrame(frameVideo, cv::Size(WinWidth, WinHeight), textureManager.arucoProcessorPtr->getFrameSize(), RTCounter::getFPS(wndID));

                //calc and apply distortion correction, very heavy hendling!!!
                //cv::Mat undistortedFrame;
                //cv::undistort(frameVideo, undistortedFrame, arucoProcessorPtr->getCameraMat(), arucoProcessorPtr->getDistortCoeff());

                //only apply distortion maps, mach more faster!!!
                cv::Mat undistortedFrame;
                cv::remap(frameVideo, undistortedFrame, textureManager.arucoProcessorPtr->getUndistortMap1(), textureManager.arucoProcessorPtr->getUndistortMap2(), cv::INTER_LINEAR);

                //check stream videoframe for aruco markers
                if (textureManager.textures[index].isStream && textureManager.arucoProcessorPtr->detectMarkers(undistortedFrame, frameVideoAruco))
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, textureManager.textures[index].internalformat, textureManager.textures[index].width, textureManager.textures[index].height, 0, textureManager.textures[index].format, GL_UNSIGNED_BYTE, frameVideoAruco.data);
                }
                else
                    glTexImage2D(GL_TEXTURE_2D, 0, textureManager.textures[index].internalformat, textureManager.textures[index].width, textureManager.textures[index].height, 0, textureManager.textures[index].format, GL_UNSIGNED_BYTE, frameVideo.data);
            }
        }
        if (textureManager.textures[index].isImg)
        {
            // checking
            if (!textureManager.textures[index].isOpened)		//может быть заменить на textures[index].isOpened ?
            {
                std::cout << "Error: Img can't be open" << std::endl;
                break;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, textureManager.textures[index].internalformat, textureManager.textures[index].width, textureManager.textures[index].height, 0, textureManager.textures[index].format, GL_UNSIGNED_BYTE, textureManager.textures[index].data);
        }

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = textureManager.arucoProcessorPtr->getProjectionMat();
        //projection = glm::perspective(glm::radians(camera.Zoom), (float)WinWidth / (float)WinHeight, 0.1f, 100.0f);
        //projection = glm::perspective(glm::radians(42.0f), (float)WinWidth / (float)WinHeight, 0.1f, 100.0f);

        if (textureManager.textures[index].showOnMarker && !textureManager.arucoProcessorPtr->getMarkers().ids.empty())
        {
            for (glm::mat4 view : textureManager.arucoProcessorPtr->getMarkers().viewMatrixes)
            {
                textureManager.drawObject(index, view, projection, textureManager.textures[index].isBackground);
            }
        }
        else
        {
            view = inputHandler.camera.GetViewMatrix();
            textureManager.drawObject(index, view, projection, textureManager.textures[index].isBackground);
        }
    }
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();

    RTCounter::stopTimer(wndID);
}