#include "TextureManager.h"


GLuint TextureManager::LoadTextureFromFrame(const cv::Mat& frame) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	textures.push_back(textureID);

	return textureID;
}

GLuint TextureManager::LoadTextureFromFile(const std::string& filepath) {
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		std::cerr << "Failed to load texture from " << filepath << std::endl;
		return 0;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (nrChannels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else if (nrChannels == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	textures.push_back(textureID); // Добавить в список текстур

	return textureID;
}

void TextureManager::UpdateTexture(const GLuint& textureID, const cv::Mat& frame) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
}

void TextureManager::DeleteTexture(const GLuint& textureID) {
	glDeleteTextures(1, &textureID);
	textures.erase(std::remove(textures.begin(), textures.end(), textureID), textures.end());
}

TextureManager::~TextureManager() {
	for (GLuint textureID : textures) {
		glDeleteTextures(1, &textureID);
	}
}
