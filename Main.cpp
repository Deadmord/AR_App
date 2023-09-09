#pragma once
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include <peak/peak.hpp>

#include "shader.h"
#include "geometryData.h"
#include "Structs.h"
#include "GWindowCore.h"

//*********************** Settings ***********************
// ---------------------- Cameras ------------------------
const int defaultCamera = 1;
const int usbCamera = 0;
// ----------------------- ArUco -------------------------

// -------------- Windows background color ----------------
const glm::vec4 BG_CLR_W1 = glm::vec4(0.2f, 0.3f, 0.2f, 1.0f);	//window 1 
const glm::vec4 BG_CLR_W2 = glm::vec4(0.2f, 0.2f, 0.3f, 1.0f);	//window 2 
const glm::vec4 BG_CLR_W3 = glm::vec4(0.3f, 0.2f, 0.2f, 1.0f);	//window 3 
const glm::vec4 BG_CLR_W4 = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);	//window 4 

int main()
{

    peak::Library::Initialize();
    std::cout << "Library successfully initialized!" << std::endl;
    auto peakVersion = peak::Library::Version();
    std::cout << "peak version: " << peakVersion.ToString() << std::endl;
    auto& deviceManager = peak::DeviceManager::Instance();
    deviceManager.Update();
    auto devices = deviceManager.Devices();
    auto device = devices.at(0)->OpenDevice(peak::core::DeviceAccessType::Control);

    peak::Library::Close();

    ArUcoSettings arUcoSettings;
    arUcoSettings.markerLength = 1.0f;
    arUcoSettings.dictionaryId = cv::aruco::DICT_6X6_250;
    arUcoSettings.defaultCameraParams = "camera_params/camera_parameters_2.yml";
    arUcoSettings.usbCameraParams = "camera_params/camera_paramsUSB01.yml";
    arUcoSettings.showRejected = true;

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
    GWindowCore window1(0, data.monitor_Width / 2, data.monitor_Height / 2, data.monitor_X, data.monitor_Y, "OpenGL window 1", NULL, BG_CLR_W1);
    GWindowCore window2(1, data.monitor_Width / 2, data.monitor_Height / 2, data.monitor_X + data.monitor_Width / 2, data.monitor_Y, "OpenGL window 2", NULL, BG_CLR_W2);
    GWindowCore window3(2, data.monitor_Width / 2, data.monitor_Height / 2, data.monitor_X, data.monitor_Y + data.monitor_Height / 2, "OpenGL window 3", NULL, BG_CLR_W3);

    //***************************** Shaders *****************************
    // 
        // build and compile our shader programs
    // ------------------------------------
    Shader shaderProgObjWin_1(window1, "shaderObj.vs", "shader.fs");
    Shader shaderProgBgrWin_1(window1, "shaderBgr.vs", "shader.fs");
    Shader shaderProgObjWin_2(window2, "shaderObj.vs", "shader.fs");
    Shader shaderProgBgrWin_2(window2, "shaderBgr.vs", "shader.fs");
    Shader shaderProgObjWin_3(window3, "shaderObj.vs", "shader.fs");
    Shader shaderProgBgrWin_3(window3, "shaderBgr.vs", "shader.fs");

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
    window1.addGeometryBuffers(3);
    window1.setupGeometryObject(0, verticesSurfFull, indicesSurf, initStateSurfFullScr);
    window1.setupGeometryObject(1, verticesCube, indicesCube, initStateCubes);
    window1.setupGeometryObject(2, verticesOrigin, indicesOrigin, initStateOrigin);

    window2.addGeometryBuffers(3);
    window2.setupGeometryObject(0, verticesSurfHalf, indicesSurf, initStateSurfTwoHalfC2);
    window2.setupGeometryObject(1, verticesCube, indicesCube, initStateCubes);
    window2.setupGeometryObject(2, verticesOrigin, indicesOrigin, initStateOrigin);

    window3.addGeometryBuffers(3);
    window3.setupGeometryObject(0, verticesSurfFull, indicesSurf, initStateSurfFullScr);
    window3.setupGeometryObject(1, verticesCube, indicesCube, initStateCube);
    window3.setupGeometryObject(2, verticesOrigin, indicesOrigin, initStateOrigin);

    // ------------------------------------ 
    window1.textureManager.setupVideoTexture(0, defaultCamera, GL_RGB, GL_BGR, arUcoSettings, true, false, arUcoSettings.defaultCameraParams);                           // set camera stream for virtual screans
    window1.textureManager.setupImgTexture(2, std::string("img/white.jpg"), GL_RGB, GL_RGB);
    window1.textureManager.setupVideoTexture(1, std::string("video/lines(540p).mp4"), GL_RGB, GL_BGR, arUcoSettings, false, true, ""); //set video texture for cube object

    window2.textureManager.setupVideoTexture(0, defaultCamera, GL_RGB, GL_BGR, arUcoSettings, true, false, arUcoSettings.defaultCameraParams);                           // set camera stream for virtual screans
    window2.textureManager.setupVideoTexture(1, std::string("video/video (1080p).mp4"), GL_RGB, GL_BGR, arUcoSettings, false, true, ""); //set video texture for cube object
    window2.textureManager.setupImgTexture(2, std::string("img/white.jpg"), GL_RGB, GL_RGB);

    window3.textureManager.setupVideoTexture(0, usbCamera, GL_RGB, GL_BGR, arUcoSettings, true, false, arUcoSettings.usbCameraParams);                           // set camera stream for virtual screans
    window3.textureManager.setupImgTexture(1, std::string("img/white.jpg"), GL_RGB, GL_RGB, false, true);
    window3.textureManager.setupImgTexture(2, std::string("img/white.jpg"), GL_RGB, GL_RGB);

    // ------------------------------------
    window1.setupShaderProgram(0, &shaderProgBgrWin_1);
    window1.setupShaderProgram(1, &shaderProgObjWin_1);
    window1.setupShaderProgram(2, &shaderProgObjWin_1);

    window2.setupShaderProgram(0, &shaderProgBgrWin_2);
    window2.setupShaderProgram(1, &shaderProgObjWin_2);
    window2.setupShaderProgram(2, &shaderProgObjWin_2);

    window3.setupShaderProgram(0, &shaderProgBgrWin_3);
    window3.setupShaderProgram(1, &shaderProgObjWin_3);
    window3.setupShaderProgram(2, &shaderProgObjWin_3);

    while (!glfwWindowShouldClose(window1) && !glfwWindowShouldClose(window2) && !glfwWindowShouldClose(window3))
    {
        window1.renderFrame(RTCounter::getDeltaTime());
        window2.renderFrame(RTCounter::getDeltaTime());
        window3.renderFrame(RTCounter::getDeltaTime());

        RTCounter::updateTimer();
        RTCounter::printFPS_Console();
    }

    // ------------------ check if auto caled and !!! include in destructor !!! ----------------
    //vid_captureCamera.release();
    //vid_captureVideo.release();
    
	return 0;
}
