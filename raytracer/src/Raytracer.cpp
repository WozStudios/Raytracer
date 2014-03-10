#include "Raytracer.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <cmath>
#include <algorithm>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "invert.h"

using std::stringstream;
using std::max;
using glm::mat4;
using glm::vec3;

#define MAX_DEPTH 3

Raytracer::Raytracer(Scene* scene)
{
	_scene = scene;
	_spheres = _scene->GetSpheres();
	_lights = _scene->GetLights();

	_pixels.reserve(3 * _scene->GetWidth() * _scene->GetHeight());
}

void Raytracer::Raytrace()
{
	int nRow = _scene->GetHeight();
	int nCol = _scene->GetWidth();

	// Calculate the width and height (which are actually half the width 
	// and height of the image plane), used to transform pixels to world space
	double width = abs(_scene->GetLeft() - _scene->GetRight()) * 0.5f;
	double height = abs(_scene->GetTop() - _scene->GetBottom()) * 0.5f;

	// Get start time
	clock_t timer = clock();
	// Iterate over each row of pixels
	for (int y = 0; y < nRow; y++)
	{
		// Iterate over each pixel in current row
		for (int x = 0; x < nCol; x++)
		{
			// Calculate pixel in world space
			double u = _scene->GetLeft() + width * 2 * x / nCol;
			double v = _scene->GetTop() - height * 2 * y / nRow;
			
			// Create primary ray from current pixel
			Ray eyeRay = Ray(vec3(u, v, -_scene->GetNear()));

			// Trace ray to find the colour of the current pixel
			vec3 colour = Raytrace(eyeRay, 0);

			// Store the pixel's colour
			_pixels.push_back(unsigned char(255.0f * colour.r));
			_pixels.push_back(unsigned char(255.0f * colour.g));
			_pixels.push_back(unsigned char(255.0f * colour.b));
		}
	}

	// Calculate how long the rendering took in seconds
	timer = clock() - timer;

	std::cout << "Raytrace completed in " << ((double)timer) / CLOCKS_PER_SEC << " seconds\n";
}

// Recursive function that returns the colour from a given ray
vec3 Raytracer::Raytrace(Ray ray, int depth)
{
	// Stop recursion if we've reached the maximum depth, and return black
	if (depth > MAX_DEPTH)
		return vec3();
	
	vec3 colour = vec3();
	vec3 point;
	vec3 normal;
	Sphere sphere;

	// Check if ray intersected with any sphere
	if (Intersect(ray, sphere, normal, point, depth))
	{
		// Calculate the ambient term
		colour = (float)sphere.GetAmbient() * _scene->GetAmbient() * sphere.GetColour();

		// For each light, cast a shadow ray
		for (unsigned int i = 0; i < _lights.size(); i++)
		{
			// Calculate direction vector from the intersection point to the current light
			vec3 direction = glm::normalize(_lights[i].GetPosition() - vec3(point));
			// Create the shadow ray
			Ray shadowRay = Ray(point, direction);

			// Check if the shadow ray intersected with any sphere.
			// If not, calculate lighting
			if (!Intersect(shadowRay, _lights[i].GetPosition()))
			{
				// If normal is facing away from us, flip it.
				// This is used when the sphere is cut open by the image plane
				float nDotV = glm::dot(normal, vec3(ray.GetDirection()));
				if (nDotV > 0)
					normal *= -1;

				// Check to make sure the normal is facing the light
				float nDotL = glm::dot(normal, shadowRay.GetDirection());
				if (nDotL > 0)
				{
					// Calculate the diffuse term
					vec3 diffuse = (float)sphere.GetDiffuse() * _lights[i].GetIntensity() * nDotL * sphere.GetColour();

					// Calculate the reflection vector from the light
					vec3 lightReflection = glm::normalize(-2.0f * glm::dot(normal, shadowRay.GetDirection()) * normal + shadowRay.GetDirection());

					vec3 specular = vec3();
					
					// Check to make sure the reflection is facing us.
					// If it is, calculate the specular term 
					float rDotV = max(glm::dot(lightReflection, vec3(ray.GetDirection())), 0.0f);
					if (rDotV > 0)
						specular = (float)sphere.GetSpecular() * pow(rDotV, (float)sphere.GetSpecularExponent()) * _lights[i].GetIntensity();

					// Add diffuse and specular to ambient
					colour += diffuse + specular;
				}
			}
		}

		// Calculate reflected ray
		if (sphere.GetReflectivity() > 0.001)
		{
			// Calculate the reflected ray's direction
			vec3 direction = -2.0f * glm::dot(normal, ray.GetDirection()) * normal + ray.GetDirection();
			Ray reflectedRay = Ray(point, direction);
		
			// Recursively trace the reflected ray, to calculate its colour
			vec3 reflectedColour = Raytrace(reflectedRay, depth + 1);
		
			// Add reflected colour to the current colour
			colour += (float)sphere.GetReflectivity() * reflectedColour;
		}
	}
	else
	{
		// If there was no intersection, and this is a primary ray,
		// return the background colour. If it's a reflected ray, return black.
		if (depth == 0)
			colour = _scene->GetBackround();
		else
			colour = vec3();
	}

	// Clamp colour values
	if (colour.r > 1.0f)
		colour.r = 1.0f;
	if (colour.g > 1.0f)
		colour.g = 1.0f;
	if (colour.b > 1.0f)
		colour.b = 1.0f;

	if (colour.r < 0.0f)
		colour.r = 0.0f;
	if (colour.g < 0.0f)
		colour.g = 0.0f;
	if (colour.b < 0.0f)
		colour.b = 0.0f;

	return colour;
}

/* Main intersection function, that tests rays with every object.
 * It outputs the closest sphere, as well as the normal and intersection point. */
bool Raytracer::Intersect(Ray ray, Sphere& sphere, vec3& normal, vec3& intersectionPoint, int depth)
{
	bool intersectionFound = false;

	// Set our initial distance to negative infinity
	intersectionPoint.z = -FLT_MAX;

	// Check for intersection with each sphere
	for (unsigned int i = 0; i < _spheres.size(); i++)
	{
		vec3 tempNormal;
		vec3 tempIntersectionPoint;

		// Check for intersection with current sphere
		if (_spheres[i].Intersect(ray, _scene->GetNear(), tempNormal, tempIntersectionPoint, depth))
		{
			intersectionFound = true;

			// If the current intersection point is closer to us than
			// the previous closest one, store it as the closest one
			if (tempIntersectionPoint.z > intersectionPoint.z)
			{
				intersectionPoint = tempIntersectionPoint;
				normal = tempNormal;
				sphere = _spheres[i];
			}
		}
	}

	return intersectionFound;
}

/* Alternate, simpler intersection method, used for shadow rays.
 * returns true as soon as it finds an intersection. */
bool Raytracer::Intersect(Ray ray, vec3 lightPosition)
{
	for (unsigned int i = 0; i < _spheres.size(); i++)
	{
		if (_spheres[i].Intersect(ray, lightPosition))
		{
			return true;
		}
	}

	return false;
}

// Return an unsigned char* to the data generated by Raytrace() 
unsigned char* Raytracer::GetData()
{
	return &_pixels[0];
}