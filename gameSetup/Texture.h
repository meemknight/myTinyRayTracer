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
	glm::ivec3 getDataUnsafe(int x, int y);
	void setDataUnsafe(int x, int y, unsigned char r, unsigned char g, unsigned char b);

	void clearAndResize(int w, int h);
};


struct SkyBox
{

	Texture textures[6];
	glm::vec3 sample(glm::vec3 direction);

};
