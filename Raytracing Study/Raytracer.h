#pragma once
#include "Sphere.h"
#include "Ray.h"
#include "Light.h"
#include "Triangle.h"
#include "Square.h" 
#include <vector> 
using namespace glm;
using namespace std;

class Raytracer
{
public:
	int width, height;
	Light light;
	vector<shared_ptr<Object>> objects;

	Raytracer(int& width, int& height) : width(width), height(height)
	{
		auto sphere1 = make_shared<Sphere>(vec3(0.3f, -0.5f, 2.25f), 1.0f);

		sphere1->amb = vec3(1.0f, 0.0f, 0.0f);
		sphere1->diff = vec3(0.0f);
		sphere1->spec = vec3(0.0f);
		sphere1->alpha = 50.0f;
		sphere1->reflection = 0.5f;
		sphere1->transparency = 0.1f;

		objects.push_back(sphere1); 

		auto sphere2 = make_shared<Sphere>(vec3(-1.75f, -0.6f, 2.0f), 0.9f);

		sphere2->amb = vec3(0.2f);
		sphere2->diff = vec3(0.0f);
		sphere2->spec = vec3(0.0f);
		sphere2->alpha = 50.0f;
		sphere2->reflection = 0.0f;
		sphere2->transparency = 0.9f;

		objects.push_back(sphere2);


		CubeMap();

		light = Light{ {0.4f, 6.5f, 9.5f} };
	}

	Hit FindClosestCollision(Ray& ray)
	{
		float d = 1000;
		Hit closestHit = Hit{ -1, dvec3(0), dvec3(0) };

		for (auto obj : objects)
		{
			auto hit = obj->CheckRayCollision(ray);

			if (hit.d >= 0 && hit.d < d) {
				d = hit.d;
				closestHit = hit;
				closestHit.obj = obj;

				closestHit.uv = hit.uv;
			}
		}

		return closestHit;
	}

	vec3 traceRay(Ray& ray, int recursiveLevel)
	{
		if (recursiveLevel < 0) return vec3(0);

		auto hit = FindClosestCollision(ray);

		if (hit.d >= 0)
		{
			vec3 color(0);
			vec3 phongColor(0);

			vec3 DirToLight = glm::normalize(light.pos - hit.point);

			Ray ShadowRay{ hit.point + DirToLight * 1e-4f, DirToLight };
			Hit ShadowHit = FindClosestCollision(ShadowRay);
			if (ShadowHit.d < 0 || ShadowHit.d > glm::length(light.pos - hit.point) || hit.obj == ShadowHit.obj) {
				float diff = glm::max(dot(hit.normal, DirToLight), 0.0f);

				vec3 ReflectDir = 2 * glm::dot(DirToLight, hit.normal) * hit.normal - DirToLight;
				float spec = glm::pow(glm::max(glm::dot(DirToLight, -ray.dir), 0.0f), hit.obj->alpha);

				if (hit.obj->ambTexture) phongColor += hit.obj->amb * hit.obj->ambTexture->SampleLinear(hit.uv);
				else phongColor += hit.obj->amb;

				if (hit.obj->diffTexture) phongColor += hit.obj->diff * hit.obj->diffTexture->SampleLinear(hit.uv);
				else phongColor += diff * hit.obj->diff;

				phongColor += hit.obj->spec * spec;
			}
			else {
				if (hit.obj->ambTexture) phongColor = glm::max(ShadowHit.obj->transparency, 0.3f) * hit.obj->amb * hit.obj->ambTexture->SampleLinear(hit.uv);
				else phongColor = glm::max(ShadowHit.obj->transparency, 0.3f) * hit.obj->amb;
			}

			color += phongColor * (1.0f - hit.obj->reflection - hit.obj->transparency);

			if (hit.obj->reflection) 
			{
				auto reflectDir = glm::normalize(2 * glm::dot(-ray.dir, hit.normal) * hit.normal + ray.dir);
				Ray reflectRay{ hit.point + reflectDir * 1e-4f, reflectDir };

				color += traceRay(reflectRay, recursiveLevel - 1) * hit.obj->reflection;
			}

			if (hit.obj->transparency)
			{
				float eta = 1.5f;
				vec3 normal = hit.normal;

				if (glm::dot(ray.dir, hit.normal) >= 0) 
				{
					eta = 1 / eta;
					normal = -normal;
				}

				const float cos1 = glm::dot(-ray.dir, normal);
				const float sin1 = glm::sqrt(1 - cos1 * cos1);
				const float sin2 = sin1 / eta;
				const float cos2 = glm::sqrt(1 - sin2 * sin2);

				const vec3 m = glm::normalize(glm::dot(-ray.dir, normal) * normal + ray.dir); 
				const vec3 a = cos2 * -normal;
				const vec3 b = sin2 * m;
				const vec3 t = glm::normalize(a + b);

				Ray transparencyRay{ hit.point + t * 1e-4f, t };
				color += traceRay(transparencyRay, recursiveLevel - 1) * hit.obj->transparency;
			}

			return color;
		}

		return vec3(0);
	}

