#include "Texture.h"
#include <stb_image.h>
#include <glm/glm.hpp>


bool Texture::loadTexture(const char *file)
{
	stbi_set_flip_vertically_on_load(true);
	int channels = 0;
	data = stbi_load(file, &w, &h, &channels, 3);

	return (bool)data;
}

void Texture::freeTexture()
{
	stbi_image_free(data);
	w = 0;
	h = 0;
}

glm::vec3 Texture::clampSample(glm::vec2 uv)
{
	return clampSample(uv.x, uv.y);
}

glm::vec3 Texture::clampSample(float x, float y)
{
	if (!data) { return {}; }

	int intX = x * w;
	int intY = y * h;

	intX = glm::clamp(intX, 0, w);
	intY = glm::clamp(intY, 0, h);

	return glm::vec3(
		data[(intX + w * intY)*3 + 0] / 255.f,
		data[(intX + w * intY)*3 + 1] / 255.f,
		data[(intX + w * intY)*3 + 2] / 255.f
	);
}

glm::vec3 SkyBox::sample(glm::vec3 direction)
{
	direction = glm::normalize(direction);


	if (direction.x > 0 
		&& abs(direction.x) > abs(direction.y) 
		&& abs(direction.x) > abs(direction.z))
	{
		float augment = 1.f / direction.x;
		auto direction2 = direction * augment;

		return textures[0].clampSample((-direction2.z + 1) / 2.f, (direction2.y + 1) / 2.f);
		return {1,0,0};
	}else if (direction.x < 0 
		&& abs(direction.x) > abs(direction.y) 
		&& abs(direction.x) > abs(direction.z))
	{
		float augment = 1.f / -direction.x;
		auto direction2 = direction * augment;

		return textures[1].clampSample((direction2.z + 1) / 2.f, (direction2.y + 1) / 2.f);
		return {0.5,0,0};
	}else if (direction.y > 0
		&& abs(direction.y) > abs(direction.x)
		&& abs(direction.y) > abs(direction.z))
	{
		float augment = 1.f / direction.y;
		auto direction2 = direction * augment;

		return textures[2].clampSample((direction2.x + 1) / 2.f, (-direction2.z + 1) / 2.f);
		return {0,1,0};
	}
	else if (direction.y < 0
		&& abs(direction.y) > abs(direction.x)
		&& abs(direction.y) > abs(direction.z))
	{
		float augment = 1.f / -direction.y;
		auto direction2 = direction * augment;

		return textures[3].clampSample((direction2.x + 1) / 2.f, (direction2.z + 1) / 2.f);
		return {0,0.5,0};
	}
	else if (direction.z > 0
		&& abs(direction.z) > abs(direction.x)
		&& abs(direction.z) > abs(direction.y))
	{
		float augment = 1.f / direction.z;
		auto direction2 = direction * augment;

		return textures[4].clampSample((direction2.x + 1) / 2.f, (direction2.y + 1) / 2.f);
		return {0,0,1};
	}
	else if (direction.z < 0
		&& abs(direction.z) > abs(direction.x)
		&& abs(direction.z) > abs(direction.y))
	{
		float augment = 1.f / -direction.z;
		auto direction2 = direction * augment;

		return textures[5].clampSample((-direction2.x + 1) / 2.f, (direction2.y + 1) / 2.f);
		return {0,0,0.5};
	}



	return glm::vec3(0.1,0.1,0.1);
}
