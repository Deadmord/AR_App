#pragma once
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "geometryData.h"
//struct Buff 
//{
//	unsigned int VBO, VAO, EBO;
//};

class GeometryObjects
{
public:
	GeometryObjects() {}

	// Generates OpenGL vertex array and buffer objects arrays with the specified size.
	static void setSize(GLsizei size);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    // Vertex Array Object sould has next form: float[] {PosX, PosY, PosZ, ColR, ColG, ColB, TextX, TextY} 
	static void addObject(GLuint wndIndex, GLuint objIndex, GLsizeiptr sizeVBO, const void* dataVBO, GLsizeiptr sizeEBO, const void* dataEBO, const std::vector<InitState>* objStatePtr);

    // set up textures data
	static void initObjectTexture(bool linePolygonMode = false);

	// move here "setupVideoTexture" from GWindow class


public:
	static std::vector<GLuint> VAO;
	static std::vector<GLuint> VBO;
	static std::vector<GLuint> EBO;
	static std::vector<GLsizei> objSize;	//number of vertexes 
	static std::vector<const std::vector<InitState>*> objStatePtrs0;
	static std::vector<const std::vector<InitState>*> objStatePtrs1;	//Рефакторинг! убрать
};

