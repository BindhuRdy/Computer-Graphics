#if !defined(EDGE)
#define EDGE

#include "Vertex.h"
#include "Face.h"
using namespace std;
struct Vertex;
struct MyPolygon;
struct Edge
{
  int start_vertex_index;
  int end_vertex_index;
  Face* face;
  Edge* next;
  Edge* prev;
  Edge* twin_e;
  vector<MyPolygon*> associatedPolygons;
};

#endif
