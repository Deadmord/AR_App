#pragma once
#include "camera.h"
#include "shader.h"
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // Äëÿ glm::value_ptr è glm::to_string
#include "stb_image.h"
#include "geometryObjects.h"
#include "geometryData.h"
#include "aruco/ArucoProcessor.h"

// declaration of global settings
namespace arUcoSettingsNamespace {
	extern float markerLength;
	extern cv::aruco::PredefinedDictionaryType dictionaryId;
	extern std::string cameraParams;
	extern bool showRejected;
}

struct TextureData
{
	GLboolean		isOpened = false;
	GLboolean		isImg = false;
	GLboolean		isVideo = false;
	GLboolean		isStream = false;
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

const int		WINDOW_PANEL_HEIGHT = 30;			// 30px for window panel
const glm::vec4 BG_CLR = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f); //default bg color

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
	void generateGeometryBuffers(GLsizei size);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	// Vertex Array Object sould has next form: float[] {PosX, PosY, PosZ, ColR, ColG, ColB, TextX, TextY, ...}.
	// Element Buffer Object should include indices of triangle vertices in the correct sequence.
	void setupGeometryObject(GLuint wndIndex, GLuint objIndex, const std::vector<float>& VBO, const std::vector<unsigned int>& EBO, const std::vector<InitState>* objStatePtr);

	// Open texture file and bind with object
	template<typename T>
	requires std::same_as<T, int> || std::same_as<T, std::string>
	inline void setupVideoTexture(GLuint index, const T & videoTexture, GLenum internalformat, GLenum format)
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
		textures[index].width = static_cast<int>(textures[index].vidCapture.get(cv::CAP_PROP_FRAME_WIDTH));
		textures[index].height = static_cast<int>(textures[index].vidCapture.get(cv::CAP_PROP_FRAME_HEIGHT));

		textures[index].internalformat = internalformat;
		textures[index].format = format;

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

		if (textures[index].isOpened && textures[index].isStream) //&& turn aruco flag
		{
			//--------------- ArUco init ======================
			arucoProcessorPtr = std::make_unique<ArucoProcessor>(arUcoSettingsNamespace::markerLength, arUcoSettingsNamespace::dictionaryId, arUcoSettingsNamespace::cameraParams, arUcoSettingsNamespace::showRejected);
			//--------------------------------------------
		}
	}

	void setupImgTexture(GLuint index, const std::string& imgTexture, GLenum internalformat, GLenum format);

	void setupShaderProgram(GLuint index, Shader* shaderProgPtr);

	void renderFrame(float deltaTime);

	void makeContextCurrent();		//remove it or make private
	//GLFWwindow* getWindowPtr();		//dont use anymore, replace with "operator GLFWwindow*"

private:
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
	std::vector<TextureData> textures;
	std::vector<Shader*> shaders;
	GLsizei objectListSize = 0;
	std::unique_ptr<ArucoProcessor> arucoProcessorPtr;
};

