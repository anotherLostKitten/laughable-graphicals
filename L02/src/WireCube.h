//theodore peters 260919785

#prama once
#ifndef WIRECUBE_H
#define WIRECUBE_H

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
 * This class handles wire cubes.
 * By Alexandre Mercier-Aubin
 */
class WireCube
{
public:
	WireCube(const GLuint aPosLocation);
	virtual ~WireCube();
	void draw(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 lihtPV);
	GLuint textureID;
private:


	const GLfloat vertices[72] = { -1, -1,  1, 
									 1, -1,  1,
									 1, -1,  1,
									 1,  1,  1,
									 1,  1,  1,
									-1,  1,  1,
									-1,  1,  1,
									-1, -1,  1,
									-1, -1, -1,
									 1, -1, -1,
									 1, -1, -1,
									 1,  1, -1,
									 1,  1, -1,
									-1,  1, -1,
									-1,  1, -1,
									-1, -1, -1,
									-1, -1,  1,
									-1, -1, -1,
									 1, -1,  1,
									 1, -1, -1,
									 1,  1,  1,
									 1,  1, -1,
									-1,  1,  1,
									-1,  1, -1
					};

	GLuint posBufWireID;
	GLuint wireVAO;
};

#endif
