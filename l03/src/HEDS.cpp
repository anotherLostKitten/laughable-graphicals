//theodore peters 260919785

#include "HEDS.h"

HEDS::HEDS(shared_ptr<PolygonSoup>soup){
  halfEdges->clear();
  faces->clear();
  faces->reserve(soup->faceList.size());
  vertices=soup->vertexList;
  for(auto&v:*vertices)
	v->n=glm::vec3(0,0,0);
  for(auto&face:soup->faceList)
	for(int i=2;i<face.size();i++){
	  HalfEdge*he1=createHalfEdge(soup,face[0],face[i-1]);
	  HalfEdge*he2=createHalfEdge(soup,face[i-1],face[i]);
	  HalfEdge*he3=createHalfEdge(soup,face[i],face[0]);
	  he1->next=he2;
	  he2->next=he3;
	  he3->next=he1;
	  Face*f=new Face(he1);
	  (*faces).push_back(f);
	  he1->head->n+=f->n;
	  he2->head->n+=f->n;
	  he3->head->n+=f->n;
	  he1->head->area+=f->area;
	  he2->head->area+=f->area;
	  he3->head->area+=f->area;
	  double crs=2.*f->area;
	  he1->cwn=glm::dot(-he1->e,he2->e)/crs;
	  he2->cwn=glm::dot(-he2->e,he3->e)/crs;
	  he3->cwn=glm::dot(-he3->e,he1->e)/crs;
	}
  for(auto&v:*vertices){
	v->n=glm::normalize(v->n);
	v->area*=1./3.;
	v->invarea=1./v->area;
	//cout<<"valence: "<<v->valence()<<"\n";
  }
}

HalfEdge*HEDS::createHalfEdge(shared_ptr<PolygonSoup>soup,unsigned int i,unsigned int j){
  unsigned long int p=(i>j)?((unsigned long int)i<<32)+j:((unsigned long int)j<<32)+i;
  HalfEdge*he=new HalfEdge();
  he->head=soup->vertexList->at(j);
  he->head->he=he;//make sure the vertex has at least one half edge that points to it.
  he->e=he->head->p-soup->vertexList->at(i)->p;
  auto twine=halfEdges->extract(p);
  if(twine){
	he->twin=twine.mapped();
	he->twin->twin=he;
  }else
	halfEdges->emplace(p,he);
  return he;
}

void HEDS::initHeatFlow(){
  for(auto&v:*vertices){
	if (v->constrained){
	  v->u0=1;
	  v->ut=1;
	}else{
	  v->u0=0;
	  v->ut=0;
	}
	v->phi=0;
  }
}

void HEDS::solveHeatFlowStep(int GSSteps,double t){
  // we'll naively choose some random vertex as a source, and
  // then do lots of GS iterations on that for a backward Euler solve of
  // (A-tL) u_t = u_0

  // what is a good value of t?  t small for accuracy, but t large for possibly floating point error
  for(int i=0;i<GSSteps;i++){
	for(auto&v:*vertices){
	  if(v->constrained)
		continue; // do nothing for the constrained vertex!
	  HalfEdge*he=v->he->twin,*end=v->he->twin;
	  int j=0;
	  double vls=0.;
	  do{
		vls+=he->head->ut*v->Lij[j++];
		he=he->twin->next;
	  }while(he!=end);
	  v->ut=(v->u0+vls*t)/(v->area-v->Lii*t);
	  v->ut_float=(float)v->ut;
	}
  }
}

void HEDS::precomputeQuantities(){
  /**
   * TODO: you can do some pre-computation here to make things faster!
   */

  //for(auto&f:*faces){
  //HalfEdge*he=f->he;
  //}

  //for(auto &v:*vertices){
  //v->divX=0;
  //}
}

void HEDS::updateDivx(){
  // Compute the divergence of these normalized grad u vectors, at vertex locations
  for(auto &v:*vertices){
	HalfEdge*he=v->he,*end=v->he;
	double vls=0.;
	do{
	  glm::vec3 grd=he->leftFace->gradu;
	  HalfEdge*oth=he->next;
	  vls+=oth->next->cwn*glm::dot(oth->e,grd)-oth->cwn*glm::dot(he->e,grd);
	  he=oth->twin;
	}while(he!=end);
	v->divX=vls*0.5;

	/**
	 * TODO: 9 Update the divergence of the normalized gradients, ie., v.divX for each Vertex v.
	 */
  }
}

void HEDS::updateGradu(){
  for(auto&f:*faces){
	HalfEdge*he1=f->he;
	HalfEdge*he2=he1->next;
	HalfEdge*he3=he2->next;
	f->gradu=glm::normalize(he1->head->ut_float*glm::cross(he3->e,f->n)+
							he2->head->ut_float*glm::cross(he1->e,f->n)+
							he3->head->ut_float*glm::cross(he2->e,f->n));
  }
}

void HEDS::solveDistanceStep(int GSSteps){
  // Finally step the solution to the distance problem
  for(int i=0;i<GSSteps;i++){
	for(auto&v:*vertices){
	  // LHS matrix is L, so to take all the LHS to the RSH for one variable we get
	  // Lii phi_i = div X + sum_{j!=i} tLij phi_j
	  /**
	   * TODO: 10 Implement the inner loop of the Gauss-Seidel solve to compute the distances to each vertex, phi.
	   */
	}
  }

  // Note that the solution to step III is unique only up to an additive constant,
  // final values simply need to be shifted such that the smallest distance is zero. 
  // We also identify the max phi value here to identify the maximum geodesic and to 
  // use adjusting the colour map for rendering
  minphi=DBL_MAX;
  maxphi=DBL_MIN;
  for(auto&v:*vertices){
	if(v->phi<minphi)
	  minphi=v->phi;
	if(v->phi>maxphi)
	  maxphi=v->phi;
  }
  maxphi-=minphi;
  for(auto&v:*vertices)
	v->phi-=minphi;
}

void HEDS::computeLaplacian(){
  for(auto&v:*vertices){
	v->Lii=0.;
	int degree=v->valence();
	v->Lij=new double[degree];
	HalfEdge*he=v->he;
	for(int i=0;i<degree;i++){
	  double cots=(he->next->cwn+he->twin->next->cwn)/2.;
	  v->Lii-=cots;
	  v->Lij[i]=cots;
	  he=he->next->twin;
	}
	//cout<<"Lii: "<<v->Lii<<" Lij[0]: "<<v->Lij[0]<<"\n";
  }
}
