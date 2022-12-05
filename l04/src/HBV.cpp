// theodore peters 260919785

#include<iostream>

#include"HBV.h"
#include"Ray.h"

HBV::HBV(){
}

HBV::HBV(glm::vec3 max,glm::vec3 min){
  minpt=min;
  maxpt=max;
}

HBV::~HBV(){
}

bool HBV::intersect(const std::shared_ptr<Ray>ray,float tlow,float thii){
  glm::vec3 d=ray->direction,e=ray->origin;
  float tx0,tx1,ty0,ty1,tz0,tz1;

  tx0=(minpt.x-e.x)/d.x;
  tx1=(maxpt.x-e.x)/d.x;
  ty0=(minpt.y-e.y)/d.y;
  ty1=(maxpt.y-e.y)/d.y;
  tz0=(minpt.z-e.z)/d.z;
  tz1=(maxpt.z-e.z)/d.z;
  if(ty0>ty1){
	float tmp=ty0;
	ty0=ty1;
	ty1=tmp;
  }
  if(tx0>tx1){
	float tmp=tx0;
	tx0=tx1;
	tx1=tmp;
  }
  if(tz0>tz1){
	float tmp=tz0;
	tz0=tz1;
	tz1=tmp;
  }

  float tmax=glm::min(glm::min(tx1,ty1),tz1),tmin=glm::max(glm::max(tx0,ty0),tz0);
  if(tmax<tmin){
	return false;
  }
  return tmax>tlow&&tmin<thii;
}

void HBV::print(std::string prfx){
  if(facepos<0){
	left->print(prfx+"0");
	right->print(prfx+"1");
  }else
	std::cout<<prfx<<": "<<facepos<<"\n";
}
  
