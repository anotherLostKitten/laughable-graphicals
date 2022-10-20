//theodore peters 260919785

#include "ArcBall.h"
#include "MatrixStack.h"
#include "Program.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"

using namespace std;


ArcBall::ArcBall():R(glm::mat4(1.0)), Rmem(glm::mat4(1.0)), p0(glm::vec3(1.0)), p1(glm::vec3(1.0)), fit(0.5), gain(5)
{
}

ArcBall::~ArcBall()
{
}

glm::vec3 ArcBall::computeVecFromMousePos(double mousex, double mousey, int windowWidth, int windowHeight) {
	//TODO: compute the projection of mouse coords on the arcball
	return glm::vec3(0,0,0);
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
	//TODO: compute the rotation update for the view camera
	R = glm::mat4(1.0);
}
