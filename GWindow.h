#pragma once
#include "camera.h"
#include "shader.h"
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <glm/glm.hpp>
#include "stb_image.h"
#include "geometryObjects.h"
#include "geometryData.h"



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
		//  isOpened = false;
		//  isImg = false;		
		//  isVideo = false;	x
		//  isStream = false;	x
		//  filePath;			x
		//  streamIndex = 0;	x
		//  width;				x
		//  height;				x
		//  nrChannels;
		//  internalformat;		x
		//  format;				x
		//  vidCapture;			x
		//  data;


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


	}

	inline void setupImgTexture(GLuint index, const std::string& imgTexture, GLenum internalformat, GLenum format)
	{
		//  isOpened = false;	x
		//  isImg = false;		x
		//  isVideo = false;	
		//  isStream = false;	
		//  filePath;			x
		//  streamIndex = 0;	
		//  width;				x
		//  height;				x
		//  nrChannels;			x
		//  internalformat;		x
		//  format;				x
		//  vidCapture;			
		//  data;				x
		

		textures[index].isImg = true;
		textures[index].filePath = imgTexture;
		
		textures[index].data = stbi_load(imgTexture.c_str(), &textures[index].width, &textures[index].height, &textures[index].nrChannels, 0);
		textures[index].internalformat = internalformat;
		textures[index].format = format;

		//---------------------- video texture -------------------
		if (!textures[index].data)
		{
			std::cout << "Error: Img can't be open! Source: " << imgTexture << std::endl;
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalformat, textures[index].width, textures[index].height, 0, format, GL_UNSIGNED_BYTE, textures[index].data);
			glGenerateMipmap(GL_TEXTURE_2D);
			std::cout << "Img opened successfully!" << std::endl;
			textures[index].isOpened = true;
			
		}
		//stbi_image_free(data);
	}

	void setupShaderProgram(GLuint index, Shader* shaderProgPtr)
	{
		shaders[index] = shaderProgPtr;
	}

	void renderFrame(float deltaTime)
	{
		glfwMakeContextCurrent(window);

		// input hendling
		processInput(window, deltaTime);

		// rendering commands here											
		glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		for (GLsizei index{0}; index < objectListSize; index++)
		{
			shaders[index]->use();
			glBindVertexArray(GeometryObjects::VAO[index]);
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection;

			if(textures[index].isStream || textures[index].isVideo)
			{
				// checking
				if (!textures[index].vidCapture.isOpened())		//может быть заменить на textures[index].isOpened ?
				{
					std::cout << "Error: Video stream can't be open" << std::endl;
					textures[index].isOpened = false;
					break;
				}
				cv::Mat frameVideo;
				bool isSuccessStream = textures[index].vidCapture.read(frameVideo);
				if (!isSuccessStream && textures[index].isVideo)
				{
					textures[index].vidCapture.set(cv::CAP_PROP_POS_FRAMES, 0); // return to file begin
					isSuccessStream = textures[index].vidCapture.read(frameVideo);
				}
				if (!isSuccessStream)
				{
					std::cout << "Error: Video stream can't be read or disconnect! Source: " << textures[index].streamIndex << textures[index].filePath << std::endl;
					textures[index].isOpened = false;
					break;
				}
				else
				{
					glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, frameVideo.data);
				}
			}
			if (textures[index].isImg)
			{
				// checking
				if (!textures[index].isOpened)		//может быть заменить на textures[index].isOpened ?
				{
					std::cout << "Error: Img can't be open" << std::endl;
					break;
				}
				glTexImage2D(GL_TEXTURE_2D, 0, textures[index].internalformat, textures[index].width, textures[index].height, 0, textures[index].format, GL_UNSIGNED_BYTE, textures[index].data);
			}

			// ------------- render objects copies from objState list ---------------
			const std::vector<InitState>& objState = (!wndID ? *GeometryObjects::objStatePtrs0[index] 
				: wndID == 1 ? *GeometryObjects::objStatePtrs1[index] 
				: wndID == 2 ? *GeometryObjects::objStatePtrs2[index] 
				: *GeometryObjects::objStatePtrs3[index]);
			//const std::vector<InitState>& objState = (!wndID ? *GeometryObjects::objStatePtrs0[index] : *GeometryObjects::objStatePtrs1[index]);
			//const std::vector<InitState> &objState = *GeometryObjects::objStatePtrs1[index];
			const GLsizei objSize = GeometryObjects::objSize[index];
			for (GLsizei i = 0; i < objState.size(); i++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, objState[i].positions);
				float angle = objState[i].angle;
				model = glm::rotate(model, glm::radians(angle), objState[i].axisRotation);
				model = glm::rotate(model, glm::radians(objState[i].speed * (float)glfwGetTime()), objState[i].axisRotation);
				view = glm::mat4(1.0f);
				view = camera.GetViewMatrix();
				projection = glm::perspective(glm::radians(camera.Zoom), (float)WinWidth / (float)WinHeight, 0.1f, 100.0f);
				shaders[index]->setCordTrans("model", glm::value_ptr(model));
				shaders[index]->setCordTrans("view", glm::value_ptr(view));
				shaders[index]->setCordTrans("projection", glm::value_ptr(projection));
				shaders[index]->setColorMask("colorMask", objState[i].colorMask);
				glDrawElements(GL_TRIANGLES, objSize, GL_UNSIGNED_INT, 0);
			}

			if(!index) glClear(GL_DEPTH_BUFFER_BIT);	// first object is background
		}
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

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
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 1.0f));
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
};

