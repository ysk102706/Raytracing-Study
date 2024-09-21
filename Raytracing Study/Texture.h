#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
using namespace glm;

class Texture
{
public:
	int width, height, channels;
	std::vector<uint8_t> image;

	Texture(const std::string& filename);
	Texture(const int& width, const int& height, const std::vector<vec3>& pixels);

	vec3 GetWrapped(int i, int j)
	{
		i %= width;
		j %= height;

		if (i < 0) i += width;
		if (j < 0) j += height;

		float r = image[(i + width * j) * channels + 0] / 255.0f;
		float g = image[(i + width * j) * channels + 1] / 255.0f;
		float b = image[(i + width * j) * channels + 2] / 255.0f;

		return vec3(r, g, b);
	}

	vec3 InterpolateBilinear(float& dx, float& dy, const vec3& v00, const vec3& v10, const vec3& v01, const vec3& v11)
	{
		vec3 a = v00 * (1 - dx) + v10 * dx;
		vec3 b = v01 * (1 - dx) + v11 * dx;
		return a * (1 - dy) + b * dy;
	}

	vec3 SamplePoint(vec2& uv)
	{
		vec2 xy = uv * vec2(width, height);

		int i = xy.x;
		int j = xy.y;

		return GetWrapped(i, j);
	}

	vec3 SampleLinear(vec2& uv)
	{
		vec2 xy = uv * vec2(width, height) - vec2(0.5f);
		int i = floor(xy.x);
		int j = floor(xy.y);
		float dx = xy.x - i;
		float dy = xy.y - j;

		return InterpolateBilinear(dx, dy, GetWrapped(i, j), GetWrapped(i + 1, j), GetWrapped(i, j + 1), GetWrapped(i + 1, j + 1));
	}
};