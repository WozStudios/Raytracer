#include "Scene.h"
#include <iostream>
#include <fstream>
#include <sstream>

using std::ifstream;
using std::cout;
using std::cin;
using std::cerr;

/* Create a scene by parsing a test file for all scene attributes. */
Scene::Scene(string filename)
{
	// Open test file
	ifstream file(filename);

	// Return if file did not open properly
	if (!file.is_open())
	{
		cerr << "Error opening the file " << filename << "\n";
		return;
	}
	else
	{
		cout << "Parsing " << filename << "...\n";
	}

	string line;		
	while (std::getline(file, line))
	{
		std::stringstream ss;
		string key;

		ss << line;
		ss >> key;

		if (key == "")
			continue;

		// Parse near plane
		else if (key == "NEAR")
			ss >> _near;

		// Parse left plane
		else if (key == "LEFT")
			ss >> _left;

		// Parse right plane
		else if (key == "RIGHT")
			ss >> _right;

		// Parse bottom plane
		else if (key == "BOTTOM")
			ss >> _bottom;

		// Parse top plane
		else if (key == "TOP")
			ss >> _top;

		// Parse resolution
		else if (key == "RES")
		{
			ss >> _width;
			ss >> _height;
		}

		// Parse spheres
		else if (key == "SPHERE")
		{
			Sphere sphere = Sphere(&ss);
			_spheres.push_back(sphere);
		}

		// Parse lights
		else if (key == "LIGHT")
		{
			Light light = Light(&ss);
			_lights.push_back(light);
		}

		// Parse background
		else if (key == "BACK")
		{
			double r;
			double g;
			double b;

			ss >> r;
			ss >> g;
			ss >> b;

			_background = vec3(r, g, b);
		}

		// Parse ambient
		else if (key == "AMBIENT")
		{
			double r;
			double g;
			double b;

			ss >> r;
			ss >> g;
			ss >> b;

			_ambient = vec3(r, g, b);
		}

		// Parse output name
		else if (key == "OUTPUT")
			ss >> _outputName;
	}
}