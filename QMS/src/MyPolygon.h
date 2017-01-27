#if !defined(MYPOLYGON)
#define MYPOLYGON

#include "Edge.h"
using namespace std;

struct Edge;

struct MyPolygon
{
	int index;
	int vert[6];
	int vi[4];
	vector<Edge*> edges;
	Vertex* myCentroidDualVertex;
	bool hasBeenProcessedForDual;

	Edge* getOppositeEdgeOfQuadrilateral(Edge* oneEdge){
		vector<Edge*>::iterator iter;
		int i=0;
		for(iter = edges.begin(); iter != edges.end(); iter++, i++){
			Edge* e = *iter;
			if(e == oneEdge)
				break;
		}
		return edges[ (i+2) % edges.size()];//in a quadrilateral, edges.size() should be 4
	}

	vector<MyPolygon*> getNeighbourQuadrilaterals(){
		vector<MyPolygon*> neighborQuadrilaterals;

		vector<Edge*>::iterator iter;
		int i=0;
		for(iter = edges.begin(); iter != edges.end(); iter++, i++){
			Edge* e = *iter;

			if(e->associatedPolygons.size() >= 2){
				MyPolygon* quad = ( (e->associatedPolygons[0] == this) ? e->associatedPolygons[1] : e->associatedPolygons[0] );//in a quadrilateral mesh, every edge has only two associated quadrilaters = one is itself, the other is a neighbour : 4 edges => 4 neighbours are fetched every time
				neighborQuadrilaterals.push_back(quad);
			}// else it means that number of associated polygons associated with this edge is only 1 & that happens to be the "this" current polygon / quadrilateral itself => no neighbour quadrilaterals
		}

		return neighborQuadrilaterals;
	}
};

#endif
