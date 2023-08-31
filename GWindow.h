#pragma once
#include "camera.h"
#include "shader.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // For glm::value_ptr è glm::to_string
#include "stb_image.h"
#include "geometryObjects.h"
#include "geometryData.h"
#include "aruco/ArucoProcessor.h"
#include "RTCounter.h"

// declaration of global settings
namespace arUcoSettingsNamespace {
	extern float markerLength;
	extern cv::aruco::PredefinedDictionaryType dictionaryId;
	extern bool showRejected;
}

struct TextureData
{
	bool			isOpened = false;
	bool			isImg = false;
	bool			isVideo = false;
	bool			isStream = false;
	bool			isBackground = false;
	bool			showOnMarker = false;
	std::string		filePath;
	GLint			streamIndex = 0;
	GLint			width;
	GLint			height;
	GLint			nrChannels = 3;
	GLenum			internalformat;
	GLenum			format;
	cv::VideoCapture vidCapture;
	uchar*			data;
};

const int		WINDOW_PANEL_HEIGHT = 0;				// 30px for window panel
const glm::vec4 BG_CLR	(0.2f, 0.3f, 0.3f, 1.0f);		//default bg color
const glm::vec3 camInitPosition	(5.0f, 4.0f, 20.0f);	//Camera start position

class GWindow
{
public:
	GWindow(unsigned int WinID, unsigned int WinWidth, unsigned int WinHeight, unsigned int WinPosX, unsigned int WinPosY, const std::string& name, GLFWmonitor* monitor, glm::vec4 bgColor = BG_CLR);
	~GWindow(){}
	operator GLFWwindow* () const
	{
		return window;
	}

	// Generates OpenGL vertex array and buffer objects arrays with the specified size.
	void addGeometryBuffers(GLsizei size);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	// Vertex Array Object sould has next form: float[] {PosX, PosY, PosZ, ColR, ColG, ColB, TextX, TextY, ...}.
	// Element Buffer Object should include indices of triangle vertices in the correct sequence.
	void setupGeometryObject(GLuint objIndex, const std::vector<float>& objVBO, const std::vector<unsigned int>& objEBO, const std::vector<InitState>& objState);

	// Open texture file and bind with object
	template<typename T>
	requires std::same_as<T, int> || std::same_as<T, std::string>
	inline void setupVideoTexture(GLuint index, const T & videoTexture, GLenum internalformat, GLenum format, bool isBackground = false, bool showOnMarker = false, std::string cameraParams = nullptr)
	{
		if constexpr (std::is_same_v<T, int>) {
			textures[index].isStream = true;
			textures[index].streamIndex = videoTexture;
		}
		else if constexpr (std::is_same_v<T, std::string>) {
			textures[index].isVideo = true;
			textures[index].filePath = videoTexture;
		}
		//cv::VideoCapture vid_captureCamera
		textures[index].vidCapture = cv::VideoCapture(videoTexture);

		//----------- Init ArUco and set resolution -----------
		if (textures[index].isStream) //&& turn aruco flag
		{
			// ArUco init
			arucoProcessorPtr = std::make_unique<ArucoProcessor>(arUcoSettingsNamespace::markerLength, arUcoSettingsNamespace::dictionaryId, cameraParams, arUcoSettingsNamespace::showRejected);

			textures[index].vidCapture.set(cv::CAP_PROP_FRAME_WIDTH, arucoProcessorPtr->getFrameSize().width);
			textures[index].vidCapture.set(cv::CAP_PROP_FRAME_HEIGHT, arucoProcessorPtr->getFrameSize().height);
		}
		//------------------------------------------

		textures[index].width = static_cast<int>(textures[index].vidCapture.get(cv::CAP_PROP_FRAME_WIDTH));
		textures[index].height = static_cast<int>(textures[index].vidCapture.get(cv::CAP_PROP_FRAME_HEIGHT));

		textures[index].internalformat = internalformat;
		textures[index].format = format;

		textures[index].isBackground = isBackground;
		textures[index].showOnMarker = showOnMarker;

		//---------------------- video texture -------------------
		if (!textures[index].vidCapture.isOpened())
		{
			std::cout << "Error: Video stream can't be open! Source: " << videoTexture << std::endl;
		}
		else 
		{
			cv::Mat frameVideo;
			//Initialize a boolean to check whether frames are present or not
			if (!textures[index].vidCapture.read(frameVideo))
			{
				std::cout << "Error: Video stream can't be read or disconnect! Source: " << videoTexture << std::endl;
			}
			else
			{
				std::cout << "Video stream opened successfully!" << std::endl;
				textures[index].data = frameVideo.data;
				textures[index].isOpened = true;
			}
		}
	}

	void setupImgTexture(GLuint index, const std::string& imgTexture, GLenum internalformat, GLenum format, bool isBackground = false, bool showOnMarker = false);

	void setupShaderProgram(GLuint index, Shader* shaderProgPtr);

	void renderFrame(float deltaTime);

	void drowObject(GLsizei objIndex, glm::mat4& viewMat, glm::mat4& projectionMat, bool background = false)
	{
		glm::mat4 model	= glm::mat4(1.0f);

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

	void makeContextCurrent();		//remove it or make private
	//GLFWwindow* getWindowPtr();		//dont use anymore, replace with "operator GLFWwindow*"

private:
	void showInFrame(const cv::Mat& frame, cv::Size WindSize, cv::Size frameSize, float FPS, std::initializer_list<float> dTimes)
	{
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

		int shift{0};
		for (float dTime : dTimes)
		{
			vector_to_marker.str(std::string());
			vector_to_marker << std::setprecision(4) << "OperationTime: " << std::setw(4) << dTime;
			cv::putText(frame, vector_to_marker.str(), cv::Point(10, 50 + shift), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(220, 16, 220), 2);
			shift += 25;
		}
	}

	static void framebufferSizeCallbackWrapper(GLFWwindow* window, int width, int height);
	static void mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods);
	static void mouseCursorCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
	static void scrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset);

	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void mouseCursorCallback(GLFWwindow* window, double xposIn, double yposIn);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window, float deltaTime);	//remove it or make private

private:
	GLFWwindow* window;

public:												//remove it or make private
	// camera
	Camera camera;
	float lastX = 400.0f;
	float lastY = 300.0f;
	bool firstMouse = true;
	bool leftMouseButtonPressed = false;

private:
	GLuint wndID = 0;
	GLuint WinWidth = 800; 
	GLuint WinHeight = 600;
	glm::vec4 bgColor;
	GeometryObjects geometryObjects;
	std::vector<TextureData> textures;
	std::vector<Shader*> shaders;
	GLsizei objectListSize = 0;
	std::unique_ptr<ArucoProcessor> arucoProcessorPtr;
};

