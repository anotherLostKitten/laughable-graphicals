//theodore peters 260919785

#include "AABB.h"
#include "Ray.h"
#include "IntersectionData.h"

#include <glm/gtx/string_cast.hpp>
#include <iostream>

static float AABB_EPSILON=1e-4; // To prevent shadow acne
float t_min=FLT_MIN;
float t_max=FLT_MAX;

AABB::AABB():minpt(0.0f, 0.0f, 0.0f),maxpt(10.0f, 10.0f, 10.0f){
}

AABB::AABB(glm::vec3 size,glm::vec3 position){
  // Compute the min and max points from the size and com
  minpt=position-size/2.0f;
  maxpt=position+size/2.0f;
}

AABB::~AABB(){
}

void AABB::intersect(const std::shared_ptr<Ray>ray,std::shared_ptr<IntersectionData>intersection,bool shad,int thrd){
  glm::vec3 d=ray->direction,e=ray->origin;
  float tx0=t_min,tx1=t_max,ty0=t_min,ty1=t_max,tz0=t_min,tz1=t_max;
  //std::cout<<"testing intersecting ray "<<glm::to_string(e)<<"+"<<glm::to_string(d)<<"*t\n  with AABB "<<glm::to_string(minpt)<<glm::to_string(maxpt)<<"\n";
  if(d.x!=0.f){
	tx0=(minpt.x-e.x)/d.x;
	tx1=(maxpt.x-e.x)/d.x;
	if(tx0>tx1){
	  float tmp=tx0;
	  tx0=tx1;
	  tx1=tmp;
	}
  }else if(e.x<minpt.x||e.x>maxpt.x){
	//std::cout<<"parallel x\n";
	return;
  }
  if(d.y!=0.f){
	ty0=(minpt.y-e.y)/d.y;
	ty1=(maxpt.y-e.y)/d.y;
	if(ty0>ty1){
	  float tmp=ty0;
	  ty0=ty1;
	  ty1=tmp;
	}
  }else if(e.y<minpt.y||e.y>maxpt.y){
	//std::cout<<"parallel y\n";
	return;
  }
  if(d.z!=0.f){
	tz0=(minpt.z-e.z)/d.z;
	tz1=(maxpt.z-e.z)/d.z;
	if(tz0>tz1){
	  float tmp=tz0;
	  tz0=tz1;
	  tz1=tmp;
	}
  }else if(e.z<minpt.z||e.z>maxpt.z){
	//std::cout<<"parallel z\n";
	return;
  }

  //std::cout<<"tx: "<<tx0<<", "<<tx1<<"\n";
  //std::cout<<"ty: "<<ty0<<", "<<ty1<<"\n";
  //std::cout<<"tz: "<<tz0<<", "<<tz1<<"\n";
  
  float tmax=glm::min(glm::min(tx1,ty1),tz1),tmin;

  glm::vec3 nrml;

  if(tx0>ty0)
	if(tx0>tz0){
	  nrml=glm::vec3(1.f,0.f,0.f);
	  tmin=tx0;
	}else{
	  nrml=glm::vec3(0.f,0.f,1.f);
	  tmin=tz0;
	}
  else
	if(ty0>tz0){
	  nrml=glm::vec3(0.f,1.f,0.f);
	  tmin=ty0;
	}else{
	  nrml=glm::vec3(0.f,0.f,1.f);
	  tmin=tz0;
	}
  
  
  if(tmax<tmin){
	//std::cout<<"does not intersect AABB\n";
	return;
  }
  //std::cout<<"hit\n";
  if(tmin>=AABB_EPSILON&&tmin<intersection->t){
	intersection->t=tmin;
	if(shad)return;
	ray->computePoint(tmin,intersection->p);
	intersection->n=glm::dot(nrml,d)<0.f?nrml:-1.f*nrml;//idk to intersect both sides?
	intersection->material=materials[0];
  }

}
