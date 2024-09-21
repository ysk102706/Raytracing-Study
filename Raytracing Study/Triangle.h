#pragma once
#include <glm/glm.hpp>
#include "Object.h"

class Triangle : public Object
{
public:
	vec3 v0, v1, v2;
	vec2 uv0, uv1, uv2;

	Triangle() : v0(vec3(0)), v1(vec3(0)), v2(vec3(0)), uv0(vec2(0)), uv1(vec2(0)), uv2(vec2(0))
	{

	}

	Triangle(vec3 v0, vec3 v1, vec3 v2, vec2 uv0 = vec2(0), vec2 uv1 = vec2(0), vec2 uv2 = vec2(0)) : v0(v0), v1(v1), v2(v2), uv0(uv0), uv1(uv1), uv2(uv2)
	{

	}

	virtual Hit CheckRayCollision(Ray& ray) {
		Hit hit = Hit{ -1, vec3(0), vec3(0) };

		vec3 point, normal;
		float t, w0, w1;
		if (IntersectRayTriangle(ray, v0, v1, v2, point, normal, t, w0, w1)) {
			hit.d = t;
			hit.point = point;
			hit.normal = normal;
			hit.uv = uv0 * w0 + uv1 * w1 + uv2 * (1 - w0 - w1);
		}

		return hit;
	}

	bool IntersectRayTriangle(
		Ray& ray, vec3 v0, vec3 v1, vec3 v2, 
		vec3& point, vec3& normal, 
		float& t, float& w0, float& w1) 
	{
		normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		if (glm::dot(-ray.dir, normal) < 0) return false;
		if (glm::abs(glm::dot(ray.dir, normal)) < 1e-2f) return false; 

		t = (glm::dot(-ray.start, normal) + glm::dot(v0, normal)) / glm::dot(ray.dir, normal);

		if (t < 0) return false;

		point = ray.start + t * ray.dir;

		vec3 cross0 = glm::cross(v1 - point, v2 - point);
		vec3 cross1 = glm::cross(v2 - point, v0 - point);
		vec3 cross2 = glm::cross(v0 - point, v1 - point);

		if (glm::dot(cross0, normal) < 0) return false;
		if (glm::dot(cross1, normal) < 0) return false;
		if (glm::dot(cross2, normal) < 0) return false;

		float a0 = glm::length(cross0) * 0.5f;
		float a1 = glm::length(cross1) * 0.5f;
		float a2 = glm::length(cross2) * 0.5f;
		float a = a0 + a1 + a2;
		
		w0 = a0 / a;
		w1 = a1 / a;

		return true;
	}
};