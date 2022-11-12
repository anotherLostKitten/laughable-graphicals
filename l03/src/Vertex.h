//theodore peters 260919785

#pragma once
#ifndef Vertex_H
#define Vertex_H
#include <memory>
#include <iostream>
#include "GLHeaders.h"

using namespace std;

class HalfEdge;

/**
 * Simple vertex class
 */
class Vertex{
public:
  /** We must know the vertex index when assembling openGL buffers */
  int index;

  glm::vec3 p;

  /** Must be computed or loaded */
  glm::vec3 n;

  /** an example half edge */
  HalfEdge *he;

  /** diagonal Laplacian weights */
  double Lii;

  /** off diagonal Laplacian weights */
  double *Lij;

  /** Heat value */
  double u0=0.;

  /** Heat value */
  double ut=0.;
  float ut_float=0.f;

  /** 1/3 area of surrounding faces */
  double area=0.;

  /** divergence of the normalized gradient of u */
  double divX=0.;

  /** distance */
  double phi=0.;

  /** For setting boundary conditions */
  bool constrained=false;

  /**
   * @return valence of the vertex 
   */
  int valence();

};
#endif
