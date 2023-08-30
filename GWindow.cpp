#include "GWindow.h"

GWindow::GWindow(unsigned int WinID, unsigned int WinWidth, unsigned int WinHeight, unsigned int WinPosX, unsigned int WinPosY, const std::string& name, GLFWmonitor* monitor, glm::vec4 bgColor)
    :wndID(WinID), WinWidth(WinWidth), WinHeight(WinHeight), lastX(WinWidth / 2.0f), lastY(WinHeight / 2.0f), camera(Camera(camInitPosition)), bgColor(bgColor)
{
    window = glfwCreateWindow(WinWidth, WinHeight, name.c_str(), monitor, NULL);
    if (window == NULL)
    {
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

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Exception: Failed to initialize GLAD" << std::endl;
        return;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // Set the user pointer to this instance so that it can be accessed in the static wrapper functions.
    glfwSetWindowUserPointer(window, this);

    std::cout << "GLFW window created successfully: " << window << std::endl;

}

void GWindow::addGeometryBuffers(GLsizei size)
{
    glfwMakeContextCurrent(window);
    geometryObjects.setSize(size);
    geometryObjects.initObjectTexture(false);
    textures.resize(size);
    shaders.resize(size);
    objectListSize = size;
}

void GWindow::setupGeometryObject(GLuint objIndex, const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState)
{
    glfwMakeContextCurrent(window);
    geometryObjects.addObject(objIndex, objVBO.size(), objVBO.data(), objEBO.size(), objEBO.data(), objState);
}

void GWindow::setupImgTexture(GLuint index, const std::string& imgTexture, GLenum internalformat, GLenum format, bool isBackground, bool showOnMarker)
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

void GWindow::setupShaderProgram(GLuint index, Shader* shaderProgPtr)
{
    shaders[index] = shaderProgPtr;
}

void GWindow::renderFrame(float deltaTime)
{
    RTCounter::startTimer(wndID);

    glfwMakeContextCurrent(window);

    // input hendling
    processInput(window, deltaTime);

    // rendering commands here											
    glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    for (GLsizei index{ 0 }; index < objectListSize; index++)
    {
        shaders[index]->use();
        geometryObjects.bindVertexArray(index);

        if (textures[index].isStream || textures[index].isVideo)
        {
            // checking
            if (!textures[index].vidCapture.isOpened())		//����� ���� �������� �� textures[index].isOpened ?
            {
                std::cout << "Error: Video stream can't be open" << std::endl;
                textures[index].isOpened = false;
                break;
            }
            cv::Mat frameVideo, frameVideoAruco;
            bool isSuccessStream = textures[index].vidCapture.read(frameVideo);
                //while (inputVideo.grab()) {           ������������ ��� ������������ ������� ����� �����, �������� ����� ���������� � ����� ������ � ������� ����������� ����� ����������� ���� �������������� ��������� ������.
                //    inputVideo.retrieve(image);
                //}
            if (!isSuccessStream && textures[index].isVideo)
            {
                textures[index].vidCapture.set(cv::CAP_PROP_POS_FRAMES, 0); // return to file begin
                isSuccessStream = textures[index].vidCapture.read(frameVideo);
            }
            if (!isSuccessStream)
            {
                std::cout << "Error: Video stream can't be read or disconnect! Source: " << textures[index].streamIndex << textures[index].filePath << std::endl;
                textures[index].isOpened = false;
                break;
            }
            else
            {
                if (textures[index].isStream)
                showInFrame(frameVideo, cv::Size(WinWidth, WinHeight), arucoProcessorPtr->getFrameSize(), RTCounter::getFPS(wndID));

                //calc and apply distortion correction, very heavy hendling!!!
                //cv::Mat undistortedFrame;
                //cv::undistort(frameVideo, undistortedFrame, arucoProcessorPtr->getCameraMat(), arucoProcessorPtr->getDistortCoeff());

                //only apply distortion maps, mach more faster!!!
                cv::Mat undistortedFrame;
                cv::remap(frameVideo, undistortedFrame, arucoProcessorPtr->getUndistortMap1(), arucoProcessorPtr->getUndistortMap2(), cv::INTER_LINEAR);

                //check stream videoframe for aruco markers
                if (textures[index].isStream && arucoProcessorPtr->detectMarkers(undistortedFrame, frameVideoAruco))
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, frameVideoAruco.data);
                }
                else
                glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, frameVideo.data);
            }
        }
        if (textures[index].isImg)
        {
            // checking
            if (!textures[index].isOpened)		//����� ���� �������� �� textures[index].isOpened ?
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
        
        if (textures[index].showOnMarker && !arucoProcessorPtr->getMarkers().ids.empty())
        {
            for (glm::mat4 view : arucoProcessorPtr->getMarkers().viewMatrixes)
            {
                drowObject(index, view, projection, textures[index].isBackground);
            }
        }
        else
        {
            view = camera.GetViewMatrix();
            drowObject(index, view, projection, textures[index].isBackground);
        }
    }
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();

    RTCounter::stopTimer(wndID);
}

void GWindow::drowObject(GLsizei objIndex, glm::mat4& viewMat, glm::mat4& projectionMat, bool background) {
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

void GWindow::makeContextCurrent()
{
    glfwMakeContextCurrent(window);
}

void GWindow::showInFrame(const cv::Mat& frame, cv::Size WindSize, cv::Size frameSize, float FPS) {
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

void GWindow::framebufferSizeCallbackWrapper(GLFWwindow* window, int width, int height)
{
    GWindow* instance = static_cast<GWindow*>(glfwGetWindowUserPointer(window));
    if (instance)
    {
        instance->framebufferSizeCallback(window, width, height);
    }
}

void GWindow::mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods)
{
    GWindow* instance = static_cast<GWindow*>(glfwGetWindowUserPointer(window));
    if (instance)
    {
        instance->mouseButtonCallback(window, button, action, mods);
    }
}

void GWindow::mouseCursorCallbackWrapper(GLFWwindow* window, double xpos, double ypos)
{
    GWindow* instance = static_cast<GWindow*>(glfwGetWindowUserPointer(window));
    if (instance)
    {
        instance->mouseCursorCallback(window, xpos, ypos);
    }
}

void GWindow::scrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
    GWindow* instance = static_cast<GWindow*>(glfwGetWindowUserPointer(window));
    if (instance)
    {
        instance->scrollCallback(window, xoffset, yoffset);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void GWindow::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    if (width != 0 && height != 0)
    {
        glViewport(0, 0, width, height);
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void GWindow::processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void GWindow::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            leftMouseButtonPressed = true;
        }
        else if (action == GLFW_RELEASE)
        {
            leftMouseButtonPressed = false;
        }
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void GWindow::mouseCursorCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = lastX - xpos;
    float yoffset = ypos - lastY; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (leftMouseButtonPressed)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void GWindow::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}