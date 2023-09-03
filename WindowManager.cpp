#include "WindowManager.h"


bool WindowManager::glfwInitialized{false};
int WindowManager::windowCount{0};

WindowManager::WindowManager(unsigned int width, unsigned int height, unsigned int WinPosX, unsigned int WinPosY, const char* title, GLFWwindow* share) {
	if (!glfwInitialized) {
		if (!glfwInit()) {
			std::cerr << "Failed to initialize GLFW" << std::endl;
			exit(-1);
		}
		glfwInitialized = true;
	}

	window = glfwCreateWindow(width, height, title, nullptr, share);
	if (window == nullptr) {
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		exit(-1);
	}

	this->width = width;
	this->height = height;

	glfwMakeContextCurrent(window);
	glfwSetWindowPos(window, WinPosX, WinPosY + WINDOW_PANEL_HEIGHT);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	windowCount++;
}

void WindowManager::FramebufferSizeCallback(GLFWwindow* window, int new_width, int new_height) {
	WindowManager* wm = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
	if (wm) {
		wm->width = new_width;
		wm->height = new_height;

		glViewport(0, 0, wm->width, wm->height);
	}
}

void WindowManager::MakeContextCurrent() {
	glfwMakeContextCurrent(window);
}


bool WindowManager::Close() const {
	return glfwWindowShouldClose(window);
}

void WindowManager::SwapBuffers() {
	glfwSwapBuffers(window);
}

void WindowManager::PollEvents() {
	glfwPollEvents();
}

int WindowManager::GetWidth() const { return this->width; }
int WindowManager::GetHeight() const { return this->height; }
GLFWwindow* WindowManager::GetWindow() const { return this->window; }

WindowManager::~WindowManager() {
	if (window != nullptr) {
		glfwDestroyWindow(window);
		windowCount--;

		if (windowCount == 0 && glfwInitialized) {
			glfwTerminate();
			glfwInitialized = false;
		}
	}
}