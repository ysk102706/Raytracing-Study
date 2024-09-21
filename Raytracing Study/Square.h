#pragma once
#include <glm/glm.hpp>
#include "Triangle.h"
#include "Object.h"
using namespace glm;

class Square : public Object
{
public:
	Triangle t1, t2;

	Square(vec3 v0, vec3 v1, vec3 v2, vec3 v3, 
		vec2 uv0 = vec2(0), vec2 uv1 = vec2(0), 
		vec2 uv2 = vec2(0), vec2 uv3 = vec2(0)) : t1(v0, v1, v2), t2(v0, v2, v3)
	{
		t1.uv0 = uv0;
		t1.uv1 = uv1;
		t1.uv2 = uv2;

		t2.uv0 = uv0;
		t2.uv1 = uv2;
		t2.uv2 = uv3;
	}

	virtual Hit CheckRayCollision(Ray& ray) {
		auto hit1 = t1.CheckRayCollision(ray);
		auto hit2 = t2.CheckRayCollision(ray);

		if (hit1.d >= 0 && hit2.d >= 0) return hit1.d < hit2.d ? hit1 : hit2;
		else if (hit1.d >= 0) return hit1;
		else return hit2;
	}
};