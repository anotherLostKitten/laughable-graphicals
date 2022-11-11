//theodore peters 260919785

#include "GLHeaders.h"

#include "HalfEdge.h"

int Vertex::valence(){
  int v=0;
  HalfEdge*tmp=he;
  do{
	v++;
	if(tmp==NULL){
	  for(tmp=he->twin;tmp!=NULL;tmp=tmp->next->next->twin)
		v++;
	  return v;
	}
	tmp=tmp->next->twin;
  }while(tmp!=he);
  return v;
}
