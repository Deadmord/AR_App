#pragma once
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "geometryData.h"
#include <memory>

class GeometryObject
{
public:
	// create boofers and bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	GeometryObject(GLsizeiptr sizeVBO, const void* dataVBO, GLsizeiptr sizeEBO, const void* dataEBO, const std::vector<InitState>& objState);

    // set up textures data
	void initObjectTexture(bool linePolygonMode = false);

	void bindVertexArray();
	// move here "setupVideoTexture" from GWindow class

	std::shared_ptr < const std::vector<InitState>> getObjStatePtr() const;

	size_t getObjSize();

private:
	GLuint VAO;	// vertexes attribute objects GLindexes
	GLuint VBO;	// vertexes buffer objects GLindexes
	GLuint EBO;	// elements buffer objects indexes
	size_t objSize;	//number of vertexes 
	std::shared_ptr<const std::vector<InitState>> objStatePtrs;	//Objects state pointer
};

