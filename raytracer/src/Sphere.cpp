#include "Sphere.h"
#include "invert.h"
#include "MathUtils.h"

/* Creates a sphere from a stringstream containing all attributes. */
Sphere::Sphere(stringstream* ss)
{
	*ss >> _name;

	*ss >> _position.x;
	*ss >> _position.y;
	*ss >> _position.z;
	
	*ss >> _scale.x;
	*ss >> _scale.y;
	*ss >> _scale.z;

	*ss >> _colour.x;
	*ss >> _colour.y;
	*ss >> _colour.z;

	*ss >> _ambient;
	*ss >> _diffuse;
	*ss >> _specular;
	*ss >> _reflectivity;
	*ss >> _specularExponent;

	CalculateTransformationMatrices();
}

/* Calculates the transformation matrix, its inverse, and its inverse transpose */
void Sphere::CalculateTransformationMatrices()
{
	double transformation[4][4] = { 0 };

	// Create the transformation 2D array in row-major order
	transformation[0][0] = _scale.x;
	transformation[1][1] = _scale.y;
	transformation[2][2] = _scale.z;
	transformation[3][3] = 1.0f;
	transformation[3][0] = _position.x;
	transformation[3][1] = _position.y;
	transformation[3][2] = _position.z;
	
	// Create the transformation glm::mat4 in column-major order
	_transformation[0][0] = (float)transformation[0][0];
	_transformation[1][1] = (float)transformation[1][1];
	_transformation[2][2] = (float)transformation[2][2];
	_transformation[3][3] = (float)transformation[3][3];
	_transformation[0][3] = (float)transformation[3][0];
	_transformation[1][3] = (float)transformation[3][1];
	_transformation[2][3] = (float)transformation[3][2];

	// Create inverted 2D array
	double inverted[4][4];
	invert_matrix(transformation, inverted);

	// Store inverted 2D array into a 1D array (needed for glm to create mat4)
	double inverted1d[16];
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++)
			inverted1d[x + y * 4] = inverted[y][x];

	// Create the mat4s out of the 1D array
	_invertedTranspose = (glm::make_mat4(inverted1d)); 
	_invertedTransformation = glm::transpose(_invertedTranspose); 
}

/* Intersection method used by primary and reflected rays.
 * Outputs the normal and intersection point in world space. */
bool Sphere::Intersect(Ray ray, double near, vec3& normal, vec3& intersectionPoint, int depth)
{
	// Apply the inverse transformation matrix to the ray
	_transformedRayPoint = multiply(_invertedTransformation, ray.GetPoint());
	_transformedRayDirection = glm::normalize(multiplyScale(_invertedTransformation, ray.GetDirection()));

	// Calculate terms needed for quadratic solution
	double b = 2 * glm::dot(_transformedRayPoint, _transformedRayDirection);
	double sMagnitude = glm::length(_transformedRayPoint);
	double c = sMagnitude * sMagnitude - 1;

	double t0;
	double t1;

	// If there are no solutions, return false
	if (!solveQuadratic(b, sMagnitude * sMagnitude - 1, t0, t1))
		return false;

	double t = t0 < t1 ? t0 : t1;

	// Calculate the closest intersection point in object space
	vec3 point = _transformedRayPoint + (float)t * _transformedRayDirection;
	// Apply transformation matrix to the point, to put it in world space
	intersectionPoint = multiply(_transformation, point);

	// If this is a primary ray, check to make sure it's not in front of the image plane.
	if (depth == 0 && intersectionPoint.z > -near)
	{
		// Calculate further point, and check if it's in front of the image plane or not
		t = t0 > t1 ? t0 : t1;
		point = _transformedRayPoint + (float)t * _transformedRayDirection;
		intersectionPoint = multiply(_transformation, point);

		if (intersectionPoint.z > -near)
			return false;
	}

	// Apply the inverse-transpose matrix to the point in object space,
	// to get the normal in world space.
	normal = glm::normalize(multiplyScale(_invertedTranspose, point));

	return true;
}

/* Alternate intersect method, used for the shadow ray.
 * Calculates whether an intersection is in front of a light or behind it. */
bool Sphere::Intersect(Ray ray, vec3 lightPosition)
{
	// Apply the inverse transformation matrix to the ray
	_transformedRayPoint = multiply(_invertedTransformation, ray.GetPoint());
	_transformedRayDirection = glm::normalize(multiplyScale(_invertedTransformation, ray.GetDirection()));

	// Calculate terms needed for quadratic solution
	double b = 2 * glm::dot(_transformedRayPoint, _transformedRayDirection);
	double sMagnitude = glm::length(_transformedRayPoint);
	double c = sMagnitude * sMagnitude - 1;

	double t0;
	double t1;

	// If there are no solutions, return false
	if (!solveQuadratic(b, sMagnitude * sMagnitude - 1, t0, t1))
		return false;

	double t = t0 < t1 ? t0 : t1;

	// Ignore negative intersections
	if (t < 0.001)
		t = t0 > t1 ? t0 : t1;

	// Calculate the closest intersection point in object space
	vec3 point = _transformedRayPoint + (float)t * _transformedRayDirection;

	// Calculate the light's location in object space
	vec3 transformedLight = multiply(_invertedTransformation, lightPosition);
	
	// Calculate the distance from the original intersection point (the shadow
	// ray's origin) to the light.
	double distanceToLight = glm::length(transformedLight - _transformedRayPoint);
	
	// If the intersection is closer than the light, we return true, as the point is
	// in shadow.  If the point is further than the light, then we return false,
	// as the light hits the point. This is used to make sure testIllum renders properly
	return (t < distanceToLight);
}