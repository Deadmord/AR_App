#include "GWindowCore.h"

//линзы 120 градусов, линзы м12, IDS/IDC камеры + SDK IDS peak?
// +документация
//боковые углы на маске 70 градусов, TRS матрица
//углы глаза должны совпадать с углами камерами
//получить изображение на устройстве с одной камеры
//профайлер посмотреть для определения сколько ресурсов тратится GPU, CPU 

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
    frameManager.renderFrame(deltaTime, textureManager, inputHandler, wndID, window, bgColor, WinWidth, WinHeight);
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