	void Render(std::vector<glm::vec4>& pixels)
	{
		std::fill(pixels.begin(), pixels.end(), vec4(0, 0, 0, 1));
		
		vec3 eyePos(0, 0, -1.5f);

#pragma omp parallel for
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				vec3 pixelPosWorld = TransformScreenToWorld(vec2(j, i));
				Ray pixelRay{ pixelPosWorld, glm::normalize(pixelPosWorld - eyePos) };
				pixels[j + i * width] = vec4(glm::clamp(traceRay(pixelRay, 5), 0.0f, 1.0f), 1);
			}
		}
	}

	vec3 TransformScreenToWorld(vec2 pos)
	{
		float x = 2.0f / width;
		float y = 2.0f / height;
		float aspect = (float)width / height;

		return vec3((pos.x * x - 1) * aspect, -pos.y * y + 1, 0);
	}

	void CubeMap() {
		auto groundTexture = std::make_shared<Texture>("negy.jpg");

		auto ground = make_shared<Square>(vec3(-10.0f, -1.5f, 10.0f), vec3(10.0f, -1.5f, 10.0f), vec3(10.0f, -1.5f, -10.0f), vec3(-10.0f, -1.5f, -10.0f),
			vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f));

		ground->amb = vec3(1.0f);
		ground->diff = vec3(0.0f);
		ground->spec = vec3(0.0f);
		ground->alpha = 10.0f;
		ground->reflection = 0.05f;
		ground->ambTexture = groundTexture;
		ground->diffTexture = groundTexture;

		objects.push_back(ground);

		auto forwardTexture = std::make_shared<Texture>("posz.jpg");

		auto forward = make_shared<Square>(vec3(-10.0f, 15.0f, 10.0f), vec3(10.0f, 15.0f, 10.0f), vec3(10.0f, -5.0f, 10.0f), vec3(-10.0f, -5.0f, 10.0f),
			vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f));

		forward->amb = vec3(1.0f);
		forward->diff = vec3(0.0f);
		forward->spec = vec3(0.0f);
		forward->alpha = 10.0f;
		forward->reflection = 0.0f;
		forward->ambTexture = forwardTexture;
		forward->diffTexture = forwardTexture;

		objects.push_back(forward);

		auto rightTexture = std::make_shared<Texture>("posx.jpg");

		auto right = make_shared<Square>(vec3(10.0f, 15.0f, 10.0f), vec3(10.0f, 15.0f, -10.0f), vec3(10.0f, -5.0f, -10.0f), vec3(10.0f, -5.0f, 10.0f),
			vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f));

		right->amb = vec3(1.0f);
		right->diff = vec3(0.0f);
		right->spec = vec3(0.0f);
		right->alpha = 10.0f;
		right->reflection = 0.0f;
		right->ambTexture = rightTexture;
		right->diffTexture = rightTexture;

		objects.push_back(right);

		auto backTexture = std::make_shared<Texture>("negz.jpg");

		auto back = make_shared<Square>(vec3(10.0f, 15.0f, -10.0f), vec3(-10.0f, 15.0f, -10.0f), vec3(-10.0f, -5.0f, -10.0f), vec3(10.0f, -5.0f, -10.0f),
			vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f));

		back->amb = vec3(1.0f);
		back->diff = vec3(0.0f);
		back->spec = vec3(0.0f);
		back->alpha = 10.0f;
		back->reflection = 0.0f;
		back->ambTexture = backTexture;
		back->diffTexture = backTexture;

		objects.push_back(back);

		auto leftTexture = std::make_shared<Texture>("negx.jpg");

		auto left = make_shared<Square>(vec3(-10.0f, 15.0f, -10.0f), vec3(-10.0f, 15.0f, 10.0f), vec3(-10.0f, -5.0f, 10.0f), vec3(-10.0f, -5.0f, -10.0f),
			vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f));

		left->amb = vec3(1.0f);
		left->diff = vec3(0.0f);
		left->spec = vec3(0.0f);
		left->alpha = 10.0f;
		left->reflection = 0.0f;
		left->ambTexture = leftTexture;
		left->diffTexture = leftTexture;

		objects.push_back(left);

		auto topTexture = std::make_shared<Texture>("posy.jpg");

		auto top = make_shared<Square>(vec3(-10.0f, 10.0f, -10.0f), vec3(10.0f, 10.0f, -10.0f), vec3(10.0f, 10.0f, 10.0f), vec3(-10.0f, 10.0f, 10.0f),
			vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f));

		top->amb = vec3(1.0f);
		top->diff = vec3(0.0f);
		top->spec = vec3(0.0f);
		top->alpha = 10.0f;
		top->reflection = 0.0f;
		top->ambTexture = topTexture;
		top->diffTexture = topTexture;

		objects.push_back(top);
	}
};