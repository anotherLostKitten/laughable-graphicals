//theodore peters 260919785

#include "Sphere.h"
#include "Ray.h"
#include "IntersectionData.h"

float SPHERE_EPSILON=1e-4; // To prevent shadow acne

Sphere::Sphere():radius(1.0f),center(glm::vec3(0.0f, 0.0f, 0.0f)){
}

Sphere::Sphere(float _radius,glm::vec3 _center):radius(_radius),center(_center){
}

Sphere::Sphere(float _radius):radius(_radius),center(glm::vec3(0.0f, 0.0f, 0.0f)){
}

Sphere::~Sphere(){
}

void Sphere::intersect(const std::shared_ptr<Ray>ray,std::shared_ptr<IntersectionData>intersection,bool shad){
  glm::vec3 d=ray->direction,e=ray->origin;
  float a=glm::dot(d,d),b=glm::dot(d,e-center),c=glm::dot(e-center,e-center)-radius*radius;
  float dsc=b*b-a*c;
  if(dsc<0)return;
  float t0=(-b-sqrt(dsc))/a,t1=(-b+sqrt(dsc))/a;
  if(t0>t1){
	float tmp=t0;t0=t1;t1=tmp;
  }
  if(t0>=SPHERE_EPSILON){
	if(t0<intersection->t){
	  intersection->t=t0;
	  if(shad)return;
	  ray->computePoint(t0,intersection->p);
	  intersection->n=(intersection->p-center)/radius;
	  intersection->material=materials[0];
	}
  }else{
	if(t1>=SPHERE_EPSILON&&t1<intersection->t){
	  intersection->t=t1;
	  if(shad)return;
	  ray->computePoint(t1,intersection->p);
	  intersection->n=(intersection->p-center)/radius;
	  intersection->material=materials[0];
	}
  }
}
