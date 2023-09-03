#include "GWindowCore.h"

GWindowCore::GWindowCore(unsigned int WinID, const unsigned int& width, const unsigned int& height, unsigned int WinPosX, unsigned int WinPosY, const std::string& name, GLFWmonitor* monitor, glm::vec4 bgColor_)
    : WinWidth(width), WinHeight(height), bgColor(bgColor_) {

    window = glfwCreateWindow(WinWidth, WinHeight, name.c_str(), monitor, NULL);
    if (window == nullptr) {
        std::cout << "Exception: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowPos(window, WinPosX, WinPosY + WINDOW_PANEL_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallbackWrapper);
    glfwSetMouseButtonCallback(window, mouseButtonCallbackWrapper);
    glfwSetCursorPosCallback(window, mouseCursorCallbackWrapper);
    glfwSetScrollCallback(window, scrollCallbackWrapper);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Exception: Failed to initialize GLAD" << std::endl;
        return;
    }
    glEnable(GL_DEPTH_TEST);

    glfwSetWindowUserPointer(window, this);

    std::cout << "GLFW window created successfully: " << window << std::endl;
}

GWindowCore::~GWindowCore() {
    glfwDestroyWindow(window);
}

void GWindowCore::addGeometryBuffers(GLsizei size)
{
    glfwMakeContextCurrent(window);
    textureManager.addGeometryBuffers(size);
}

void GWindowCore::setupGeometryObject(GLuint objIndex, const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState)
{
    glfwMakeContextCurrent(window);
    textureManager.setupGeometryObject(objIndex, objVBO, objEBO, objState);
}

void GWindowCore::setupShaderProgram(GLuint index, Shader* shaderProgPtr)
{
    textureManager.shaders[index] = shaderProgPtr;
}

void GWindowCore::renderFrame(float deltaTime)
{
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
                    frameAnalyzer.showInFrame(frameVideo, cv::Size(WinWidth, WinHeight), textureManager.arucoProcessorPtr->getFrameSize(), RTCounter::getFPS(wndID));

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

void GWindowCore::framebufferSizeCallbackWrapper(GLFWwindow* window, int width, int height)
{
    GWindowCore* instance = static_cast<GWindowCore*>(glfwGetWindowUserPointer(window));
    if (instance)
    {
        instance->framebufferSizeCallback(window, width, height);
    }
}

void GWindowCore::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (width != 0 && height != 0)
    {
        glViewport(0, 0, width, height);
    }
}

void GWindowCore::mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods)
{
    GWindowCore* instance = static_cast<GWindowCore*>(glfwGetWindowUserPointer(window));
    if (instance)
    {
        instance->inputHandler.mouseButtonCallback(window, button, action, mods);
    }
}

void GWindowCore::mouseCursorCallbackWrapper(GLFWwindow* window, double xpos, double ypos)
{
    GWindowCore* instance = static_cast<GWindowCore*>(glfwGetWindowUserPointer(window));
    if (instance)
    {
        instance->inputHandler.mouseCursorCallback(window, xpos, ypos);
    }
}

void GWindowCore::scrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
    GWindowCore* instance = static_cast<GWindowCore*>(glfwGetWindowUserPointer(window));
    if (instance)
    {
        instance->scrollCallback(window, xoffset, yoffset);
    }
}

void GWindowCore::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    inputHandler.camera.ProcessMouseScroll(static_cast<float>(yoffset));
}