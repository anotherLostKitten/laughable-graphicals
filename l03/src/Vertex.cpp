//theodore peters 260919785

#include "GLHeaders.h"

#include "HalfEdge.h"

int Vertex::valence() {
  int v=0;
  /**
   * TODO: 5 compute the valence of this vertex
   */
  HalfEdge*tmp=he;
  do{
	v++;
	if(tmp==NULL){
	  tmp=he->twin;
	  while(tmp!=NULL){
		v++;
		tmp=tmp->next->next->twin;
	  }
	  return v;
	}
	tmp=tmp->next->twin;
  }while(tmp!=he);
  return v;
}
