//theodore peters 260919785

#include "Face.h"
#include <iostream>

using namespace std;

Face::Face(HalfEdge *he)
{
  this->he = he;
  HalfEdge *loop = he;
  do
    {
	  loop->leftFace = this;
	  loop = loop->next;
    } while (loop != he);
  computeNormal();
  computeCenter();
}

void Face::computeCenter()
{
  c = glm::vec3(0, 0, 0);
  HalfEdge *loop = he;
  do
    {
	  c += loop->head->p;
	  loop = loop->next;
    } while (loop != he);
  c *= (1.0 / 3.0);
}

void Face::computeNormal(){
  glm::vec3 v1=he->e;
  glm::vec3 v2=he->next->e;
  n=glm::cross(v1,v2);
  area=0.5*glm::length(n);
  if(area>0.)
	n=glm::normalize(n);
}

void Face::printFace(){
  cout<<"v01 ("<<he->head->p.x<<", "<<he->head->p.y<<", "<<he->head->p.z<<")\n";
  cout<<"v02 ("<<he->next->head->p.x<<", "<<he->next->head->p.y<<", "<<he->next->head->p.z<<")\n";
  cout<<"v03 ("<<he->next->next->head->p.x<<", "<<he->next->next->head->p.y<<", "<<he->next->next->head->p.z<<")\n";
  cout<<"area: "<<area<<"; normal ("<<n.x<<", "<<n.y<<", "<<n.z<<")\n";
}
