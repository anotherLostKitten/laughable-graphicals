//theodore peters 260919785

#include "Mesh.h"
#include "Ray.h"
#include "IntersectionData.h"

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

float MESH_EPSILON=1e-4; // To prevent shadow acne

Mesh::Mesh(const std::string&meshName):filepath(meshName){
  loadOBJ(meshName);
}

Mesh::Mesh(const std::string&meshName,glm::vec3 position,float s):filepath(meshName){
  loadOBJ(meshName);
  scale(s);
  translate(position);
  std::cout<<"Building HBV for "<<name<<" "<<type<<"...\n";
  hbv=std::make_shared<HBV>();
  genHBV(0,faceCount,0,hbv);
  std::cout<<"Done\n";

}

Mesh::~Mesh(){
}

void Mesh::translate(glm::vec3 position){
  int j=0;
  for(int i=0;i<faceCount;i++){
	j=9*i;
	// v1
	posBuf[j+0]=posBuf[j+0]+position.x;
	posBuf[j+1]=posBuf[j+1]+position.y;
	posBuf[j+2]=posBuf[j+2]+position.z;

	// v2
	posBuf[j+3]=posBuf[j+3]+position.x;
	posBuf[j+4]=posBuf[j+4]+position.y;
	posBuf[j+5]=posBuf[j+5]+position.z;

	// v3
	posBuf[j+6]=posBuf[j+6]+position.x;
	posBuf[j+7]=posBuf[j+7]+position.y;
	posBuf[j+8]=posBuf[j+8]+position.z;
  }
}

void Mesh::scale(float s){
  for(int i=0;i<posBuf.size();i++){
	posBuf[i]=s*posBuf[i];
  }
}

void Mesh::loadOBJ(const std::string&meshName){
  // Load geometry
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t>shapes;
  std::vector<tinyobj::material_t>materials;
  std::string warnStr,errStr;
  bool rc=tinyobj::LoadObj(&attrib,&shapes,&materials,&warnStr,&errStr,meshName.c_str());
  if(!rc){
	std::cerr<<errStr<<std::endl;
  }else{
	// Some OBJ files have different indices for vertex positions, normals,
	// and texture coordinates. For example, a cube corner vertex may have
	// three different normals. Here, we are going to duplicate all such
	// vertices.
	// Loop over shapes
	for(size_t s=0;s<shapes.size();s++){
	  // Loop over faces (polygons)
	  size_t index_offset=0;
	  for(size_t f=0;f<shapes[s].mesh.num_face_vertices.size();f++){
		size_t fv=shapes[s].mesh.num_face_vertices[f];
		// Loop over vertices in the face.
		for(size_t v=0;v<fv;v++){
		  // access to vertex
		  tinyobj::index_t idx=shapes[s].mesh.indices[index_offset+v];
		  posBuf.push_back(attrib.vertices[3*idx.vertex_index+0]);
		  posBuf.push_back(attrib.vertices[3*idx.vertex_index+1]);
		  posBuf.push_back(attrib.vertices[3*idx.vertex_index+2]);
		  if(!attrib.normals.empty()){
			norBuf.push_back(attrib.normals[3*idx.normal_index+0]);
			norBuf.push_back(attrib.normals[3*idx.normal_index+1]);
			norBuf.push_back(attrib.normals[3*idx.normal_index+2]);
		  }
		  if(!attrib.texcoords.empty()){
			texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
			texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
		  }
		}
		index_offset+=fv;
		// per-face material (IGNORE)
		shapes[s].mesh.material_ids[f];
	  }
	}
  }
  faceCount=posBuf.size()/9;
  //hbv->print("");
}

void Mesh::swap(int i,int j){
  for(int k=0;k<9;k++){
	float tmp=posBuf[i*9+k];
    posBuf[i*9+k]=posBuf[j*9+k];
	posBuf[j*9+k]=tmp;
  }
  if(norBuf.size()){
	for(int k=0;k<9;k++){
	  float tmp=posBuf[i*9+k];
	  posBuf[i*9+k]=posBuf[j*9+k];
	  posBuf[j*9+k]=tmp;
	}
  }
  if(texBuf.size()){
	for(int k=0;k<6;k++){
	  float tmp=posBuf[i*6+k];
	  posBuf[i*6+k]=posBuf[j*6+k];
	  posBuf[j*6+k]=tmp;
	}
  }
}

int Mesh::partition(int s,int e,int axis,float piv){
  e--;
  if(s>=e)return s;
  while(true){
	while(glm::max(glm::max(posBuf[s*9+axis],posBuf[s*9+axis+3]),posBuf[s*9+axis+6])<=piv&&s<e)
	  s++;
	while(glm::min(glm::min(posBuf[s*9+axis],posBuf[s*9+axis+3]),posBuf[s*9+axis+6])>piv&&s<e)
	  e--;
	if(s>=e)
	  return s;
	swap(s,e);
	s++;
	e--;
  }
}

