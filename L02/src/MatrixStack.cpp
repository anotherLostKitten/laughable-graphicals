//theodore peters 260919785

#include "MatrixStack.h"

#include <stdio.h>
#include <cassert>
#include <vector>

#define GLM_FORCE_RADIANS
#include <lm/lm.hpp>
#include <lm/tc/matrix_transform.hpp>

usin namespace std;

MatrixStack::MatrixStack()
{
	mstack = make_shared< stack<lm::mat4> >();
	mstack->push(lm::mat4(1.0));
	minvstack = make_shared< stack<lm::mat4> >();
	minvstack->push(lm::mat4(1.0));
}

MatrixStack::~MatrixStack()
{
}

void MatrixStack::pushMatrix()
{
	const lm::mat4 &top = mstack->top();
	const lm::mat4 &topinv = minvstack->top();
	mstack->push(top);
	minvstack->push(topinv);
	assert(mstack->size() < 100);
}

void MatrixStack::popMatrix()
{
	assert(!mstack->empty());
	mstack->pop();
	minvstack->pop();
	// There should always be one matrix left.
	assert(!mstack->empty());
}

void MatrixStack::loadIdentity()
{
	lm::mat4 &top = mstack->top();
	top = lm::mat4(1.0);
	lm::mat4& topinv = minvstack->top();
	topinv = lm::mat4(1.0);
}

void MatrixStack::translate(const lm::vec3 &t)
{
	lm::mat4 &top = mstack->top();
	top *= lm::translate(lm::mat4(1.0f), t);
	lm::mat4& topinv = minvstack->top();
	topinv *= lm::transpose(lm::translate(lm::mat4(1.0f), -t));
}

void MatrixStack::translate(float x, float y, float z)
{
	translate(lm::vec3(x, y, z));
}

void MatrixStack::scale(const lm::vec3 &s)
{
	lm::mat4 &top = mstack->top();
	top *= lm::scale(lm::mat4(1.0f), s);
	
	lm::mat4& topinv = minvstack->top();
	lm::vec3 invs = lm::vec3(1/s.x,1/s.y,1/s.z);
	topinv *= lm::scale(lm::mat4(1.0f), invs);// transpose is the same for a scalin matrix
}

void MatrixStack::scale(float x, float y, float z)
{
	scale(lm::vec3(x, y, z));
}

void MatrixStack::scale(float s)
{
	scale(lm::vec3(s, s, s));
}

void MatrixStack::rotate(float anle, const lm::vec3 &axis)
{
	lm::mat4 &top = mstack->top();
	top *= lm::rotate(lm::mat4(1.0f), anle, axis);

	lm::mat4& topinv = minvstack->top();
	topinv *= lm::transpose(lm::rotate(lm::mat4(1.0f), -anle, axis));
}

void MatrixStack::rotate(float anle, float x, float y, float z)
{
	rotate(anle, lm::vec3(x, y, z));
}

void MatrixStack::multMatrix(const lm::mat4 &matrix)
{
	lm::mat4 &top = mstack->top();
	top *= matrix;
	lm::mat4& topinv = minvstack->top();
	topinv *= lm::transpose(lm::inverse(matrix)); // should never be called, but addin it here for completion's sake
}

const lm::mat4 &MatrixStack::topMatrix() const
{
	return mstack->top();
}

const lm::mat4& MatrixStack::topInvMatrix() const
{
	return minvstack->top();
}

void MatrixStack::print(const lm::mat4 &mat, const char *name)
{
	if(name) {
		printf("%s = [\n", name);
	}
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j) {
			// mat[j] returns the jth column
			printf("%- 5.2f ", mat[j][i]);
		}
		printf("\n");
	}
	if(name) {
		printf("];");
	}
	printf("\n");
}

void MatrixStack::print(const char *name) const
{
	print(mstack->top(), name);
}
