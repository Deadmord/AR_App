#include "geometryObjects.h"
std::vector<GLuint> GeometryObjects::VAO;
std::vector<GLuint> GeometryObjects::VBO;
std::vector<GLuint> GeometryObjects::EBO;
std::vector<GLsizei> GeometryObjects::objSize;
std::vector<const std::vector<InitState>*> GeometryObjects::objStatePtrs0; // !!!! Убрать это все! Костыль. Обьект должен быть одни в не зависимости от окна.
std::vector<const std::vector<InitState>*> GeometryObjects::objStatePtrs1;
std::vector<const std::vector<InitState>*> GeometryObjects::objStatePtrs2;
std::vector<const std::vector<InitState>*> GeometryObjects::objStatePtrs3;

void GeometryObjects::setSize(GLsizei size)
{
    VAO.resize(size);
    VBO.resize(size);
    EBO.resize(size);
    objSize.resize(size);
    objStatePtrs0.resize(size);         // !!!! Убрать это все! Костыль. Обьект должен быть одни в не зависимости от окна.
    objStatePtrs1.resize(size);
    objStatePtrs2.resize(size);
    objStatePtrs3.resize(size);

    glGenVertexArrays(size, VAO.data());
    glGenBuffers(size, VBO.data());
    glGenBuffers(size, EBO.data());
}

void GeometryObjects::addObject(GLuint wndIndex, GLuint objIndex, GLsizeiptr sizeVBO, const void* dataVBO, GLsizeiptr sizeEBO, const void* dataEBO, const std::vector<InitState>* objStatePtr)
{
    glBindVertexArray(GeometryObjects::VAO[objIndex]);
    glBindBuffer(GL_ARRAY_BUFFER, GeometryObjects::VBO[objIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeVBO * sizeof(float), dataVBO, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GeometryObjects::EBO[objIndex]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeEBO * sizeof(unsigned int), dataEBO, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // -------------------------------------------------------------------
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    objSize[objIndex] = sizeEBO;

    //!wndIndex ? objStatePtrs0[objIndex] = objStatePtr : objStatePtrs1[objIndex] = objStatePtr; 
    if (wndIndex == 0) objStatePtrs0[objIndex] = objStatePtr;                           // !!!! Убрать это все! Костыль. Обьект должен быть одни в не зависимости от окна.
    if (wndIndex == 1) objStatePtrs1[objIndex] = objStatePtr;
    if (wndIndex == 2) objStatePtrs2[objIndex] = objStatePtr;
    if (wndIndex == 3) objStatePtrs3[objIndex] = objStatePtr;
}

void GeometryObjects::initObjectTexture(bool linePolygonMode)
{
    unsigned int texture;

    glActiveTexture(GL_TEXTURE0);   //default value, it neccesary only for 2+ textures
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // uncomment this call to draw in wireframe polygons.
    if (linePolygonMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}
