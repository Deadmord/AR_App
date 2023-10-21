#include "GWindow.h"

GWindow::GWindow(unsigned int WinID, unsigned int WinWidth, unsigned int WinHeight, unsigned int WinPosX, unsigned int WinPosY, const std::string& name, GLFWmonitor* monitor, glm::vec4 bgColor)
    :wndID(WinID), WinWidth(WinWidth), WinHeight(WinHeight), lastX(WinWidth / 2.0f), lastY(WinHeight / 2.0f), camera(Camera(camInitPosition)), bgColor(bgColor)
{
    window = glfwCreateWindow(WinWidth, WinHeight, name.c_str(), monitor, NULL);
    if (window == NULL)
    {
        Console::red() << "Exception: Failed to create GLFW window" << std::endl;
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
        Console::red() << "Exception: Failed to initialize GLAD" << std::endl;
        return;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // Set the user pointer to this instance so that it can be accessed in the static wrapper functions.
    glfwSetWindowUserPointer(window, this);
    Console::green() << "GLFW window created successfully: " << window << std::endl;
}

GWindow::~GWindow()
{
    Console::yellow() << "Window deleting: " << window << std::endl;
}

void GWindow::Close()
{
    glObjects.clear();
}

void GWindow::addGLObject(const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState, Shader* shaderProgPtr, const std::shared_ptr<texture> texture, bool linePolygonMode)
{
    glfwMakeContextCurrent(window);
    GLObject newGLObject(objVBO, objEBO, objState, linePolygonMode);
    newGLObject.setupArUcoPtr(std::shared_ptr<ArucoThreadWrapper>(&arucoThreadWrapper, [](ArucoThreadWrapper*) {})); // Create a shared_ptr to ArucoThreadWrapper directly, without triggering the copy constructor

    if (shaderProgPtr != nullptr)
    {
        newGLObject.setupShaderProgram(shaderProgPtr);
    }
    else
    {
        Console::red() << "Invalid shaderProg pointer!" << std::endl;
        throw std::runtime_error("Invalid shaderProg pointer!");   //Refactoring !!! Add tray/catch
    }

    if (texture != nullptr)
    {
        newGLObject.setupTexture(texture);
    }
    else
    {
        Console::red() << "Invalid texture pointer!" << std::endl;
        throw std::runtime_error("Invalid texture pointer!");   //Refactoring !!! Add tray/catch
    }
    glObjects.push_back(std::move(newGLObject));
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

    // Render object method reqaer camera veuw matrix and printState functions in some caces
    // create alias for lambda function
    using RenderCallback = std::function<void(const cv::Mat&, cv::Size)>;

    // define lambda f()
    RenderCallback renderCallback = [&](const cv::Mat& frame, cv::Size frameSize) {
        showInFrame(frame, cv::Size(WinWidth, WinHeight), frameSize, RTCounter::getFPS(wndID),
            { RTCounter::getDeltaTime(wndID),
            RTCounter::getDeltaTime((4 * 1) + wndID),
            RTCounter::getDeltaTime((4 * 2) + wndID),
            RTCounter::getDeltaTime((4 * 3) + wndID),
            RTCounter::getDeltaTime((4 * 4) + wndID),
            RTCounter::getDeltaTime((4 * 5) + wndID),
            RTCounter::getDeltaTime((4 * 6) + wndID),
            RTCounter::getDeltaTime((4 * 7) + wndID),
            RTCounter::getDeltaTime((4 * 8) + wndID),
            RTCounter::getDeltaTime((4 * 9) + wndID),
            RTCounter::getDeltaTime((4 * 10) + wndID),
            RTCounter::getDeltaTime((4 * 11) + wndID)
            }
        );
    };

    for (GLsizei index{ 0 }; index < glObjects.size(); index++)
    {
        RTCounter::startTimer((index + 1) * 4 + wndID);      // For debugging perfomance, remove it!!!

        glObjects[index].renderObject(camera, renderCallback);

        RTCounter::stopTimer((index + 1) * 4 + wndID);      // For debugging perfomance, remove it!!!
    }
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();

    RTCounter::stopTimer(wndID);
}

void GWindow::showInFrame(const cv::Mat& frame, cv::Size WindSize, cv::Size frameSize, float FPS, std::initializer_list<float> dTimes)
{
    // Lock the mutex before accessing the frame
    std::lock_guard<std::mutex> lock(frameMutex);

    std::ostringstream vector_to_marker;

    vector_to_marker.str(std::string());
    vector_to_marker << std::setprecision(4) << "WindwRes: " << std::setw(2) << WindSize.width << " x " << WindSize.height;
    cv::putText(frame, vector_to_marker.str(), cv::Point(10, 25), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(32, 32, 240), 2);

    vector_to_marker.str(std::string());
    vector_to_marker << std::setprecision(4) << "FrameRes: " << std::setw(4) << frameSize.width << " x " << frameSize.height;
    cv::putText(frame, vector_to_marker.str(), cv::Point(250, 25), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(32, 240, 32), 2);

    vector_to_marker.str(std::string());
    vector_to_marker << std::setprecision(4) << "FPS: " << std::setw(6) << FPS;
    cv::putText(frame, vector_to_marker.str(), cv::Point(500, 25), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(240, 32, 32), 2);

    int shift{ 0 };
    int timerId{ 1 };
    for (float dTime : dTimes)
    {
        vector_to_marker.str(std::string());
        vector_to_marker << std::setprecision(4) << "Timer " << std::setw(2) << timerId++ <<":  " << std::setw(1) << dTime;
        cv::putText(frame, vector_to_marker.str(), cv::Point(10, 50 + shift), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(220, 16, 220), 2);
        shift += 25;
    }
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
    try
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
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            ScreanShot::makeAndSave(wndID, glObjects.at(0).getTextureImage());
    }
    catch (const std::exception& e)
    {
        Console::red() << "Exception: " << e.what() << std::endl;
        return;
    }
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