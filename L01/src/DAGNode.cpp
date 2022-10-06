#include "DAGNode.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"

using namespace std;

DAGNode::DAGNode()
{
}

DAGNode::~DAGNode()
{
}

void DAGNode::init()
{
}

void DAGNode::draw(const std::shared_ptr<Program> prog, const std::shared_ptr<Program> prog2, const std::shared_ptr<MatrixStack> MV, GLuint vao, int fileid, const std::shared_ptr<Shape> teapot, const std::shared_ptr<Shape> sphere, const std::shared_ptr<Shape> cube, float* frameData, bool isHead) const
{
  float spacefactor = fileid ? 1.f : 3.f; // idk i will space out the bollywood one a bit
  
  MV->pushMatrix();
  
  MV->translate(offset * spacefactor);

  float* fd = frameData + channelDataStartIndex;
  int i = 0;
  
  if(channels.size()) {
	if(channels.size() == 6) {
	  MV->translate(fd[0] * spacefactor, fd[1] * spacefactor, fd[2] * spacefactor);
	  i=3;
	}

	for(int j = 0; j < 3; j++) {
	  if(channels[i+j][0]=='X')
		MV->rotate(fd[i+j] * 3.1415926535 / 180., 1., 0., 0.);
	  else if(channels[i+j][0]=='Y')
		MV->rotate(fd[i+j] * 3.1415926535 / 180., 0., 1., 0.);
	  else
		MV->rotate(fd[i+j] * 3.1415926535 / 180., 0., 0., 1.);
	}
  }
  
  for(DAGNode* child : children) {
	child->draw(prog, prog2, MV, vao, fileid, teapot, sphere, cube, frameData, name.compare("Head"));
  }
  prog2->bind();
  MV->scale(4.);
  glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
  glBindVertexArray(vao);
  glDrawArrays(GL_LINES, 0, 6);
  prog2->unbind();

  prog->bind();

  if(isHead) {
	MV->scale(0.75);
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
	sphere->draw(prog);
  } else {
	MV->rotate(-3.1415926535 / 2., 0., 1., 0.);
	MV->scale(6.);
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
	teapot->draw(prog);
	MV->scale(0.125);
	MV->translate(4.2, 4.5, 2.2);
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
	sphere->draw(prog);
	MV->translate(0., 0., -4.4);
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
	sphere->draw(prog);
	MV->translate(0.8, -2.8, 2.2);
	MV->scale(0.2, 0.7, 4.);
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
	sphere->draw(prog);
  }

  if(channelDataStartIndex) {
	MV->popMatrix();
	MV->pushMatrix();
	MV->translate(offset*0.5f*spacefactor);
	MV->scale(offset*.5f*spacefactor + 1.f);
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
	cube->draw(prog);
  }
  prog->unbind();
  
  MV->popMatrix();
}
