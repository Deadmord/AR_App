#pragma once
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "geometryData.h"
#include <memory>

class GeometryObjects
{
public:
	GeometryObjects() {}

	// Generates OpenGL vertex array and buffer objects arrays with the specified size.
	void setSize(GLsizei& size);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    // Vertex Array Object sould has next form: float[] {PosX, PosY, PosZ, ColR, ColG, ColB, TextX, TextY} 
	void addObject(GLuint objIndex, GLsizeiptr sizeVBO, const void* dataVBO, GLsizeiptr sizeEBO, const void* dataEBO, const std::vector<InitState>& objState);

    // set up textures data
	void initObjectTexture(bool linePolygonMode = false);

	void bindVertexArray(GLsizei& objIndex);
	// move here "setupVideoTexture" from GWindow class

	std::shared_ptr <const std::vector<InitState>> getObjStatePtr(GLsizei& objIndex) const;

	GLsizei getObjSize(GLsizei& objIndex);

private:
	std::vector<GLuint> VAO;	// vertexes attribute objects GLindexes
	std::vector<GLuint> VBO;	// vertexes buffer objects GLindexes
	std::vector<GLuint> EBO;	// elements buffer objects indexes
	std::vector<GLsizei> objSizes;	//number of vertexes 
	std::vector<std::shared_ptr<const std::vector<InitState>>> objStatePtrs;	//Objects state pointer
};

