#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "glm/glm.hpp"

using glm::mat4;
using glm::vec3;
using glm::vec3;

/* Optimized multiplication function, that only calculates the terms
 * necessary for scaling and translation. t is assumed to be a point, 
 * and so translation is applied. */
template <class T>
static vec3 multiply(mat4 mat, T t)
{
	double x = mat[0][0] * t.x + mat[0][3]; 
	double y = mat[1][1] * t.y + mat[1][3]; 
	double z = mat[2][2] * t.z + mat[2][3];

	return vec3(x, y, z);
}

/* Optimized multiplication function, that only calculates the terms
 * necessary for scaling. t is assumed to be a direction, and so 
 * translation is ignored. */
template <class T>
static vec3 multiplyScale(mat4 mat, T t)
{
	double x = mat[0][0] * t.x; 
	double y = mat[1][1] * t.y; 
	double z = mat[2][2] * t.z;

	return vec3(x, y, z);
}

/* Optimized function that solves a quadratic equation. Since in our case,
 * the a term is just the magnitude of the direction vector, we make sure to
 * normalize it, and we can then ignore it here, as it would just be 1. */
static bool solveQuadratic(double b, double c, double& t0, double& t1)
{
	double disciminant = b * b - 4 * c;

	// If the disciminant is negative, there are no real solutions
	if (disciminant < -0.001)
	{
		t0 = t1 = FLT_MAX;
		return false;
	}

	// If the disciminant is ~0, there is one solution.
	if (disciminant < 0.001)
	{
		t0 = t1 = -b / 2.0f;
	}
	else
	{
		double sqrtDiscriminant = sqrt(disciminant);

		t0 = (-b + sqrtDiscriminant) / 2.0f;
		t1 = (-b - sqrtDiscriminant) / 2.0f;
	}

	// Make sure that at least one of the solutions is positive.
	return t0 > 0.001 || t1 > 0.001;
}

#endif