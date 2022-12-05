//theodore peters 260919785

#pragma once
#ifndef MESH_H
#define MESH_H

#include <string>
#include <memory>

#include "Shape.h"
#include "Material.h"
#include "HBV.h"

/**
 * A shape defined by a list of triangles
 * - posBuf should be of length 3*ntris
 * - norBuf should be of length 3*ntris (if normals are available)
 * - texBuf should be of length 2*ntris (if texture coords are available)
 */
class Mesh:public Shape{
public:
  Mesh(const std::string&meshName);
  Mesh(const std::string&meshName,glm::vec3 position,float scale);

  virtual~Mesh();
  void loadOBJ(const std::string&meshName);
	
  void intersect(const std::shared_ptr<Ray>ray,std::shared_ptr<IntersectionData>intersection,bool shad,int thrd);

  void translate(glm::vec3 position);

  void scale(float s); // Uniform scale

  std::string filepath;

private:
  std::vector<float> posBuf;
  std::vector<float> norBuf;
  std::vector<float> texBuf;
  
  std::shared_ptr<HBV>hbv;
  
  int faceCount=0;

  void swap(int i,int j);
  int partition(int s,int e,int axis,float piv);

  void genHBV(int s,int e,int axis,std::shared_ptr<HBV>root);

  void intrec(const std::shared_ptr<Ray>ray,std::shared_ptr<IntersectionData>inter,bool shad,int t,std::shared_ptr<HBV>root);
};

#endif
