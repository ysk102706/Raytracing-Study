#pragma once
#include <iostream>
#include <glm/glm.hpp>
using namespace glm;

class Object;

class Hit
{
public:
	float d;
	vec3 point;
	vec3 normal;
	vec2 uv;

	std::shared_ptr<Object> obj;
};