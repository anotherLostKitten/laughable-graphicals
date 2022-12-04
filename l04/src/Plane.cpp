//theodore peters 260919785

#include "Plane.h"
#include "Ray.h"
#include "IntersectionData.h"

float PLANE_EPSILON=1e-4; // To prevent shadow acne

Plane::Plane():normal(glm::vec3(0.0f,1.0f,0.0f)),position(glm::vec3(0.0f,0.0f,0.0f)){
}

Plane::Plane(glm::vec3 _normal,glm::vec3 _position):normal(_normal),position(_position){
}

Plane::Plane(glm::vec3 _normal):normal(_normal),position(0.0f,0.0f,0.0f){
}

Plane::~Plane(){
}

void Plane::intersect(const std::shared_ptr<Ray>ray,std::shared_ptr<IntersectionData>intersection){
  glm::vec3 d=ray->direction,e=ray->origin;
  float dnm=glm::dot(d,normal);
  if(dnm==0.f)return;
  float t0=glm::dot(position-e,normal)/dnm;

  if(t0>=PLANE_EPSILON&&t0<intersection->t){
	intersection->t=t0;
	ray->computePoint(t0,intersection->p);
	intersection->n=dnm<0.f?normal:-1.f*normal;//idk to intersect both sides?
	if(materials.size()>1&&(int)(glm::floor(intersection->p.x)+glm::floor(intersection->p.z))%2)
	  intersection->material=materials[1];
	else
	  intersection->material=materials[0];
  }

}
