#pragma once
#include <iostream>

#include "GWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include "shader.h"
#include "geometryData.h"
#include "RTCounter.h"

struct MonitorData
{
    GLFWmonitor* monitor;
    int monitor_X, monitor_Y, monitor_Width, monitor_Height;
};

//*********************** Settings ***********************
// ----------------------- ArUco --------------------------
namespace arUcoSettingsNamespace {
    float markerLength = 1.0f;    // 0.035f;
    cv::aruco::PredefinedDictionaryType dictionaryId = cv::aruco::DICT_6X6_250;
    std::string cameraParams = "camera_params/camera_params03.yml";
    bool showRejected = true;
}
// -------------- Windows background color ----------------
const glm::vec4 BG_CLR_W1 = glm::vec4(0.2f, 0.3f, 0.2f, 1.0f);	//window 1 
const glm::vec4 BG_CLR_W2 = glm::vec4(0.2f, 0.2f, 0.3f, 1.0f);	//window 2 
const glm::vec4 BG_CLR_W3 = glm::vec4(0.3f, 0.2f, 0.2f, 1.0f);	//window 3 
const glm::vec4 BG_CLR_W4 = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);	//window 4 

int main()
{
    //******************** Initialisation ********************
    // ----------------------- GLFW --------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);    // specifies whether the windowed mode window will be maximized when created.
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // ----------------- Monitors state ----------------------
    int monitorsCount{};
    GLFWmonitor** monitorsPtr = glfwGetMonitors(&monitorsCount);
    std::cout << "Monitors curently connected: " << monitorsCount << std::endl;
    if (!monitorsPtr) {
        std::cout << "No monitors were found!" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    std::vector<MonitorData> monitors;
    for (int monitorIndex{ 0 }; monitorIndex < monitorsCount; monitorIndex++)
    {
        MonitorData data;
        data.monitor = *(monitorsPtr + monitorIndex);
        glfwGetMonitorWorkarea(data.monitor, &data.monitor_X, &data.monitor_Y, &data.monitor_Width, &data.monitor_Height);
        monitors.push_back(data);
    }

    // ------------------ Windows init ---------------------
    MonitorData& data = monitors[0];
    GWindow window_1(0, 1280, 720, data.monitor_X, data.monitor_Y, "OpenGL window 1", NULL, BG_CLR_W1);
    GWindow window_2(1, 1280, 720, data.monitor_X + data.monitor_Width/2, data.monitor_Y, "OpenGL window 2", NULL, BG_CLR_W2);
    GWindow window_3(2, 640, 480, data.monitor_X, data.monitor_Y + data.monitor_Height/2, "OpenGL window 3", NULL, BG_CLR_W3);
    
    
    //***************************** Shaders *****************************
    // 
        // build and compile our shader programs
    // ------------------------------------
    //window_1.makeContextCurrent();
    Shader shaderProgObjWin_1(window_1, "shaderObj.vs", "shader.fs");
    Shader shaderProgBgrWin_1(window_1, "shaderBgr.vs", "shader.fs");
    Shader shaderProgObjWin_2(window_2, "shaderObj.vs", "shader.fs");
    Shader shaderProgBgrWin_2(window_2, "shaderBgr.vs", "shader.fs");
    Shader shaderProgObjWin_3(window_3, "shaderObj.vs", "shader.fs");
    Shader shaderProgBgrWin_3(window_3, "shaderBgr.vs", "shader.fs");
    // ------------------------------------
    shaderProgObjWin_1.use();
    shaderProgObjWin_1.setInt("texture", 0);
    shaderProgBgrWin_1.use();
    shaderProgBgrWin_1.setInt("texture", 0);

    shaderProgObjWin_2.use();
    shaderProgObjWin_2.setInt("texture", 0);
    shaderProgBgrWin_2.use();
    shaderProgBgrWin_2.setInt("texture", 0); 

    shaderProgObjWin_3.use();
    shaderProgObjWin_3.setInt("texture", 0);
    shaderProgBgrWin_3.use();
    shaderProgBgrWin_3.setInt("texture", 0);
    // ------------------------------------
    window_1.addGeometryBuffers(3);
    window_1.setupGeometryObject(0, verticesSurfFull, indicesSurf, initStateSurfFullScr);
    window_1.setupGeometryObject(1, verticesCube, indicesCube, initStateCubes);
    window_1.setupGeometryObject(2, verticesOrigin, indicesOrigin, initStateOrigin);
    
    window_2.addGeometryBuffers(3);
    window_2.setupGeometryObject(0, verticesSurfHalf, indicesSurf, initStateSurfTwoHalfC2);
    window_2.setupGeometryObject(1, verticesCube, indicesCube, initStateCubes);
    window_2.setupGeometryObject(2, verticesOrigin, indicesOrigin, initStateOrigin);

    window_3.addGeometryBuffers(3);
    window_3.setupGeometryObject(0, verticesSurfFull, indicesSurf, initStateSurfFullScr);
    window_3.setupGeometryObject(1, verticesCube, indicesCube, initStateCube);
    window_3.setupGeometryObject(2, verticesOrigin, indicesOrigin, initStateOrigin);
    // ------------------------------------
    //window_1.makeContextCurrent();
    window_1.setupVideoTexture(0, 1, GL_RGB, GL_BGR);                           // set camera stream for virtual screans
    window_1.setupVideoTexture(1, std::string("video/lines(540p).mp4"), GL_RGB, GL_BGR); //set video texture for cube object
    window_1.setupImgTexture(2, std::string("img/white.jpg"), GL_RGB, GL_RGB);

    window_2.setupVideoTexture(0, 1, GL_RGB, GL_BGR);                           // set camera stream for virtual screans
    window_2.setupVideoTexture(1, std::string("video/video (1080p).mp4"), GL_RGB, GL_BGR); //set video texture for cube object
    window_2.setupImgTexture(2, std::string("img/white.jpg"), GL_RGB, GL_RGB);

    window_3.setupVideoTexture(0, 0, GL_RGB, GL_BGR);                           // set camera stream for virtual screans
    window_3.setupImgTexture(1, std::string("img/white.jpg"), GL_RGB, GL_RGB);
    window_3.setupImgTexture(2, std::string("img/white.jpg"), GL_RGB, GL_RGB);
    // ------------------------------------
    window_1.setupShaderProgram(0, &shaderProgBgrWin_1);
    window_1.setupShaderProgram(1, &shaderProgObjWin_1);
    window_1.setupShaderProgram(2, &shaderProgObjWin_1);

    window_2.setupShaderProgram(0, &shaderProgBgrWin_2);
    window_2.setupShaderProgram(1, &shaderProgObjWin_2);
    window_2.setupShaderProgram(2, &shaderProgObjWin_2);

    window_3.setupShaderProgram(0, &shaderProgBgrWin_3);
    window_3.setupShaderProgram(1, &shaderProgObjWin_3);
    window_3.setupShaderProgram(2, &shaderProgObjWin_3);

    while (!glfwWindowShouldClose(window_1) && !glfwWindowShouldClose(window_2) && !glfwWindowShouldClose(window_3))
    {
        window_1.renderFrame(RTCounter::getDeltaTime());
        window_2.renderFrame(RTCounter::getDeltaTime());
        window_3.renderFrame(RTCounter::getDeltaTime());

        RTCounter::updateTimer();
        RTCounter::printFPS_Console();
    }

    // ------------------ check if auto caled and !!! include in destructor !!! ----------------
    //vid_captureCamera.release();
    //vid_captureVideo.release();
    glfwTerminate();

	return 0;
}