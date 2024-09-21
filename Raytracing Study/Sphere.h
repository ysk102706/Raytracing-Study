#pragma once
#include "Object.h"


class Sphere : public Object
{
public: 
	vec3 center;
	float radius;

	Sphere(vec3 center, float radius, vec3 color = vec3(1)) : center(center), radius(radius), Object(color)
	{
	}

	Hit CheckRayCollision(Ray& ray)
	{
		Hit hit = Hit{ -1, vec3(0), vec3(0) };

		const float b = glm::dot(ray.dir, ray.start - center);
		const float c = glm::dot(ray.start - center, ray.start - center) - radius * radius; 
		
		const float det = b * b - c;
		if (det >= 0) 
		{
			const float d1 = (-b + glm::sqrt(det));
			const float d2 = (-b - glm::sqrt(det));
			hit.d = glm::min(d1, d2);

			if (hit.d < 0) hit.d = glm::max(d1, d2);

			hit.point = ray.start + hit.d * ray.dir;
			hit.normal = glm::normalize(hit.point - center);
		}

		return hit;
	}
};