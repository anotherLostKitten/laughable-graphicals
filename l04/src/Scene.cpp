//theodore peters 260919785

#include "Scene.h"
#include "Ray.h"
#include "IntersectionData.h"
#include "Camera.h"
#include "Light.h"
#include "Shape.h"

#include <stdlib.h> 

float SHADOW_EPSILON = 1e-6; // To prevent shadow acne

// Default values, they get overwritten if scene file contains them
Scene::Scene(const std::string name) :
	width(1080), height(720), aspect(1080.0f / 720.0f),
	shininess(64),
	ambient(0.1f, 0.1f, 0.1f)
{
	outputFilename = name;
}

void Scene::init() 
{
	// Initialize camera	
	cam = make_shared<Camera>();

	// Init image
	image = make_shared<Image>(width, height);
}

Scene::~Scene()
{
}

void Scene::addObject(shared_ptr<Shape> shape) 
{
	shapes.push_back(shape);
}

void Scene::addLight(shared_ptr<Light> light) 
{
	lights.push_back(light);
}

void Scene::render() 
{
	std::shared_ptr<IntersectionData> intersection = make_shared<IntersectionData>();
	std::shared_ptr<IntersectionData> shadowIntersection = make_shared<IntersectionData>();
	glm::vec3 camDir = -cam->lookAt + cam->position;
	float d = 1;
	float top = d * glm::tan(0.5 * M_PI * cam->fovy / 180.f);
	float right = aspect * top;
	float bottom = -top;
	float left = -right;

	// Compute basis
	glm::vec3 w = glm::normalize(camDir);
	glm::vec3 u = glm::normalize(glm::cross(cam->up, w));
	glm::vec3 v = glm::cross(w, u);

	std::shared_ptr<Ray> ray = make_shared<Ray>();
	std::shared_ptr<Ray> shadowRay = make_shared<Ray>();

	glm::vec3 eyeVec = glm::vec3();
	glm::vec3 lightVec = glm::vec3();
	glm::vec3 h = glm::vec3(); // Bisection vector

	ray->origin = cam->position;

	glm::vec3 pixel = glm::vec3();
	glm::vec3 colour = glm::vec3();

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			colour = glm::vec3(0.0f, 0.0f, 0.0f);

			// TODO: 1. generate a ray

			// TODO: 2. test for intersection with scene surfaces

			// TODO: 3. compute the shaded result for the intersection point

			// Clamp colour values to 1
			colour.r = glm::min(1.0f, colour.r);
			colour.g = glm::min(1.0f, colour.g);
			colour.b = glm::min(1.0f, colour.b);

			// Write pixel colour to image
			colour *= 255;
			image->setPixel(i, j, colour.r, colour.g, colour.b);
		}
	}

	image->writeToFile( outputFilename );
}
