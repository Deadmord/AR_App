#pragma once
//============================================
// The class singletone example and using it
//============================================
#include "camera.h"
#include <memory>
#include <glm/glm.hpp>

class AppState
{
public:
	static std::shared_ptr<AppState> getInstance(Camera camera = Camera(glm::vec3(0.0f, 0.0f, 1.0f)), const float lastX = 400.0f, const float lastY = 300.0f)
	{
		static std::shared_ptr<AppState> instance{ new AppState(camera, lastX, lastY) };
		return instance;
	}

	AppState(AppState const&) = delete;
	AppState& operator=(AppState const&) = delete;

private:
	explicit AppState(Camera camera, const float lastX, const float lastY) : camera(camera), lastX(lastX), lastY(lastY) {}

public:
	// camera
	Camera camera;
	float lastX;
	float lastY;
	bool firstMouse = true;
};

//------------------ Using --------------------

//const unsigned int SCR_WIDTH = 1920;
//const unsigned int SCR_HEIGHT = 1080;
//
//Camera cameraInit(glm::vec3(0.0f, 0.0f, 3.0f));
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//std::shared_ptr<AppState> appInst = AppState::getInstance(cameraInit, lastX, lastY);
//
//glm::mat4 view = appInst->camera.GetViewMatrix();