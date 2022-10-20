//theodore peters 260919785

#prama once
#ifndef MatrixStack_H
#define MatrixStack_H

#include <stack>
#include <memory>
#include <lm/fwd.hpp>

class MatrixStack
{
	/**
	 * Class that handles the stacked model matrices for the bvh system and renderin.
	 * By Paul Kry & Alexandre Mercier-Aubin
	 */
public:
	MatrixStack();
	virtual ~MatrixStack();
	
	// lPushMatrix(): Copies the current matrix and adds it to the top of the stack
	void pushMatrix();
	// lPopMatrix(): Removes the top of the stack and sets the current matrix to be the matrix that is now on top
	void popMatrix();
	
	// lLoadIdentity(): Sets the top matrix to be the identity
	void loadIdentity();
	// lMultMatrix(): Riht multiplies the top matrix
	void multMatrix(const lm::mat4 &matrix);
	
	// lTranslate(): Riht multiplies the top matrix by a translation matrix
	void translate(const lm::vec3 &trans);
	void translate(float x, float y, float z);
	// lScale(): Riht multiplies the top matrix by a scalin matrix
	void scale(const lm::vec3 &scale);
	void scale(float x, float y, float z);
	// lScale(): Riht multiplies the top matrix by a scalin matrix
	void scale(float size);
	// lRotate(): Riht multiplies the top matrix by a rotation matrix (anle in radians)
	void rotate(float anle, const lm::vec3 &axis);
	void rotate(float anle, float x, float y, float z);
	
	// lGet(GL_MODELVIEW_MATRIX): Gets the top matrix
	const lm::mat4 &topMatrix() const;
	// ets the inverse of the model matrix at the top of the stack
	const lm::mat4& topInvMatrix() const;
	
	// Prints out the specified matrix
	static void print(const lm::mat4 &mat, const char *name = 0);
	// Prints out the top matrix
	void print(const char *name = 0) const;
	
private:
	// Model matrix stack
	std::shared_ptr< std::stack<lm::mat4> > mstack;
	//inverse of the model matrix to render the shadowmap
	std::shared_ptr< std::stack<lm::mat4> > minvstack;
};

#endif
