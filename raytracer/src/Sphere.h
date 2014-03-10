#ifndef SPHERE_H
#define SPHERE_H

#include <sstream>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Ray.h"

using std::stringstream;
using std::string;
using glm::vec3;
using glm::mat4;

class Sphere
{
private:
	string _name;
	vec3 _position;
	vec3 _scale;
	vec3 _colour;
	double _ambient;
	double _diffuse;
	double _specular;
	double _reflectivity;
	double _specularExponent;

	mat4 _transformation;
	mat4 _invertedTransformation;
	mat4 _invertedTranspose;

	vec3 _transformedRayPoint;
	vec3 _transformedRayDirection;

public:
	Sphere() {};
	Sphere(stringstream* ss);

	bool Intersect(Ray ray, double near, vec3& normal, vec3& intersectionPoint, int depth);
	bool Intersect(Ray ray, vec3 lightPosition);
	
	string GetName() { return _name; }
	vec3 GetPosition() { return _position; }
	vec3 GetScale() { return _scale; }
	vec3 GetColour() { return _colour; }
	double GetAmbient() { return _ambient; }
	double GetDiffuse() { return _diffuse; }
	double GetSpecular() { return _specular; }
	double GetReflectivity() { return _reflectivity; }
	double GetSpecularExponent() { return _specularExponent; }

private:
	void CalculateTransformationMatrices();
};

#endif