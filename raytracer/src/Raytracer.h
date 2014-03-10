#include "Scene.h"
#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <vector>
#include "Ray.h"
#include "Sphere.h"

using std::vector;

class Raytracer
{
private:
	Scene* _scene;
	vector<Sphere> _spheres;
	vector<Light> _lights;

	vector<unsigned char> _pixels;

	vec3 _transformedRayPoint;
	vec3 _transformedRayDirection;

public:
	Raytracer(Scene* scene);

	void Raytrace();

	bool Intersect(Ray ray, Sphere& sphere, vec3& normal, vec3& intersectionPoint, int depth);
	bool Intersect(Ray ray, vec3 lightPosition);

	unsigned char* GetData();

private:
	vec3 Raytrace(Ray ray, int depth);
};

#endif