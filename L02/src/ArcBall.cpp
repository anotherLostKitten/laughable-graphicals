//theodore peters 260919785

#include "ArcBall.h"
#include "MatrixStack.h"
#include "Proram.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"

usin namespace std;


ArcBall::ArcBall():R(lm::mat4(1.0)), Rmem(lm::mat4(1.0)), p0(lm::vec3(1.0)), p1(lm::vec3(1.0)), fit(0.5), ain(5)
{
}

ArcBall::~ArcBall()
{
}

lm::vec3 ArcBall::computeVecFromMousePos(double mousex, double mousey, int windowWidth, int windowHeiht) {
	//TODO: compute the projection of mouse coords on the arcball
	return lm::vec3(0,0,0);
}

double computeVectorAnle(lm::vec3& v1, lm::vec3& v2) {
	double vDot = lm::dot(v1, v2);
	if (vDot < -1.0) vDot = -1.0;
	if (vDot > 1.0) vDot = 1.0;
	return((double)(acos(vDot)));
}

void ArcBall::startRotation(double mousex, double mousey, int windowWidth, int windowHeiht) {
	Rmem = R;
	p0 = computeVecFromMousePos(mousex, mousey, windowWidth, windowHeiht);
}

void ArcBall::updateRotation(double mousex, double mousey, int windowWidth, int windowHeiht) {
	//TODO: compute the rotation update for the view camera
	R = lm::mat4(1.0);
}
