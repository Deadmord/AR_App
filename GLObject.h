#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include "camera.h"
#include "shader.h"				//move it up if problem
#include "geometryObject.h"
#include "geometryData.h"
#include "texture.h"
#include "ArucoThreadWrapper.h"
#include "CascadeObjectDetector.h"
#include "YOLOObjectDetector.h"
#include "config.h"
#include "Console.h"

using PrintInFrameCallback = std::function<void(const cv::Mat& frame, cv::Size frameSize, float cameraFPS, float markerFPS)>;

class GLObject
{
public:
// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
// Vertex Array Object sould has next form: float[] {PosX, PosY, PosZ, ColR, ColG, ColB, TextX, TextY, ...}.
// Element Buffer Object should include indices of triangle vertices in the correct sequence.
	GLObject(const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState, bool linePolygonMode = false);

	// Copy constructor
	GLObject(const GLObject& other) = default;
	// Move constructor
	GLObject(GLObject&& other) noexcept = default;
	// Copy assignment operator
	GLObject& operator=(const GLObject& other) = default;
	// Move assignment operator
	GLObject& operator=(GLObject&& other) noexcept = default;

	~GLObject();

	void setupShaderProgram(Shader* shaderProgPtr);

	void setupArUcoPtr(std::shared_ptr<ArucoThreadWrapper> arucoThreadWrapper);

	void setupCascadeObjectDetector(std::shared_ptr<CascadeObjectDetector> cascadeObjectDetector);

	void setupYOLOObjectDetector(std::shared_ptr<YOLOObjectDetector> yoloObjectDetector);

	void setupTexture(const std::shared_ptr<texture> texture);

	void renderObject(Camera& camera, PrintInFrameCallback printCallback);

	void drawObject(glm::mat4& viewMat, glm::mat4& projectionMat, bool background = false);

	cv::Mat getTextureImage();

private:
	GeometryObject geometryObject_;
	Shader* shader_;
	std::shared_ptr<texture> texture_;
	std::shared_ptr<ArucoThreadWrapper> arucoThreadWrapperPtr_;
	std::shared_ptr<CascadeObjectDetector> cascadeObjectDetector_;
	std::shared_ptr<YOLOObjectDetector> yoloObjectDetector_;
};

