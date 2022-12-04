//theodore peters 260919785

#pragma once
#ifndef INTERSECTIONDATA_H
#define INTERSECTIONDATA_H

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Material.h"

/**
 * Convenient class to hold information of the closest intersection for a given ray. 
 * By Loïc Nassif
 */
class IntersectionData{
public:
  IntersectionData(){
	material=std::make_shared<Material>();
  }
  virtual~IntersectionData() {}

  void reset(){
	t=FLT_MAX;
  }

  glm::vec3 n; // Intersection normal
  glm::vec3 p; // Intersection point
  float t=FLT_MAX; // Ray parameter giving the position of intersection

  std::shared_ptr<Material>material;
};

#endif
