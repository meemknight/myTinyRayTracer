#include "rayTracer.h"


using glm::vec3;
using glm::vec4;
using glm::normalize;
using glm::dot;

constexpr float PI = 3.1415926;

vec3 fresnelSchlickVector(float cosTheta, vec3 F0 = vec3(0.04))
{
	return F0 + (vec3(1.f) - F0) * glm::pow(glm::clamp(vec3(1.0) - cosTheta, vec3(0.f), vec3(1.f)), vec3(5.f));
}

float fresnelSchlick(float cosTheta, float F0 = 0.04)
{
	return fresnelSchlickVector(cosTheta, glm::vec3(F0)).r;
}


RayIntersectRezult RayTracer::rayCast(glm::vec3 origin, glm::vec3 direction)
{
	RayIntersectRezult rezult;

	for (int s = 0; s < spheres.size(); s++)
	{

		auto c = spheres[s].rayIntersect(origin, direction);

		if (c < rezult.distance)
		{
			rezult.sphereId = s;
			rezult.distance = c;
		}
	}

	rezult.intersectPoint = origin + ((direction * 0.999f) * rezult.distance);

	//if (direction.x < 0)
	//{
	//	rezult.intersectPoint.x = std::nexttowardf(rezult.intersectPoint.x, -INFINITY);
	//}
	//else
	//{
	//	rezult.intersectPoint.x = std::nexttowardf(rezult.intersectPoint.x, INFINITY);
	//}
	//
	//if (direction.y < 0)
	//{
	//	rezult.intersectPoint.y = std::nexttowardf(rezult.intersectPoint.y, -INFINITY);
	//}
	//else
	//{
	//	rezult.intersectPoint.y = std::nexttowardf(rezult.intersectPoint.y, INFINITY);
	//}
	//
	//if (direction.z < 0)
	//{
	//	rezult.intersectPoint.z = std::nexttowardf(rezult.intersectPoint.z, -INFINITY);
	//}
	//else
	//{
	//	rezult.intersectPoint.z = std::nexttowardf(rezult.intersectPoint.z, INFINITY);
	//}

	return rezult;
}

bool RayTracer::rayCastAny(glm::vec3 origin, glm::vec3 direction)
{
	for (int s = 0; s < spheres.size(); s++)
	{

		auto c = spheres[s].rayIntersect(origin, direction);

		if (c != INFINITY)
		{
			return true;
		}
	}

	return false;
}


//diffuse
inline glm::vec3 fLambert(glm::vec3 color)
{
	return color / PI;
}

//normal distribution function
inline float GGXTrowbridgeReitz(float alpha, glm::vec3 N, glm::vec3 H)
{
	float alpha2 = alpha * alpha;
	float nDotH = std::max(dot(N, H), 0.0000001f);
	float denominator = (nDotH * nDotH * (alpha2 - 1) + 1);
		
	return (
		alpha2/
			(PI * denominator * denominator)
		);
}

inline float SchlickBeckman(glm::vec3 X, glm::vec3 N, float alpha)
{
	float k = alpha / 2.f;

	float nDotX = std::max(dot(N, X), 0.00001f);

	float denominator = nDotX * (1 - k) + k;

	return nDotX / denominator;
}

//geometric obstruction function
inline float SchlickGGX(glm::vec3 N, glm::vec3 L, glm::vec3 V, float alpha)
{
	return SchlickBeckman(L, N, alpha) * SchlickBeckman(V, N, alpha);
}


//specular
float fCookTorrance(float roughness, vec3 N, vec3 V, vec3 L, vec3 H)
{
	float alpha = roughness * roughness;

	float nominator = GGXTrowbridgeReitz(alpha, N, H) * SchlickGGX(N, L, V, alpha); //we omit fresnell because it is already used
	
	float denominator = 4.f * std::max(dot(V, N), 0.00001f) * std::max(dot(L, N), 0.00001f);

	return nominator / denominator;
}


inline glm::vec3 BRDF(vec3 L, vec3 V, vec3 H, vec3 N, float roughness, float metallic, vec3 color)
{


	float ks = fresnelSchlick(std::max(dot(H, V), 0.f));
	float kd = (1.f-ks) * (1.f-metallic);


	glm::vec3 rez = kd * fLambert(color) + ks * fCookTorrance(roughness, N, V, L, H);

	return rez;
}

inline vec3 renderingEquation(glm::vec3 V, glm::vec3 L, glm::vec3 N, glm::vec3 baseColor, float roughness, float metallic, vec3 light)
{
	vec3 H = normalize(V + L);

	vec3 brdf = BRDF(L, V, H, N, roughness, metallic, baseColor);
	float LdotN = std::max(dot(L, N), 0.f);

	return brdf * light * LdotN;
}


glm::vec3 RayTracer::renderRay(glm::vec3 origin, glm::vec3 direction, int depth)
{

	if (depth <= 0) { return skyBox.sample(direction); }
	
	auto ray = rayCast(origin, direction);

	if (ray.distance != INFINITY)
	{

		auto &s = spheres[ray.sphereId];

		glm::vec3 sphereNormal = normalize(ray.intersectPoint - s.center);

		vec3 irradiance = {}; //ambient //todo convolute skybox

		for (auto light : pointLights)
		{

			//check if ocluded by geometry
			glm::vec3 toLight = glm::normalize(light.position - ray.intersectPoint);

			bool occluded = rayCastAny(ray.intersectPoint, toLight);
			
			for (int sphere = 0; sphere < spheres.size(); sphere++)
			{
				if (sphere == ray.sphereId) { continue; }


				auto rez = spheres[sphere].rayIntersect(ray.intersectPoint, toLight);

				if (rez != INFINITY)
				{
					occluded = true;
					break;
				}

			}

			if (occluded) { continue; }

			glm::vec3 L = normalize(light.position - ray.intersectPoint);
			glm::vec3 V = -direction;
			glm::vec3 H = normalize(L + V);
			glm::vec3 N = sphereNormal;

			irradiance += renderingEquation(V, L, N, s.color, s.roughness, s.metallic, vec3(light.intensity));
		}

		glm::vec3 R = glm::reflect(direction, sphereNormal);

		auto reflectColor = renderRay(ray.intersectPoint, sphereNormal, depth - 1);
		auto irradianceColor = s.color * irradiance;


		vec3 H = normalize(-direction + R);

		vec3 F0 = vec3(0.04);
		F0 = glm::mix(F0, s.color, s.metallic);
		auto fresnel = fresnelSchlickVector(glm::max(dot(H, -direction), 0.f), F0);

		return glm::mix(irradianceColor, reflectColor, fresnel);

	}else
	{
		return skyBox.sample(direction);
	}

}
