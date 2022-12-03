//theodore peters 260919785

#include "Sphere.h"
#include "Ray.h"
#include "IntersectionData.h"

float SPHERE_EPSILON = 1e-4; // To prevent shadow acne

Sphere::Sphere() :
    radius(1.0f),
    center(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Sphere::Sphere(float _radius, glm::vec3 _center) :
    radius(_radius),
    center(_center)
{
}

Sphere::Sphere(float _radius) :
    radius(_radius),
    center(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Sphere::~Sphere()
{
}

void Sphere::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    // TODO: Objective 2: intersection of ray with sphere
}