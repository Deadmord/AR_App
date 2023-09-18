#pragma once

#include "GWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <peak/peak.hpp>
#include "stb_image.h"

#include "config.h"
#include "MonitorsManager.h"
#include "shader.h"
#include "geometryData.h"
#include "AcquisitionWorker.h"
#include "peakInitializer.h"

void appInit()
{
    // ----------------------- GLFW --------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);    // specifies whether the windowed mode window will be maximized when created.
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

//------------- helper: set geometry objects --------------
void loadObjects(GWindow& window, Shader& shaderProgObjWin, Shader& shaderProgBgrWin, int cameraId, std::string& cameraParams, bool rotateCamera = false)
{
    shaderProgObjWin.use();
    shaderProgObjWin.setInt("texture", 0);
    shaderProgBgrWin.use();
    shaderProgBgrWin.setInt("texture", 0);

    window.addGLObject(verticesSurfFull, indicesSurf, initStateSurfFullScr, &shaderProgBgrWin, std::to_string(cameraId),                GL_RGB, GL_BGR, false, rotateCamera, true, false, nullptr, cameraParams);
    window.addGLObject(verticesOrigin, indicesOrigin, initStateOrigin,      &shaderProgObjWin, std::string("img/white.jpg"),            GL_RGB, GL_RGB, false, false, false, false, nullptr, "");
    window.addGLObject(verticesCube, indicesCube, initStateCube,            &shaderProgObjWin, std::string("img/border.png"),           GL_RGB, GL_RGB, false, false, false, true, std::make_shared<std::vector<int>>(markerIdsCube), "");
    window.addGLObject(verticesBrdCube, indicesBrdCube, initStateBrdCube,   &shaderProgObjWin, std::string("img/border.png"),           GL_RGB, GL_RGB, false, false, false, true, std::make_shared<std::vector<int>>(markerIdsBrdCube), "");
    window.addGLObject(verticesCube, indicesCube, initStateCubes,           &shaderProgObjWin, std::string("video/video (1080p).mp4"),  GL_RGB, GL_BGR, false, false, false, true, std::make_shared<std::vector<int>>(markerIdsCubes), "");
    
    window.addGLObject(verticesSurfWall, indicesSurfWall, initStateSurfWall, &shaderProgObjWin, std::string("img/rome.jpg"),            GL_RGB, GL_RGB, false, false, false, true, std::make_shared<std::vector<int>>(markerIdsSurfWall_40), "");
    window.addGLObject(verticesSurfWall, indicesSurfWall, initStateSurfWall, &shaderProgObjWin, std::string("img/moon.jpg"),            GL_RGB, GL_RGB, false, false, false, true, std::make_shared<std::vector<int>>(markerIdsSurfWall_41), "");
    window.addGLObject(verticesSurfWall, indicesSurfWall, initStateSurfWall, &shaderProgObjWin, std::string("img/krasnii-kover-13.jpg"),GL_RGB, GL_RGB, false, false, false, true, std::make_shared<std::vector<int>>(markerIdsSurfWall_42), "");
    window.addGLObject(verticesSurfFace, indicesSurfFace, initStateSurfFace, &shaderProgObjWin, std::string("img/Alon.png"),            GL_RGBA, GL_RGBA, false, false, false, true, std::make_shared<std::vector<int>>(markerIdsSurfFace_37), "");
    window.addGLObject(verticesSurfFace, indicesSurfFace, initStateSurfFace, &shaderProgObjWin, std::string("img/Eres.png"),            GL_RGBA, GL_RGBA, false, false, false, true, std::make_shared<std::vector<int>>(markerIdsSurfFace_38), "");
    window.addGLObject(verticesSurfWall, indicesSurfWall, initStateSurfWall, &shaderProgObjWin, std::string("img/kot.png"),             GL_RGBA, GL_RGBA, false, false, false, true, std::make_shared<std::vector<int>>(markerIdsSurfWall_39), "");
}

int main()
{
    peakInitializer::InitializeLibraryFindDevices();
    auto workers = peakInitializer::InitializeWorkers();

    //******************** Initialisation ********************
    appInit();
    MonitorsManager monitors;
    // ------------------ Windows init ---------------------
    const MonitorData& data = monitors.getMonitor(0);
    GWindow window_1(0, data.monitor_Width/3, data.monitor_Width /3 *3/4, data.monitor_X + data.monitor_Width *1/3, data.monitor_Y, "OpenGL center left window", NULL, BG_CLR_W1);
    GWindow window_2(1, data.monitor_Width/3, data.monitor_Width /3 *3/4, data.monitor_X, data.monitor_Y, "OpenGL left window", NULL, BG_CLR_W2);
    GWindow window_3(2, data.monitor_Width/3, data.monitor_Width /3 *3/4, data.monitor_X + data.monitor_Width *2/3, data.monitor_Y, "OpenGL right window", NULL, BG_CLR_W3);
   
    //***************************** Shaders *****************************
    // 
    // build and compile our shader programs
    Shader shaderProgObjWin_1(window_1, "shaderObj.vs", "shader.fs");
    Shader shaderProgBgrWin_1(window_1, "shaderBgr.vs", "shader.fs");
    Shader shaderProgObjWin_2(window_2, "shaderObj.vs", "shader.fs");
    Shader shaderProgBgrWin_2(window_2, "shaderBgr.vs", "shader.fs");
    Shader shaderProgObjWin_3(window_3, "shaderObj.vs", "shader.fs");
    Shader shaderProgBgrWin_3(window_3, "shaderBgr.vs", "shader.fs");

    loadObjects(window_1, shaderProgObjWin_1, shaderProgBgrWin_1, usbCamera_1, usbCam01Params);
    loadObjects(window_2, shaderProgObjWin_2, shaderProgBgrWin_2, usbCamera_2, usbCam02Params, true);
    loadObjects(window_3, shaderProgObjWin_3, shaderProgBgrWin_3, usbCamera_3, usbCam03Params);

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
    peakInitializer::CloseLibrary();

	return 0;
}