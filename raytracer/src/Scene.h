#ifndef SCENE_H
#define SCENE_H

#include "glm/glm.hpp"
#include <vector>
#include <string>
#include "Sphere.h"
#include "Light.h"

using std::vector;
using std::string;
using glm::vec3;

class Scene
{
private:
	double _near;
	double _left;
	double _right;
	double _bottom;
	double _top;

	int _width;
	int _height;

	vector<Sphere> _spheres;
	vector<Light> _lights;

	vec3 _background;
	vec3 _ambient;

	string _outputName;

public:
	Scene(string filename);

	double GetNear() { return _near; }
	double GetLeft() { return _left; }
	double GetRight() const { return _right; }
	double GetBottom() const { return _bottom; }
	double GetTop() const { return _top; }
	int GetWidth() { return _width; }
	int GetHeight() { return _height; }
	vector<Sphere> GetSpheres() { return _spheres; }
	vector<Light> GetLights() { return _lights; }
	vec3 GetBackround() { return _background; }
	vec3 GetAmbient() { return _ambient; }
	string GetOutputName() { return _outputName; }
};

#endif