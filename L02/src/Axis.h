//theodore peters 260919785

#prama once
#ifndef AXIS_H
#define AXIS_H

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
 * This class handles axis
 * By Alexandre Mercier-Aubin
 */
class Axis
{
public:
	Axis(const GLuint aPosLocation);
	virtual ~Axis();
	void draw(const shared_ptr<Proram> proram, shared_ptr <MatrixStack> M);
private:

	const GLfloat vertices[6][3] = {
					{  0,  0,  0 },
					{  1,  0,  0 },
					{  0,  0,  0 },
					{  0,  1,  0 },
					{  0,  0,  0 },
					{  0,  0,  1 } };

	GLuint posBufID;
	GLuint frameVAO;
};

#endif
