#pragma once
#include <vector>
#include <glm/glm.hpp>

struct InitState
{
	glm::vec3	positions;
	glm::vec3	axisRotation;
	float		angle;
	float		speed;
	glm::vec3	colorMask;
};

extern std::vector<float> verticesSurfHalf;
extern std::vector<float> verticesSurfFull;
extern std::vector<unsigned int> indicesSurf;
extern std::vector<InitState> initStateSurfTwoHalfC1;
extern std::vector<InitState> initStateSurfTwoHalfC2;
extern std::vector<InitState> initStateSurfFullScr;
extern std::vector<InitState> initStateSurfLeftHalf;
extern std::vector<InitState> initStateSurfRghtHalf;

extern std::vector<float> verticesOrigin;
extern std::vector<unsigned int> indicesOrigin;
extern std::vector<InitState> initStateOrigin;

extern std::vector<float> verticesCube;
extern std::vector<unsigned int> indicesCube;
extern std::vector<InitState> initStateCubes;
extern std::vector<InitState> initStateCube;

