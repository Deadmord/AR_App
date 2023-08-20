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
extern std::vector<InitState> initStateSurfW1;
extern std::vector<InitState> initStateSurfW2;
extern std::vector<InitState> initStateSurfW3;
extern std::vector<InitState> initStateSurfW4;

extern std::vector<float> verticesOrigin;
extern std::vector<unsigned int> indicesOrigin;
extern std::vector<InitState> initStateOrigin;

extern std::vector<float> verticesCube;
extern std::vector<unsigned int> indicesCube;
extern std::vector<InitState> initStateCube;
extern std::vector<InitState> initStateCubeEmpt;

