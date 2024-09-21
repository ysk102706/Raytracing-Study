#pragma once
#include <iostream>
#include <glm/glm.hpp>

#include "Hit.h"
#include "Ray.h"
#include "Texture.h"

class Object
{
public:
	vec3 amb = vec3(0);
	vec3 diff = vec3(0);
	vec3 spec = vec3(0);
	float alpha = 10;
	float reflection = 0;
	float transparency = 0;

	std::shared_ptr<Texture> ambTexture;
	std::shared_ptr<Texture> diffTexture;

	Object(const vec3& color = { 1, 1, 1 }) : amb(color), diff(color), spec(color)
	{
	}

	virtual Hit CheckRayCollision(Ray& ray) = 0;
};