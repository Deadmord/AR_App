#pragma once

#include "GWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "config.h"
#include "MonitorsManager.h"
#include "shader.h"
#include "geometryData.h"
#include "AcquisitionWorker.h"
#include "IDSPeak.h"
#include "texture.h"
#include "videoTexture.h"
#include "imageTexture.h"

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

    // ----------------------- IDS camera --------------------------
    IDSPeak::Initialize();
}

void InitFoneTextures(std::vector<std::shared_ptr<texture>>& textures)
{
    textures.push_back(std::make_shared<videoTexture>(usbCamera_1, GL_RGB, GL_BGR, false, true, false, nullptr, usbCam01Params));
    textures.push_back(std::make_shared<videoTexture>(usbCamera_2, GL_RGB, GL_BGR, true, true, false, nullptr, usbCam02Params));
    textures.push_back(std::make_shared<videoTexture>(usbCamera_3, GL_RGB, GL_BGR, false, true, false, nullptr, usbCam03Params));
    textures.push_back(std::make_shared<videoTexture>(IDSPeak::GetWorker(),                 GL_RGB, GL_BGR, false, true, false, nullptr, idsCamParams));
}

void InitObjectTextures(std::vector<std::shared_ptr<texture>>& textures)
{
    textures.push_back(std::make_shared<imageTexture>(std::string("img/white.jpg"),         GL_RGB, GL_RGB, false, false, false,    nullptr));
    textures.push_back(std::make_shared<imageTexture>(std::string("img/border.png"),        GL_RGB, GL_RGB, false, false, true,     std::make_shared<std::vector<int>>(markerIdsCube)));
    textures.push_back(std::make_shared<imageTexture>(std::string("img/border.png"),        GL_RGB, GL_RGB, false, false, true,     std::make_shared<std::vector<int>>(markerIdsBrdCube)));
    textures.push_back(std::make_shared<videoTexture>(std::string("video/video (1080p).mp4"), GL_RGB, GL_BGR, false, false, true,   std::make_shared<std::vector<int>>(markerIdsCubes), " "));

    textures.push_back(std::make_shared<imageTexture>(std::string("img/rome.jpg"),          GL_RGB, GL_RGB, false, false, true,     std::make_shared<std::vector<int>>(markerIdsSurfWall_40)));
    textures.push_back(std::make_shared<imageTexture>(std::string("img/moon.jpg"),          GL_RGB, GL_RGB, false, false,  true,    std::make_shared<std::vector<int>>(markerIdsSurfWall_41)));
    textures.push_back(std::make_shared<imageTexture>(std::string("img/krasnii-kover-13.jpg"), GL_RGB, GL_RGB, false, false, true,  std::make_shared<std::vector<int>>(markerIdsSurfWall_42)));
    textures.push_back(std::make_shared<imageTexture>(std::string("img/Alon.png"),          GL_RGBA, GL_RGBA, false, false, true,   std::make_shared<std::vector<int>>(markerIdsSurfFace_37)));
    textures.push_back(std::make_shared<imageTexture>(std::string("img/Eres.png"),          GL_RGBA, GL_RGBA, false, false, true,   std::make_shared<std::vector<int>>(markerIdsSurfFace_38)));
    textures.push_back(std::make_shared<imageTexture>(std::string("img/kot.png"),           GL_RGBA, GL_RGBA, false, false, true,   std::make_shared<std::vector<int>>(markerIdsSurfWall_39)));
}

//------------- helper: set geometry objects --------------
void loadObjects(GWindow& window, Shader& shaderProgObjWin, Shader& shaderProgBgrWin, std::shared_ptr<texture> foneTexture, std::vector<std::shared_ptr<texture>>& objectTextures)
{
    shaderProgObjWin.use();
    shaderProgObjWin.setInt("texture", 0);
    shaderProgBgrWin.use();
    shaderProgBgrWin.setInt("texture", 0);

    window.addGLObject(verticesSurfFull, indicesSurf, initStateSurfFullScr,  &shaderProgBgrWin, foneTexture, false);
                               
   window.addGLObject(verticesOrigin, indicesOrigin, initStateOrigin,       &shaderProgObjWin, objectTextures.at(0), false);
   window.addGLObject(verticesCube, indicesCube, initStateCube,             &shaderProgObjWin, objectTextures.at(1), false);
   window.addGLObject(verticesBrdCube, indicesBrdCube, initStateBrdCube,    &shaderProgObjWin, objectTextures.at(2), false);
   window.addGLObject(verticesCube, indicesCube, initStateCubes,            &shaderProgObjWin, objectTextures.at(3), false);
                                                                                               
   window.addGLObject(verticesSurfWall, indicesSurfWall, initStateSurfWall, &shaderProgObjWin, objectTextures.at(4), false);
   window.addGLObject(verticesSurfWall, indicesSurfWall, initStateSurfWall, &shaderProgObjWin, objectTextures.at(5), false);
   window.addGLObject(verticesSurfWall, indicesSurfWall, initStateSurfWall, &shaderProgObjWin, objectTextures.at(6), false);
   window.addGLObject(verticesSurfFace, indicesSurfFace, initStateSurfFace, &shaderProgObjWin, objectTextures.at(7), false);
   window.addGLObject(verticesSurfFace, indicesSurfFace, initStateSurfFace, &shaderProgObjWin, objectTextures.at(8), false);
   window.addGLObject(verticesSurfWall, indicesSurfWall, initStateSurfWall, &shaderProgObjWin, objectTextures.at(9), false);
}

int main()
{
    //Console::test();
    //******************** Initialisation ********************
    appInit();
    std::vector<std::shared_ptr<texture>> foneTextures;
    std::vector<std::shared_ptr<texture>> objectTextures;
    InitFoneTextures(foneTextures);
    InitObjectTextures(objectTextures);

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

    loadObjects(window_1, shaderProgObjWin_1, shaderProgBgrWin_1, foneTextures.at(3), objectTextures);
    loadObjects(window_2, shaderProgObjWin_2, shaderProgBgrWin_2, foneTextures.at(1), objectTextures);
    loadObjects(window_3, shaderProgObjWin_3, shaderProgBgrWin_3, foneTextures.at(2), objectTextures);

    while (!glfwWindowShouldClose(window_1) && !glfwWindowShouldClose(window_2) && !glfwWindowShouldClose(window_3))
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(20));

        //RTCounter::StartMainTimer();
        //RTCounter::startTimer(4 * 3 +1);
        //Console::log() << std::setprecision(8);
        //float start = static_cast<float>(glfwGetTime());
        //Console::log() << "Start:\t" << start << std::endl;
        window_1.renderFrame(RTCounter::getMainDeltaTime());
        //RTCounter::StopMainTimer();
        //RTCounter::stopTimer(4 * 3 + 1);
        //float stop = static_cast<float>(glfwGetTime());
        //Console::log() << "Stop:\t" << stop << std::endl;
        //Console::log() << "Difer:\t" << stop - start << std::endl << std::endl;
        window_2.renderFrame(RTCounter::getMainDeltaTime());
        window_3.renderFrame(RTCounter::getMainDeltaTime());

        RTCounter::updateMainTimer();
        RTCounter::printMainFPS_Console();
    }

    foneTextures.clear();
    objectTextures.clear();
    glfwTerminate();
    window_1.Close();
    window_2.Close();
    window_3.Close();
    IDSPeak::CloseLibrary();

	return 0;
}