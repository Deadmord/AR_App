#include "geometryData.h"

//***************************** Set data *****************************
    // 
    // set up vertex data (and buffer(s)) and configure vertex attributes

    //--------------------------------------- 00 camera surfece -------------------------------------------------- 
std::vector<float> verticesSurfHalf = {
    -0.5f,  -0.99f,  0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
    0.5f,   -0.99f,  0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
    0.5f,   0.99f,   0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.99f,   0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
};
std::vector<float> verticesSurfFull = {
    -1.0f,  -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
    1.0f,   -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
    1.0f,   1.0f,   0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
    -1.0f,  1.0f,   0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
};

std::vector<unsigned int> indicesSurf = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3,    // second triangle
};

//--------------- positions ------------------ axisRotation ------ angle -- speed ------- colorMask --------------
std::vector<InitState> initStateSurfW1 =
{
    {   glm::vec3(-0.5f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -4.0f,  0.0f, glm::vec3(1.2f, 0.8f,  0.8f)    },
    {   glm::vec3(0.5f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f),  4.0f,  0.0f, glm::vec3(1.2f, 1.2f,  0.8f)    }
};
std::vector<InitState> initStateSurfW2 =
{
    {   glm::vec3(-0.5f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -4.0f,  0.0f, glm::vec3(0.8f, 1.2f,  1.2f)    },
    {   glm::vec3(0.5f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f),  4.0f,  0.0f, glm::vec3(0.8f, 0.8f,  1.2f)    }
};
std::vector<InitState> initStateSurfW3 =
{
    {   glm::vec3(0.0f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,  0.0f, glm::vec3(1.0f, 1.0f,  1.0f)    }
};
std::vector<InitState> initStateSurfW4 =
{
    {   glm::vec3(-0.5f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -4.0f,  0.0f, glm::vec3(1.0f, 1.0f,  1.0f)    },
    {   glm::vec3(0.5f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f),  4.0f,  0.0f, glm::vec3(1.0f, 1.0f,  1.0f)    }
};



    //----------------------01 coourdinate system ------------------------------
std::vector<float> verticesOrigin = {
     0.0f,  0.0f,  0.0f,  0.8f,  0.0f,  0.0f,   0.0f, 0.0f,
     2.0f,  0.0f,  0.0f,  0.8f,  0.0f,  0.0f,   1.0f, 0.0f,
     2.0f,  2.0f,  0.0f,  0.8f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.0f,  2.0f,  0.0f,  0.8f,  0.0f,  0.0f,   0.0f, 1.0f,
   
     0.0f,  0.0f,  0.0f,  0.0f,  0.8f,  0.0f,   0.0f, 0.0f,
     2.0f,  0.0f,  0.0f,  0.0f,  0.8f,  0.0f,   1.0f, 0.0f,
     2.0f,  0.0f,  2.0f,  0.0f,  0.8f,  0.0f,   1.0f, 1.0f,
     0.0f,  0.0f,  2.0f,  0.0f,  0.8f,  0.0f,   0.0f, 1.0f,
   
     0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.8f,   1.0f, 0.0f,
     0.0f,  2.0f,  0.0f,  0.0f,  0.0f,  0.8f,   1.0f, 1.0f,
     0.0f,  2.0f,  2.0f,  0.0f,  0.0f,  0.8f,   0.0f, 1.0f,
     0.0f,  0.0f,  2.0f,  0.0f,  0.0f,  0.8f,   0.0f, 0.0f,
};

std::vector<unsigned int> indicesOrigin = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3,    // second triangle
    4, 5, 7,
    5, 6, 7,
    8, 9, 11,
    9, 10, 11
};
//--------------- positions ------------------ axisRotation ------ angle -- speed ------- colorMask --------------
std::vector<InitState> initStateOrigin =
{
    {   glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),  0.0f,  0.0f, glm::vec3(1.0f, 1.0f, 1.0f)    },
};


    //---------------------- 02 cube ------------------------------
std::vector<float> verticesCube = {
    -0.5f, -0.5f, -0.5f,  0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.5f,  0.5f,  0.5f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.5f,  1.5f,  0.5f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.5f,  1.5f,  0.5f,   0.0f, 1.0f,

    -0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  1.5f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.5f,  0.5f,  1.5f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.5f,  1.5f,  1.5f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.5f,  1.5f,  1.5f,   0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  0.5f,  1.5f,  1.5f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.5f,  1.5f,  0.5f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  1.5f,   0.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.5f,  1.5f,  1.5f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.5f,  1.5f,  0.5f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.5f,  0.5f,  0.5f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.5f,  0.5f,  1.5f,   0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.5f,  0.5f,  0.5f,   1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.5f,  0.5f,  1.5f,   1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  1.5f,   0.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.5f,  1.5f,  0.5f,   0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.5f,  1.5f,  0.5f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.5f,  1.5f,  1.5f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.5f,  1.5f,  1.5f,   0.0f, 0.0f,
};

std::vector<unsigned int> indicesCube = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3,    // second triangle
    4, 5, 7,
    5, 6, 7,
    8, 9, 11,
    9, 10, 11,
    12, 13, 15,
    13, 14, 15,                                                      
    16, 17, 19,                                                      
    17, 18, 19,                                                      
    20, 21, 23,                                                      
    21, 22, 23                                                       
};
//--------------- positions ------------------ axisRotation ------ angle -- speed ------- colorMask --------------
std::vector<InitState> initStateCube =
{
    {   glm::vec3(0.0f,  0.0f, 0.0f), glm::vec3(0.3f, 1.0f, 0.2f),  0.0f,  5.0f, glm::vec3(1.0f, 1.0f, 1.0f)    },
    {   glm::vec3(1.5f,  0.0f, 0.0f), glm::vec3(0.8f, 0.2f, 0.5f),  45.0f, 10.0f, glm::vec3(1.0f, 0.0f, 0.0f)    },
    {   glm::vec3(-1.5f, 0.0f, 0.0f), glm::vec3(0.5f, 0.7f, 0.1f), -45.0f, 15.0f, glm::vec3(0.0f, 1.0f, 0.0f)    },
    {   glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.1f, 1.0f, 0.9f),  30.0f, 20.0f, glm::vec3(3.0f, 3.0f, 3.0f)    }
};

std::vector<InitState> initStateCubeEmpt =
{};


