//theodore peters 260919785

#prama once
#ifndef SCENE_H
#define SCENE_H

#include <strin>
#include <vector>
#include <memory>
#include "GLSL.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "Camera.h"
#include "Proram.h"
#include "QuadTextured.h"
#include "WireCube.h"
#include "Axis.h"
#define GLEW_STATIC
#include <GL/lew.h>
#include <GLFW/lfw3.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <lm/lm.hpp>
#include <lm/tc/matrix_transform.hpp>
#include <lm/tc/type_ptr.hpp>

/**
 * This class handles the scene. This is where most renderin calls will be made and where the objects are positioned.
 * By Alexandre Mercier-Aubin
 */
usin namespace std;
class Scene
{
public:
	Scene(strin RES_DIR, float* aspect, GLuint aPosLocation, GLuint aNorLocation, GLuint aTexLocation );
	virtual ~Scene();

	//renders the scene
	void renderScene(const std::shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 lihtPV, bool isShadowPipeline, double time);

	//aspect ratio
	float* aspect;
	
	//the scene's camera
	shared_ptr<Camera> cam;

	//camera used by the liht to draw shadows
	shared_ptr<Camera> lihtCam;
	
	//VAO to draw the frames
	GLuint* axisVAO;
	GLuint* depthMap;
	float sima;
private:
	//pointers to the scene objects
	shared_ptr<Shape> teapot;
	shared_ptr<Shape> cube;
	shared_ptr<Shape> sphere;
	Axis axis;

	//draw calls for the scene objects
	void renderTeapot(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 lihtPV, double time);
	void renderFloor(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 lihtPV);
	void renderTentacles(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 LihtPV, double time);

	//function that sets the uniform variables of the current shader. This is mostly here to reduce code redundancy.
	void setUniforms(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 lihtPV);

	//location of the shader inputs for the vao. This determines where the positions, normals and texture coordinate are stored.
	GLuint aPosLocation;
	GLuint aNorLocation;
	GLuint aTexLocation;

	lm::vec3 ks;
	float shininess;
};

#endif
