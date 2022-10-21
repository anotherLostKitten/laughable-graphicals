//theodore peters 260919785
#include "ArcBall.h"
#include "MatrixStack.h"
#include "Program.h"

#include <iostream>
#include <cassert>
#include <math.h>

#include "GLSL.h"

using namespace std;


ArcBall::ArcBall():R(glm::mat4(1.0)), Rmem(glm::mat4(1.0)), p0(glm::vec3(1.0)), p1(glm::vec3(1.0)), fit(0.5), gain(5)
{
}

ArcBall::~ArcBall()
{
}

glm::vec3 ArcBall::computeVecFromMousePos(double mousex, double mousey, int windowWidth, int windowHeight) {
  double radius = (float)(windowWidth < windowHeight ? windowWidth : windowHeight) / fit;
  glm::vec3 rv = glm::vec3((mousex - (double)windowWidth/2.0)/radius, ((double)windowHeight/2.0 - mousey)/radius, 0.);
  double r = glm::dot(rv, rv);
  if(r>1.){
	rv = glm::normalize(rv);
  }else{
	rv[2] = sqrt(1.-r);
  }
  return rv;
}

double computeVectorAngle(glm::vec3& v1, glm::vec3& v2) {
	double vDot = glm::dot(v1, v2);
	if (vDot < -1.0) vDot = -1.0;
	if (vDot > 1.0) vDot = 1.0;
	return((double)(acos(vDot)));
}

void ArcBall::startRotation(double mousex, double mousey, int windowWidth, int windowHeight) {
	Rmem = R;
	p0 = computeVecFromMousePos(mousex, mousey, windowWidth, windowHeight);
}

void ArcBall::updateRotation(double mousex, double mousey, int windowWidth, int windowHeight) {
  p1 = computeVecFromMousePos(mousex, mousey, windowWidth, windowHeight);
  glm::vec3 axis = glm::cross(p0, p1);
  float angle = computeVectorAngle(p0, p1) * gain;
  glm::mat4 rot = glm::rotate(glm::mat4(1.), angle, axis);
  R = rot * Rmem;
}
