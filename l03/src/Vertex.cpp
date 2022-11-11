//theodore peters 260919785

#include "GLHeaders.h"

#include "HalfEdge.h"
#include "Vertex.h"

int Vertex::valence(){
  int v=0;
  HalfEdge*tmp=he;
  do{
	v++;
	if(tmp==NULL){
	  throw runtime_error("vertex with boundary");//comment out if you want to have these but laplacian will break
	  for(tmp=he->twin;tmp!=NULL;tmp=tmp->next->next->twin)
		v++;
	  return v;
	}
	tmp=tmp->next->twin;
  }while(tmp!=he);
  return v;
}
