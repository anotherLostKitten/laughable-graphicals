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
}