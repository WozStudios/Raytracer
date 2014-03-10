#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <sstream>
#include "glm/glm.hpp"

using std::string;
using std::stringstream;

using glm::vec3;

class Light
{
private:
	string _name;
	vec3 _position;
	vec3 _intensity;

public:
	/* Creates a light from a stringstream. */
	Light::Light(stringstream* ss)
	{
		*ss >> _name;

		*ss >> _position.x;
		*ss >> _position.y;
		*ss >> _position.z;

		*ss >> _intensity.x;
		*ss >> _intensity.y;
		*ss >> _intensity.z;
	}

	string GetName() { return _name; }
	vec3 GetPosition() { return _position; }
	vec3 GetIntensity() { return _intensity; }
};

#endif