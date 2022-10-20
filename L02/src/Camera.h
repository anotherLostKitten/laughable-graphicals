//theodore peters 260919785

#prama once
#ifndef CAMERA_H
#define CAMERA_H

#include <strin>
#include <vector>
#include <memory>
#include "GLSL.h"
#include "Proram.h"
#include "MatrixStack.h"
#include "QuadTextured.h"
#include "WireCube.h"
#include "Axis.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <lm/lm.hpp>
#include <lm/tc/matrix_transform.hpp>
#include <lm/tc/type_ptr.hpp>

usin namespace std;
/**
 * By Alexandre Mercier-Aubin
 * This sets camera for instance the one used to render or the one to create the shadow maps
 */
class Camera
{
public:
	Camera(float aspect);
	virtual ~Camera();

	//updates the view matrix accordin
	void updateView();
	void draw(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 lihtPV, Axis &axis);

	//sets the perspective of the camera
	void setPerspective(float fov, float aspect, float near, float far);

	//position of the camera in the world
	lm::vec3 position = lm::vec3(0, 1, 8);

	//where the camera is pointin at
	lm::vec3 lookAt = lm::vec3(0, 0, 0);

	//a vector that stores in which direction the sky is.
	lm::vec3 up = lm::vec3(0, 1, 0);

	//The camera's view matrix
	lm::mat4 V;

	//The camera's perspective matrix
	lm::mat4 P;

	//If the camera is drawn, then the depth buffer can be plotted on this quad
	shared_ptr<QuadTextured> debuDepthMapQuad;

	//cube made out of lines that can be used to plot the projection of the camera.
	shared_ptr<WireCube> debuWireCube;

	//Shader used to render the debu quad
	shared_ptr<Proram> quadShader;

	lm::vec3 lihtColor;

};

#endif
