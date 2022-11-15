#include "GLHeaders.h"

#include "HalfEdge.h"

int Vertex::valence() {
    int v = 0;

    /**
     * TODO: 5 compute the valence of this vertex
     */
    
    HalfEdge *vertexHE = this->he;
    do{
        v++;
        vertexHE = vertexHE->next->twin;
    }while(vertexHE != this->he);

    return v;
}
