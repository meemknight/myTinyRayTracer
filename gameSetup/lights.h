#pragma once
#include <glm/glm.hpp>


struct PointLight
{
	PointLight(glm::vec3 p, float i): position(p), intensity(i) {}
	
	glm::vec3 position = {};
	float intensity = 0;

};