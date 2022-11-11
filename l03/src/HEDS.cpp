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
	}
  for(auto&v:*vertices)
	v->n=glm::normalize(v->n);
}

HalfEdge*HEDS::createHalfEdge(shared_ptr<PolygonSoup>soup,unsigned int i,unsigned int j){
  unsigned long int p=(i>j)?((unsigned long int)i<<32)+j:((unsigned long int)j<<32)+i;
  HalfEdge*he=new HalfEdge();
  he->head=soup->vertexList->at(j);
  he->head->he=he;//make sure the vertex has at least one half edge that points to it.
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
	  /**
	   * TODO: 7 write inner loop code for the PGS heat solve.
	   */
	}
  }
}

void HEDS::precomputeQuantities(){
  /**
   * TODO: you can do some pre-computation here to make things faster!
   */

  for(auto&f:*faces){
	HalfEdge*he=f->he;
  }

  for(auto &v:*vertices){
	v->divX=0;
  }
}

void HEDS::updateDivx(){
  // Compute the divergence of these normalized grad u vectors, at vertex locations
  for(auto &v:*vertices){
	v->divX=0;
	/**
	 * TODO: 9 Update the divergence of the normalized gradients, ie., v.divX for each Vertex v.
	 */
  }
}

void HEDS::updateGradu(){
  // do a pass to compute face gradients of u with the current solution
  for(auto&f:*faces){
	f->gradu[0]=0;
	f->gradu[1]=0;
	f->gradu[2]=0;
	/**
	 * TODO: 8 update the gradient of u from the heat values, i.e., f.gradu for each Face f.
	 */
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
	v->area=0;
	v->Lii=0;
	// get degree of v
	int degree=v->valence();
	v->Lij=new double[degree];

	/**
	 * TODO: 6 Compute the Laplacian and store as vertex weights, and cotan operator diagonal Lii and off diagonal Lij terms. 
	 */
        
  }
}

double HEDS::angleWithNext(HalfEdge*he){
  /**
   * TODO: 6 Implement this function to compute the angle with next edge... you'll want to use this in a few places.
   */
  return 0;
}