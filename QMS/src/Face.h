#if !defined(FACE)
#define FACE

#include "Edge.h"
using namespace std;

struct Edge;
struct Face
{
  Edge* edge;
  int index;
};

#endif
