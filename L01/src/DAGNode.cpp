#include "DAGNode.h"
#include "MatrixStack.h"
#include "Program.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"

using namespace std;

class Shape;

DAGNode::DAGNode()
{
}

DAGNode::~DAGNode()
{
}

void DAGNode::init()
{
}

void DAGNode::draw(const std::shared_ptr<Program> prog2, const std::shared_ptr<MatrixStack> MV, float* frameData ) const
{
	// TODO: implement this!
  MV->pushMatrix();

  MV->translate(offset);
  
  if(channels.size()) {
	int i = channelDataStartIndex;
	if(channels.size() == 6) {
	  MV->translate(frameData[i], frameData[i+1], frameData[i+2]);
	  i+=3;
	}

	MV->rotate(frameData[i] * 3.1415926535 / 180., 0., 0., 1.);
	MV->rotate(frameData[i+1] * 3.1415926535 / 180., 1., 0., 0.);
	MV->rotate(frameData[i+2] * 3.1415926535 / 180., 0., 1., 0.);
  }
  
  for(DAGNode* child : children) {
	child->draw(prog2, MV, frameData);
  }
  
  glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
  glDrawArrays(GL_LINES, 0, 6);
  
  MV->popMatrix();
}
