#include "Edge.h"
using namespace std;
struct QMS
{
	int index;
	int e_n[4];
	int v_i[4];
	vector<Edge*> edges;
	void quadVertexCollapse(){

	    int n_size;
	    vector<Vertex*> vertCurrentArray;
	    vector<Face*> faceCurrentArray;
	    vector<Face*> facePrevArray;
	    float sum_x, sum_y, sum_z;
		Vertex *vert_1, *vert_2, *vert_3, *vert_4, *vert_New;
	 	Edge *edge;
	 	vector<int> neighbs;
	    int numVert = 0;
	    float weight_1 = 0, weight_2 = 0;
	     for(int i = 0;  i < numVert; i++)
	     {

	         n_size = numVert*i;
	         if(n_size == 3)       weight_2 = 0;
	         else if(n_size == 4)  weight_2 = 1;
	 		vert_1 = mesh->vertices_Array[i];
	 		sum_x = weight_2*(vert_1->x);
			sum_y = weight_2*(vert_1->y);
	 		sum_z = weight_2*(vert_1->z);
	 		for(int k = 0;  k < n_size; k++)
	 		{

				sum_x += weight_2*(vert_2->x);
				sum_y += weight_2*(vert_2->y);
				sum_z += weight_2*(vert_2->z);
		}
	    	vert_New = new Vertex;
			vert_New->x = sum_x;
	        vert_New->y = sum_y;
	        vert_New->z = sum_z;
			vert_New->index = i;
	        vert_New->edge = vert_1->edge;
	 		vertCurrentArray.push_back(vert_New);
	    }

	    }
	void quadEdgeCollapse(){
	 	Edge *e[3], *e_current1, *e_current2, *e_current3;
		Face *f_one[3];
	 	Vertex *v_norm[3], *v_ortho[3];
	    vector<Edge*> edgeCurrentArray;
	 	int k_1, k_2;

	 	int f_size = 0;
	 	for(int f_oneindx = 0;  f_oneindx < f_size; f_oneindx++)
	 	{
	 		e[1] = e[0]->next;
			e[2] = e[1]->next;

			for(int k = 0;  k < 4;  k++)
			{
	            int v_indx = k;
	            v_norm[k] = edgeCurrentArray[v_indx];
				v_ortho[k] = edgeCurrentArray[v_indx];
	 		}
	 		for(int k = 0;  k < 3;  k++)
	 		{
	 			e_current1 = new Edge();
				e_current2 = new Edge();
	            e[k]->vert = v_norm[k];
				e[k]->next = e_current1;
	            e[k]->prev = e_current2;
	 			k_1 = (k+2)%3;
	 			e_current1->vert = v_norm[k_1];
	            e_current1->prev = e[k];
	            e_current1->next = e_current2;
	 			e_current2->vert = v_ortho[k_1];
	            e_current2->prev = e_current1;
	            e_current2->next = e[k];
	 			v_ortho[k_1]->edge = e_current2;

	 		}

	}
	}

};