void Mesh::genHBV(int s,int e,int axis,std::shared_ptr<HBV>root){
  if(s+1==e){
	root->facepos=s;
	root->left=NULL;
	root->right=NULL;
	root->minpt=glm::vec3(glm::min(glm::min(posBuf[s*9],posBuf[s*9+3]),posBuf[s*9+6]),
						 glm::min(glm::min(posBuf[s*9+1],posBuf[s*9+4]),posBuf[s*9+7]),
						 glm::min(glm::min(posBuf[s*9+2],posBuf[s*9+5]),posBuf[s*9+8]));
	root->maxpt=glm::vec3(glm::max(glm::max(posBuf[s*9],posBuf[s*9+3]),posBuf[s*9+6]),
						 glm::max(glm::max(posBuf[s*9+1],posBuf[s*9+4]),posBuf[s*9+7]),
						 glm::max(glm::max(posBuf[s*9+2],posBuf[s*9+5]),posBuf[s*9+8]));
  }else{
	root->facepos=-1;
	root->right=std::make_shared<HBV>();
	root->left=std::make_shared<HBV>();
	if(s+2==e){
	  genHBV(s,s+1,axis,root->right);
	  genHBV(s+1,e,axis,root->left);
	}else{
	  float max=FLT_MIN,min=FLT_MAX,tmp;
	  for(int i=s;i<e;i++){
		tmp=glm::min(glm::min(posBuf[s*9+axis],posBuf[s*9+axis+3]),posBuf[s*9+axis+6]);
		if(tmp<min)
		  min=tmp;
		tmp=glm::max(glm::max(posBuf[s*9+axis],posBuf[s*9+axis+3]),posBuf[s*9+axis+6]);
		if(tmp>max)
		  max=tmp;
	  }
	  int part=partition(s,e,axis,(max+min)/2.f);
	  genHBV(s,part,(axis+1)%3,root->right);
	  genHBV(part,e,(axis+1)%3,root->left);
	}
	root->minpt=glm::vec3(glm::min(root->right->minpt.x,root->left->minpt.x),
						  glm::min(root->right->minpt.y,root->left->minpt.y),
						  glm::min(root->right->minpt.z,root->left->minpt.z));
	root->maxpt=glm::vec3(glm::max(root->right->maxpt.x,root->left->maxpt.x),
						  glm::max(root->right->maxpt.y,root->left->maxpt.y),
						  glm::max(root->right->maxpt.z,root->left->maxpt.z));
  }
}

void Mesh::intersect(const std::shared_ptr<Ray>ray,std::shared_ptr<IntersectionData>intersection,bool shad,int thrd){
  intrec(ray,intersection,shad,thrd,hbv);
}
void Mesh::intrec(const std::shared_ptr<Ray>ray,std::shared_ptr<IntersectionData>intersection,bool shad,int thrd,std::shared_ptr<HBV>root){
  if(root->facepos<0){
	if(root->intersect(ray,MESH_EPSILON,intersection->t)){
	  intrec(ray,intersection,shad,thrd,root->left);
	  intrec(ray,intersection,shad,thrd,root->right);
	}
  }else{
	int ii=root->facepos;
	int jj=ii*9;
	float a=posBuf[jj]-posBuf[jj+3],d=posBuf[jj]-posBuf[jj+6],g=ray->direction.x,j=posBuf[jj]-ray->origin.x,
	  b=posBuf[jj+1]-posBuf[jj+4],e=posBuf[jj+1]-posBuf[jj+7],h=ray->direction.y,k=posBuf[jj+1]-ray->origin.y,
	  c=posBuf[jj+2]-posBuf[jj+5],f=posBuf[jj+2]-posBuf[jj+8],i=ray->direction.z,l=posBuf[jj+2]-ray->origin.z;
	float eihf=e*i-h*f,gfdi=g*f-d*i,dheg=d*h-e*g;
	float invd=1/(a*eihf+b*gfdi+c*dheg);
	float beta=(j*eihf+k*gfdi+l*dheg)*invd;
	if(beta<0.f||beta>1.f)return;
	float akjb=a*k-j*b,jcal=j*c-a*l,blkc=b*l-k*c;
	float gamma=(i*akjb+h*jcal+g*blkc)*invd;
	if(gamma<0.f||gamma>1.f-beta)return;
	float t=-(f*akjb+e*jcal+d*blkc)*invd;
	if(t>=MESH_EPSILON&&t<intersection->t){
	  intersection->t=t;
	  if(shad)return;
	  ray->computePoint(t,intersection->p);
	  intersection->n=glm::normalize(glm::vec3(b*f-c*e,c*d-a*f,a*e-b*d));
	  intersection->material=materials[0];
	}
  }
  
}
