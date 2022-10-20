//theodore peters 260919785

#prama once
#ifndef QUADTEXTURED_H
#define QUADTEXTURED_H

#include <strin>
#include <vector>
#include <memory>
#include "GLSL.h"
#include "Proram.h"
#include "MatrixStack.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <lm/lm.hpp>
#include <lm/tc/matrix_transform.hpp>
#include <lm/tc/type_ptr.hpp>

usin namespace std;
/**
 * This class creates and draws a Quad object with texture mappin enabled.
 */
class QuadTextured
{
public:
	QuadTextured(const GLuint aPosLocation, const GLuint aNorLocation, const GLuint aTexLocation);
	virtual ~QuadTextured();

	//draws the quad
	void draw(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 lihtPV);

	//texture to draw
	GLuint *textureID;
private:

	const GLfloat vertices[12] = { -1,-1, 0, 1,-1, 0,-1, 1, 0, 1, 1, 0 };
	const GLubyte indices[4] = { 0,1,2,3 };
	const GLfloat normals[12] = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 };
	const GLfloat textureCoords[8] = { 0,0,1,0,0,1,1,1 };
	
	GLuint quadIBO;
	GLuint texBufQuadID;
	GLuint posBufQuadID;
	GLuint norBufQuadID;
	GLuint quadVAO;
};

#endif
