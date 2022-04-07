#pragma once
#include <glm/glm.hpp>
#include <limits.h>


struct Sphere
{
	
	glm::vec3 center = {};
	float radius = 0;
	glm::vec3 color = {1,1,1};
	float metallic = 0;
	float roughness = 0;


	bool rayIntersect(glm::vec3 orig, glm::vec3 dir, float &t0) const
	{
		t0 = INFINITY;

		glm::vec3 L = center - orig;
		float tca = glm::dot(L, dir);
		float d2 = glm::dot(L, L) - glm::dot(tca, tca);
		
		if (d2 > radius * radius) 
			return false;
		
		float thc = sqrtf(radius * radius - d2);
		t0 = tca - thc;
		float t1 = tca + thc;
		if (t0 < 0) t0 = t1;
		
		if (t0 < 0) 
			return false;
		else
			return true;
	}

	float rayIntersect(glm::vec3 orig, glm::vec3 dir) const
	{
		float dist = 0;
		if (rayIntersect(orig, dir, dist))
		{
			return dist;
		}
		else
		{
			return INFINITY;
		}

	}


};


