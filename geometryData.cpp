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
std::vector<InitState> initStateSurfTwoHalfC1 =
{
    {   glm::vec3(-0.5f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -4.0f,  0.0f, glm::vec3(1.0f, 1.0f,  1.0f)    },
    {   glm::vec3(0.5f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f),  4.0f,  0.0f, glm::vec3(0.6f, 0.6f,  1.4f)    }
};
std::vector<InitState> initStateSurfTwoHalfC2 =
{
    {   glm::vec3(-0.5f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -4.0f,  0.0f, glm::vec3(1.4f, 0.8f,  0.8f)    },
    {   glm::vec3(0.5f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f),  4.0f,  0.0f, glm::vec3(0.8f, 1.4f,  0.8f)    }
};
std::vector<InitState> initStateSurfFullScr =
{
    {   glm::vec3(0.0f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,  0.0f, glm::vec3(1.0f, 1.0f,  1.0f)    }
};
std::vector<InitState> initStateSurfLeftHalf =
{
    {   glm::vec3(-0.5f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,  0.0f, glm::vec3(1.0f, 1.0f,  1.0f)    }
};
std::vector<InitState> initStateSurfRghtHalf =
{
    {   glm::vec3(0.5f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f),  0.0f,  0.0f, glm::vec3(1.0f, 1.0f,  1.0f)    }
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
std::vector<InitState> initStateCubes =
{
    {   glm::vec3( 0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),  0.0f,  0.0f, glm::vec3(1.0f, 1.0f, 1.0f)    },
    {   glm::vec3( 1.0f, 1.0f, 1.0f), glm::vec3(0.3f, 1.0f, 0.2f),  0.0f,  10.0f, glm::vec3(1.0f, 1.0f, 1.0f)    },
    {   glm::vec3( 1.0f,-1.0f, 1.5f), glm::vec3(0.8f, 0.2f, 0.5f),  45.0f, 20.0f, glm::vec3(1.0f, 0.0f, 0.0f)    },
    {   glm::vec3(-1.0f,-1.0f, 2.0f), glm::vec3(0.5f, 0.7f, 0.1f), -45.0f, 30.0f, glm::vec3(0.0f, 1.0f, 0.0f)    },
    {   glm::vec3(-1.0f, 1.0f, 2.5f), glm::vec3(0.1f, 1.0f, 0.9f),  30.0f, 45.0f, glm::vec3(3.0f, 3.0f, 3.0f)    }
};

std::vector<InitState> initStateCube =
{
    {   glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),  0.0f,  0.0f, glm::vec3(0.8f, 0.8f, 0.8f)    }
};

std::vector<int> markerIdsCubes{ 36 };
std::vector<int> markerIdsCube{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35 };

//---------------------- 03 cube_1.25 ------------------------------
std::vector<float> verticesBrdCube = {
    -0.625f, -0.625f, -0.75f,  0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
     0.625f, -0.625f, -0.75f,  1.5f,  0.5f,  0.5f,   1.0f, 0.0f,
     0.625f,  0.625f, -0.75f,  1.5f,  1.5f,  0.5f,   1.0f, 1.0f,
    -0.625f,  0.625f, -0.75f,  0.5f,  1.5f,  0.5f,   0.0f, 1.0f,

    -0.625f, -0.625f,  0.5f,  0.5f,  0.5f,  1.5f,   0.0f, 0.0f,
     0.625f, -0.625f,  0.5f,  1.5f,  0.5f,  1.5f,   1.0f, 0.0f,
     0.625f,  0.625f,  0.5f,  1.5f,  1.5f,  1.5f,   1.0f, 1.0f,
    -0.625f,  0.625f,  0.5f,  0.5f,  1.5f,  1.5f,   0.0f, 1.0f,

    -0.625f,  0.625f,  0.5f,  0.5f,  1.5f,  1.5f,   1.0f, 0.0f,
    -0.625f,  0.625f, -0.75f,  0.5f,  1.5f,  0.5f,   1.0f, 1.0f,
    -0.625f, -0.625f, -0.75f,  0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
    -0.625f, -0.625f,  0.5f,  0.5f,  0.5f,  1.5f,   0.0f, 0.0f,

     0.625f,  0.625f,  0.5f,  1.5f,  1.5f,  1.5f,   1.0f, 0.0f,
     0.625f,  0.625f, -0.75f,  1.5f,  1.5f,  0.5f,   1.0f, 1.0f,
     0.625f, -0.625f, -0.75f,  1.5f,  0.5f,  0.5f,   0.0f, 1.0f,
     0.625f, -0.625f,  0.5f,  1.5f,  0.5f,  1.5f,   0.0f, 0.0f,

    -0.625f, -0.625f, -0.75f,  0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
     0.625f, -0.625f, -0.75f,  1.5f,  0.5f,  0.5f,   1.0f, 1.0f,
     0.625f, -0.625f,  0.5f,  1.5f,  0.5f,  1.5f,   1.0f, 0.0f,
    -0.625f, -0.625f,  0.5f,  0.5f,  0.5f,  1.5f,   0.0f, 0.0f,

    -0.625f,  0.625f, -0.75f,  0.5f,  1.5f,  0.5f,   0.0f, 1.0f,
     0.625f,  0.625f, -0.75f,  1.5f,  1.5f,  0.5f,   1.0f, 1.0f,
     0.625f,  0.625f,  0.5f,  1.5f,  1.5f,  1.5f,   1.0f, 0.0f,
    -0.625f,  0.625f,  0.5f,  0.5f,  1.5f,  1.5f,   0.0f, 0.0f,
};

std::vector<unsigned int> indicesBrdCube = {  // note that we start from 0!
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
std::vector<InitState> initStateBrdCube =
{
    {   glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f),  0.0f,  0.0f, glm::vec3(0.8f, 0.8f, 0.8f)    }
};

std::vector<int> markerIdsBrdCube{ 45,46,47,48,49,50 };

//--------------------------------------- 04 wall texture -------------------------------------------------- 
std::vector<float> verticesSurfWall = {
    -8.0f,  -5.25f,  0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
    8.0f,   -5.25f,  0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
    8.0f,   5.25f,   0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
    -8.0f,  5.25f,   0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
};
std::vector<unsigned int> indicesSurfWall = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3,    // second triangle
};
//--------------- positions ------------------ axisRotation ------ angle -- speed ------- colorMask --------------
std::vector<InitState> initStateSurfWall =
{
    {   glm::vec3(0.0f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,  0.0f, glm::vec3(1.0f, 1.0f,  1.0f)    }
};
std::vector<int> markerIdsSurfWall_39{ 39 };
std::vector<int> markerIdsSurfWall_40{ 40 };
std::vector<int> markerIdsSurfWall_41{ 41 };
std::vector<int> markerIdsSurfWall_42{ 42 };

//--------------------------------------- 05 face texture -------------------------------------------------- 
std::vector<float> verticesSurfFace = {
    -2.3f,  -2.5f,  0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
    2.3f,   -2.5f,  0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
    2.3f,   2.5f,   0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
    -2.3f,  2.5f,   0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
};
std::vector<unsigned int> indicesSurfFace = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3,    // second triangle
};
//--------------- positions ------------------ axisRotation ------ angle -- speed ------- colorMask --------------
std::vector<InitState> initStateSurfFace =
{
    {   glm::vec3(0.0f, 2.5f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,  0.0f, glm::vec3(1.0f, 1.0f,  1.0f)    }
};
std::vector<int> markerIdsSurfFace_37{ 37 };
std::vector<int> markerIdsSurfFace_38{ 38 };