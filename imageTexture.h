#pragma once
#include "texture.h"
#include "stb_image.h"

class imageTexture : public texture
{
public:

	imageTexture(const std::string& imgTexture, GLenum internalformat, GLenum format, bool rotate = false, bool isBackground = false, bool showOnMarker = false, std::shared_ptr<std::vector<int>> markerIds = nullptr);
	
	~imageTexture();

	void start() override
	{}

	void stop() override
	{}

	uchar* getData() override;

	float getFPS() override;

	void setWidth(GLint width) override;

	void setHeight(GLint height) override;

	const std::string& getCameraParams() const;

private:
	std::string		filePath_; 
	std::string		emptyString_;
};

