#if !defined(VERTEX)
#define VERTEX

#include "Edge.h"
using namespace std;

struct Vertex 
{
  int index;
  float x; 
  float y;
  float z;
  Edge* edge;
};

#endif
