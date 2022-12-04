//theodore peters 260919785

#include "HierachicalShape.h"
#include "Ray.h"
#include "IntersectionData.h"

float HIERACHY_EPSILON=1e-4; // To prevent shadow acne

#include <iostream>
#include <glm/gtx/string_cast.hpp>

HierachicalShape::HierachicalShape():M(glm::mat4(1.0f)), Minv(glm::mat4(1.0f)){
  transformRay=std::make_shared<Ray>();
  transformData=std::make_shared<IntersectionData>();
}

HierachicalShape::~HierachicalShape(){
}

void HierachicalShape::intersect(const std::shared_ptr<Ray>ray,std::shared_ptr<IntersectionData>intersection){
  // TODO 7: do something useful here!
  glm::vec4 tmp=Minv*glm::vec4(ray->origin,1.f);
  transformRay->origin=glm::vec3(tmp.x,tmp.y,tmp.z)/tmp.w;
  
  tmp=Minv*glm::vec4(ray->direction,0.f);
  transformRay->direction=glm::vec3(tmp.x,tmp.y,tmp.z);
  
  transformData->t=intersection->t;
  
  for(auto s:children){
	s->intersect(transformRay,transformData);
  }
  if(transformData->t>=HIERACHY_EPSILON&&transformData->t<intersection->t){
	//std::cout<<"overwritten t in "<<name<<", "<<type<<"\n";
	intersection->t=transformData->t;
	ray->computePoint(transformData->t,intersection->p);
	tmp=glm::transpose(Minv)*glm::vec4(transformData->n,0.f);
	intersection->n=glm::normalize(glm::vec3(tmp.x,tmp.y,tmp.z));
	if(materials.size()==0)
	  intersection->material=transformData->material;
	else
	  intersection->material=materials[0];
  }
}

void HierachicalShape::print(std::string str){
  std::cout<<str<<name<<": "<<type<<" "<<glm::to_string(M)<<"\n";
  for(auto s:children)
	s->print(str+"-");
}
