#pragma once
#include <vector>
#include "Geometry.h"
#include "lights.h"
#include "Texture.h"

struct RayIntersectRezult
{
	glm::vec3 intersectPoint = {};
	int sphereId = -1;
	float distance = INFINITY;
};



struct RayTracer
{

	std::vector<Sphere> spheres;
	std::vector<PointLight> pointLights;
	glm::vec3 skyColor = {0.2, 0.7, 0.8};
	
	RayIntersectRezult rayCast(glm::vec3 origin, glm::vec3 direction);
	bool rayCastAny(glm::vec3 origin, glm::vec3 direction);
	glm::vec3 renderRay(glm::vec3 origin, glm::vec3 direction, int depth);

	SkyBox skyBox;

};