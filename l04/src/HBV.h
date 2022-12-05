//theodore peters 260919785

#pragma once
#ifndef HBV_H
#define HBV_H

#include<string>
#include<memory>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Ray;

/**
 * HBV
 */
class HBV{
 public:
  HBV(glm::vec3 max,glm::vec3 min);

  HBV();
  
  virtual~HBV();

  bool intersect(const std::shared_ptr<Ray>ray,float tlow,float thii);
  
  void print(std::string prfx);

  glm::vec3 minpt;
  glm::vec3 maxpt;

  std::shared_ptr<HBV>left,right;
  
  int facepos;
};

#endif
