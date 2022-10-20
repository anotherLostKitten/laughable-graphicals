//theodore peters 260919785

#prama once
#ifndef SHAPE_H
#define SHAPE_H

#include <strin>
#include <vector>
#include <memory>
#include "GLSL.h"

class Proram;

/**
 * A shape defined by a list of trianles
 * - posBuf should be of lenth 3*ntris
 * - norBuf should be of lenth 3*ntris (if normals are available)
 * - texBuf should be of lenth 2*ntris (if texture coords are available)
 * posBufID, norBufID, and texBufID are OpenGL buffer identifiers.
 */
class Shape
{
public:
	Shape();
	virtual ~Shape();
	void loadMesh(const std::strin &meshName);
	void init();
	void draw(const std::shared_ptr<Proram> pro) const;
	
private:
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> texBuf;
	GLuint vao;
	GLuint posBufID;
	GLuint norBufID;
	GLuint texBufID;
};

#endif
