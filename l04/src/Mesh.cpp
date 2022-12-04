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

Mesh::Mesh(const std::string&meshName,glm::vec3 position):filepath(meshName){
  loadOBJ(meshName);
  translate(position);
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
  faceCount=int(posBuf.size()/9.0f);
}

void Mesh::intersect(const std::shared_ptr<Ray>ray,std::shared_ptr<IntersectionData>intersection,bool shad,int thrd){
  // TODO some volume hierarchy
  for(int ii=0,jj=0;ii<faceCount;++ii&&(jj+=9)){
	float a=posBuf[jj]-posBuf[jj+3],d=posBuf[jj]-posBuf[jj+6],g=ray->direction.x,j=posBuf[jj]-ray->origin.x,
	  b=posBuf[jj+1]-posBuf[jj+4],e=posBuf[jj+1]-posBuf[jj+7],h=ray->direction.y,k=posBuf[jj+1]-ray->origin.y,
	  c=posBuf[jj+2]-posBuf[jj+5],f=posBuf[jj+2]-posBuf[jj+8],i=ray->direction.z,l=posBuf[jj+2]-ray->origin.z;
	float eihf=e*i-h*f,gfdi=g*f-d*i,dheg=d*h-e*g;
	float invd=1/(a*eihf+b*gfdi+c*dheg);
	float beta=(j*eihf+k*gfdi+l*dheg)*invd;
	if(beta<0.f||beta>1.f)continue;
	float akjb=a*k-j*b,jcal=j*c-a*l,blkc=b*l-k*c;
	float gamma=(i*akjb+h*jcal+g*blkc)*invd;
	if(gamma<0.f||gamma>1.f-beta)continue;
	float t=-(f*akjb+e*jcal+d*blkc)*invd;
	if(t>=MESH_EPSILON&&t<intersection->t){
	  intersection->t=t;
	  if(shad)return;
	  ray->computePoint(t,intersection->p);
	  intersection->n=glm::normalize(glm::vec3(b*f-c*e,c*d-a*f,a*e-b*d));
	  intersection->material=materials[0];
	  }
	  
	  // v1
	  //a.x posBuf[j]
	  //a.y posBuf[j+1]
	  //a.z posBuf[j+2]

	  // v2
	  //b.x posBuf[j+3]
	  //b.y posBuf[j+4]
	  //b.z posBuf[j+5]
	  
	  // v3
	  //c.x posBuf[j+6]
	  //c.y posBuf[j+7]
	  //c.z posBuf[j+8]
  }
  
}
