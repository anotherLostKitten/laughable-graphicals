#pragma once
#ifndef DAGNODE_H
#define DAGNODE_H

#include <string>
#include <vector>
#include <memory>
#include "GLSL.h"

#define GLEW_STATIC
#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Program;
class MatrixStack;
class Shape;

/**
 * This class manages nodes for a character skeleton specified by a bvh file.
 * TODO: add to this class as needed!
 */
class DAGNode
{
public:
	DAGNode();
	virtual ~DAGNode();	
	void init();
	// Draws the node and its children.  TODO: modify this method signature if needed!
  void draw(const std::shared_ptr<Program> prog, const std::shared_ptr<Program> prog2, const std::shared_ptr<MatrixStack> MV, GLuint vao, int fileid, const std::shared_ptr<Shape> teapot, const std::shared_ptr<Shape> sphere, const std::shared_ptr<Shape> cube, float* frameData,bool isHead) const;

	// name of this node (useful for debugging)
	std::string name;
	// translational offset of this node from its parent
	glm::fvec3 offset;
	// channel names, each will be one of Xposition Yposition Zposition Xrotation Yrotation Zrotation
	std::vector<std::string> channels;
	// parent of this node, or NULL if this node is the root
	DAGNode* parent;
	// children of this node
	std::vector <DAGNode*> children;
	// start index from which this node's channel data can be read from a frame of motion capture data
	int channelDataStartIndex;

private:
};

#endif
