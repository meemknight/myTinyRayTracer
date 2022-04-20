#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct Texture
{
	
	unsigned char *data = nullptr;
	int w = 0;
	int h = 0;

	bool loadTexture(const char *file);
	void freeTexture();

	glm::vec3 clampSample(glm::vec2 uv);
	glm::vec3 clampSample(float x, float y);
};


struct SkyBox
{

	Texture textures[6];
	glm::vec3 sample(glm::vec3 direction);

};
