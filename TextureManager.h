#pragma once
#include<string>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<glad/glad.h>
#include "stb_image.h"

class TextureManager
{
private:
	std::vector<GLuint> textures;

public:
	TextureManager() {}
	~TextureManager();

	GLuint LoadTextureFromFrame(const cv::Mat& frame);
	GLuint LoadTextureFromFile(const std::string& filepath);
	void UpdateTexture(const GLuint& textureID, const cv::Mat& frame);
	void DeleteTexture(const GLuint& textureID);
};