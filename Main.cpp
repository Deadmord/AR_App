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

// settings
const glm::vec4 BG_CLR_W1 = glm::vec4(0.2f, 0.3f, 0.2f, 1.0f);	//window 1 background color
const glm::vec4 BG_CLR_W2 = glm::vec4(0.2f, 0.2f, 0.3f, 1.0f);	//window 2 background color
const glm::vec4 BG_CLR_W3 = glm::vec4(0.3f, 0.2f, 0.2f, 1.0f);	//window 3 background color
const glm::vec4 BG_CLR_W4 = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);	//window 4 background color

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
    GWindow window_1(0, data.monitor_Width/2, data.monitor_Height/2, data.monitor_X, data.monitor_Y, "OpenGL window 1", NULL, BG_CLR_W1);
    GWindow window_2(1, data.monitor_Width/2, data.monitor_Height/2, data.monitor_X + data.monitor_Width/2, data.monitor_Y, "OpenGL window 2", NULL, BG_CLR_W2);
    GWindow window_3(2, data.monitor_Width/2, data.monitor_Height/2, data.monitor_X, data.monitor_Y + data.monitor_Height/2, "OpenGL window 3", NULL, BG_CLR_W3);
    
    
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


    window_1.generateGeometryBuffers(3);
    window_1.setupGeometryObject(0, 0, verticesSurf, indicesSurf, &initStateSurfW1);
    window_1.setupGeometryObject(0, 1, verticesCube, indicesCube, &initStateCube);
    window_1.setupGeometryObject(0, 2, verticesOrigin, indicesOrigin, &initStateOrigin);
    
    window_2.generateGeometryBuffers(3);
    window_2.setupGeometryObject(1, 0, verticesSurf, indicesSurf, &initStateSurfW2);
    window_2.setupGeometryObject(1, 1, verticesCube, indicesCube, &initStateCube);
    window_2.setupGeometryObject(1, 2, verticesOrigin, indicesOrigin, &initStateOrigin);

    window_3.generateGeometryBuffers(3);
    window_2.setupGeometryObject(2, 0, verticesSurf, indicesSurf, &initStateSurfW3);
    window_2.setupGeometryObject(2, 2, verticesOrigin, indicesOrigin, &initStateOrigin);

    //window_1.makeContextCurrent();
    window_1.setupVideoTexture(0, 0, GL_RGB, GL_BGR);                           // set camera stream for virtual screans
    window_1.setupVideoTexture(1, std::string("video/lines(540p).mp4"), GL_RGB, GL_BGR); //set video texture for cube object
    window_1.setupImgTexture(2, std::string("img/white.jpg"), GL_RGB, GL_RGB); 
    window_2.setupVideoTexture(0, 1, GL_RGB, GL_BGR);                           // set camera stream for virtual screans
    window_2.setupVideoTexture(1, std::string("video/video (1080p).mp4"), GL_RGB, GL_BGR); //set video texture for cube object
    window_2.setupImgTexture(2, std::string("img/white.jpg"), GL_RGB, GL_RGB);

    window_1.setupShaderProgram(0, &shaderProgBgrWin_1);
    window_1.setupShaderProgram(1, &shaderProgObjWin_1);
    window_1.setupShaderProgram(2, &shaderProgObjWin_1);
    window_2.setupShaderProgram(0, &shaderProgBgrWin_2);
    window_2.setupShaderProgram(1, &shaderProgObjWin_2);
    window_2.setupShaderProgram(2, &shaderProgObjWin_2);

    //-----------------------------------------
    cv::Mat markerImage;
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_1000);
    for (int i{ 0 }; i < 1000; i++)
    {
        cv::aruco::generateImageMarker(dictionary, i, 240, markerImage, 1);

        std::string formattedIndex = std::to_string(i);
        while (formattedIndex.length() < 3) {
            formattedIndex = "0" + formattedIndex;
        }

        std::string filename = "markers/marker" + formattedIndex + ".png";
        cv::imwrite(filename, markerImage);
        //cv::imshow(filename, markerImage);
    }
    cv::imshow("Marker N 32", markerImage);

    cv::VideoCapture vidCapture = cv::VideoCapture(1);
    cv::Mat inputImage;
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    //cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_1000);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);
    //...


    //-----------------------------------------

    while (!glfwWindowShouldClose(window_1)&&!glfwWindowShouldClose(window_2))
    {
        window_1.renderFrame(RTCounter::getDeltaTime());
        window_2.renderFrame(RTCounter::getDeltaTime());

        //-----------------------------------------
        vidCapture.read(inputImage);
        detector.detectMarkers(inputImage, markerCorners, markerIds, rejectedCandidates);
        //-----------------------------------------

        RTCounter::updateTimer();
        RTCounter::printFPS_Console();

        //-----------------------------------------
        
        for (int markerId : markerIds)
        {
            std::cout << markerId << " ";
        }
        std::cout << '\r';
        //-----------------------------------------


    }

    // ------------------ check if auto caled and !!! include in destructor !!! ----------------
    //vid_captureCamera.release();
    //vid_captureVideo.release();
    glfwTerminate();

	return 0;
}