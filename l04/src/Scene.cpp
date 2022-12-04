//theodore peters 260919785

#include "Scene.h"
#include "Ray.h"
#include "IntersectionData.h"
#include "Camera.h"
#include "Light.h"
#include "Shape.h"

#include <stdlib.h>
#include<iostream>
#include<random>

float SHADOW_EPSILON=1e-6; // To prevent shadow acne

// Default values, they get overwritten if scene file contains them
Scene::Scene(const std::string name):width(1080),height(720),aspect(1080.0f / 720.0f),ambient(0.1f, 0.1f, 0.1f){
  outputFilename=name;
}

void Scene::init(){
  // Initialize camera	
  cam=make_shared<Camera>();

  // Init image
  image=make_shared<Image>(width,height);
}

Scene::~Scene(){
}

void Scene::addObject(shared_ptr<Shape>shape){
  shapes.push_back(shape);
}

void Scene::addLight(shared_ptr<Light>light){
  lights.push_back(light);
}

void Scene::renderSetup(){
  camDir=-cam->lookAt+cam->position;
  d=1;
  top=d*glm::tan(0.5*M_PI*cam->fovy/180.f);
  right=aspect*top;
  bottom=-top;
  left=-right;

  // Compute basis
  w=glm::normalize(camDir);
  u=glm::normalize(glm::cross(cam->up,w));
  v=glm::cross(w,u);

  //for(auto s:shapes)s->print("|");
}

void Scene::renderThr(int thrdn){
  std::random_device rd;
  std::ranlux24 gen{rd()};
  std::uniform_real_distribution<> dist(0, 1);
  
  std::shared_ptr<IntersectionData>intersection=make_shared<IntersectionData>();
  std::shared_ptr<IntersectionData>shadowIntersection=make_shared<IntersectionData>();
  std::shared_ptr<Ray>ray=make_shared<Ray>();
  std::shared_ptr<Ray>shadowRay=make_shared<Ray>();
  glm::vec3 pixel;
  glm::vec3 colour;

  for(int i=thrdn;i<width;i+=RENDER_THREADS){
	//std::cout<<"thrdn: "<<thrdn<<", i: "<<i<<"\n";
	for(int j=0;j<height;j++){
	  pixel=glm::vec3(0.0f,0.0f,0.0f);
	  for(int k=0;k<samples*samples;k++){
		float pixx=((float)(k%samples)+jitter?dist(gen):0.5f)/(float)samples,
		  pixy=((float)(k/samples)+jitter?dist(gen):0.5f)/(float)samples;
		intersection->reset();
		colour=glm::vec3(0.0f,0.0f,0.0f);

		ray->origin=cam->position;
		ray->direction=(left+(right-left)*(((float)i+pixx)/width))*u+(bottom+(top-bottom)*(((float)j+pixy)/height))*v-d*w;
		//std::cout<<"ray dir: "<<ray->direction.x<<", "<<ray->direction.y<<", "<<ray->direction.z<<"\n";
	  
		for(auto s:shapes){
		  s->intersect(ray,intersection,false);
		}

		if(intersection->t<FLT_MAX){
		  colour=ambient;
		  auto m=intersection->material;
		  glm::vec3 v=glm::normalize(ray->direction)*-1.f;
		  shadowRay->origin=intersection->p;
		  for(auto l:lights){
			glm::vec3 ld=l->dir(intersection->p);
			shadowIntersection->reset();
			shadowRay->direction=ld;
			for(auto s:shapes){
			  s->intersect(shadowRay,shadowIntersection,true);
			  if(shadowIntersection->t<FLT_MAX)break;
			}
			if(shadowIntersection->t<FLT_MAX)continue;
			colour+=l->colour*
			  (m->diffuse*glm::max(0.f,glm::dot(intersection->n,ld))
			   +m->specular*glm::pow(glm::max(0.f,glm::dot(intersection->n,glm::normalize(ld+v))),m->hardness));
		  }
		  if(m->mirror.r>0.f||m->mirror.g>0.f||m->mirror.b>0.f){
			float cos=glm::dot(intersection->n,v);
			float coeff=glm::pow(1.f-cos,5);
			glm::vec3 colourref=glm::vec3(0.f,0.f,0.f),fres=m->mirror*(1.f-coeff)+coeff;
			
			ray->origin=intersection->p;
			ray->direction=2.f*cos*intersection->n-v;
			
			intersection->reset();
			for(auto s:shapes){
			  s->intersect(ray,intersection,false);
			}
			
			if(intersection->t<FLT_MAX){
			  auto m=intersection->material;
			  glm::vec3 v=glm::normalize(ray->direction)*-1.f;
			  shadowRay->origin=intersection->p;
			  for(auto l:lights){
				glm::vec3 ld=l->dir(intersection->p);
				shadowIntersection->reset();
				shadowRay->direction=ld;
				for(auto s:shapes){
				  s->intersect(shadowRay,shadowIntersection,true);
				  if(shadowIntersection->t<FLT_MAX)break;
				}
				if(shadowIntersection->t<FLT_MAX)continue;
				colourref+=l->colour*
				  (m->diffuse*glm::max(0.f,glm::dot(intersection->n,ld))
				   +m->specular*glm::pow(glm::max(0.f,glm::dot(intersection->n,glm::normalize(ld+v))),m->hardness));
			  }
			  colour+=colourref*fres;
			}
			
		  }
		  colour.r=glm::min(1.0f,colour.r);
		  colour.g=glm::min(1.0f,colour.g);
		  colour.b=glm::min(1.0f,colour.b);
		  pixel+=colour/(float)(samples*samples);
		}
	  }
	  // Write pixel colour to image
	  pixel*=255;
	  image->setPixel(i,j,pixel.r,pixel.g,pixel.b);
	}
  }
}

void Scene::writeFile() {
  image->writeToFile(outputFilename);
}
