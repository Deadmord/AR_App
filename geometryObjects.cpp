#include "geometryObjects.h"

void GeometryObjects::setSize(GLsizei& size)
{
    VAO.resize(size);
    VBO.resize(size);
    EBO.resize(size);
    objSizes.resize(size);
    objStatePtrs.resize(size);

    glGenVertexArrays(size, VAO.data());
    glGenBuffers(size, VBO.data());
    glGenBuffers(size, EBO.data());
}

void GeometryObjects::addObject(GLuint objIndex, GLsizeiptr sizeVBO, const void* dataVBO, GLsizeiptr sizeEBO, const void* dataEBO, const std::vector<InitState>& objState)
{
    glBindVertexArray(VAO[objIndex]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[objIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeVBO * sizeof(float), dataVBO, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[objIndex]);
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
    objSizes[objIndex] = sizeEBO;           // store the object size
    objStatePtrs[objIndex] = std::make_shared<const std::vector<InitState>>(objState);   // store the object state   
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

void GeometryObjects::bindVertexArray(GLsizei& objIndex)
{
    glBindVertexArray(VAO[objIndex]);
}

std::shared_ptr<const std::vector<InitState>> GeometryObjects::getObjStatePtr(GLsizei& objIndex) const
{
    return objStatePtrs[objIndex];
}

GLsizei GeometryObjects::getObjSize(GLsizei& objIndex)
{
    return objSizes[objIndex];
}
