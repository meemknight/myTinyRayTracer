#pragma once
#include <glm/glm.hpp>
#include <limits.h>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/transform.hpp>


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


//https://gamedev.stackexchange.com/questions/22204/from-normal-to-rotation-matrix#:~:text=Therefore%2C%20if%20you%20want%20to,the%20first%20and%20second%20columns.
inline glm::mat3 NormalToRotation(glm::vec3 normal)
{
	// Find a vector in the plane
	glm::vec3 tangent0 = glm::cross(normal, glm::vec3(1, 0, 0));
	if (dot(tangent0, tangent0) < 0.001)
		tangent0 = glm::cross(normal, glm::vec3(0, 1, 0));
	tangent0 = glm::normalize(tangent0);
	// Find another vector in the plane
	glm::vec3 tangent1 = glm::normalize(cross(normal, tangent0));
	// Construct a 3x3 matrix by storing three vectors in the columns of the matrix

	return glm::mat3x3(-tangent1, -tangent0, -normal);

	//return ColumnVectorsToMatrix(tangent0, tangent1, normal);
}



inline glm::vec3 fromAnglesToDirection(float zenith, float azimuth)
{
	glm::vec4 vec(0, 0, 1, 0);

	auto zenithRotate = glm::rotate(-zenith, glm::vec3(1.f, 0.f, 0.f));
	vec = zenithRotate * vec;

	auto azimuthRotate = glm::rotate(-azimuth, glm::vec3(0.f, 1.f, 0.f));
	vec = azimuthRotate * vec;

	return glm::normalize(glm::vec3(vec));
}