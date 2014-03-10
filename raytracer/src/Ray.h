#ifndef RAY_H
#define RAY_H

#include "glm/glm.hpp"

using glm::vec3;

class Ray
{
private:
	vec3 _point;
	vec3 _direction;

public:
	/* Create a ray from a direction, starting at the origin. */
	Ray(vec3 direction) { _point = vec3(); _direction = glm::normalize(direction); }
	/* Create a ray from a direction and a point. */
	Ray(vec3 point, vec3 direction) { _point = point; _direction = glm::normalize(direction); }

	vec3 GetPoint() { return _point; }
	vec3 GetDirection() { return _direction; }
};

#endif