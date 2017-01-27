#if !defined(MESH)
#define MESH

#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include "MyPolygon.h"
#include <set>
#include <ctime>
#include <queue>

using namespace std;

class Mesh 
{
	public:
    int polygonSize;
	int num_Vertices, num_Poly;
	vector<MyPolygon*> polygons_Array;
	vector<Edge*> edges_Array;
    vector<Vertex*> vertices_Array;
    vector< vector<MyPolygon*> > polyChords_Array;

	Mesh()
		: polygonSize(0), num_Vertices(0), num_Poly(0)
	      {}
	~Mesh() {}

	Edge* findIfEdgeWithGivenVertexIndicesIsAlreadyConstructed(int startVertexIndex, int endVertexIndex){
		vector<Edge*>::iterator iter;
		for(iter = edges_Array.begin(); iter != edges_Array.end(); iter++){
			Edge* e = *iter;
			if((e->start_vertex_index == startVertexIndex) && (e->end_vertex_index == endVertexIndex))
				return e;
		}

		return NULL;
	}

	void meshFileLoader(char* filename)
	{
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~ loading ply file : " << filename << endl;

	        int temp=0;
	        float x,y,z;
	        char str[300];
	        FILE *file = fopen(filename,"r");

			if(!file){
				system("cd");
				perror("cant open ply file");
				throw std::invalid_argument( "received invalid ply file" );
			}

		while (  strncmp( "element vertex", str,strlen("element vertex")) != 0  )
				{
					fgets(str,300,file);			// format
				}
		strcpy(str, str+strlen("element vertex"));
		sscanf(str,"%i", &num_Vertices);
		cout<< "Number of vertices"<<num_Vertices<<endl;

		while (  strncmp( "element face", str,strlen("element face")) != 0  )
				{
					fgets(str,300,file);			// format
				}
		strcpy(str, str+strlen("element face"));
		sscanf(str,"%i", &num_Poly);
		cout<< "Number of faces" << num_Poly<<endl;

		do{
			fgets(str,300,file);
			cout << str << endl;
		}while(strncmp( "end_header", str,strlen("end_header")) != 0  );
		cout<<"Header ended";

		for(int i=0;i<num_Vertices;i++)
			{
				fgets(str,300,file);
				sscanf(str,"%f %f %f",&x,&y,&z);
				Vertex *v = new Vertex;
				v->x = x/num_Vertices;
				v->y = y/num_Vertices;
				v->z = z/num_Vertices;
				v->index = i;
				v->edge = NULL;

				vertices_Array.push_back(v);
			}
			cout << "################ vertices array size : " << vertices_Array.size() << endl;
			cout <<"faces";
			fgets(str,300,file);
			sscanf(str,"%d " ,&temp);
			printf("%d",temp);
			polygonSize = temp;
			cout << "polygonSize"<<polygonSize<<endl;
			int i=0;
            int flag = 0;
				do{
					MyPolygon* p  = new MyPolygon;
					p->vert[0] = p->vert[1] = p->vert[2] = p->vert[3] = p->vert[4] = -1;
				if(temp==3)
					sscanf(str,"%d %d %d %d",&temp,&p->vert[0],&p->vert[1],&p->vert[2]);
				if(temp==4){

					sscanf(str,"%d %d %d %d %d",&temp,&p->vert[0],&p->vert[1],&p->vert[2],&p->vert[3]);
					cout<<"bfore:";
					cout<<p->vert[0];
                    if((i==0) && ((p->vert[0] > 0) && (p->vert[1] > 0) && (p->vert[2] > 0) && (p->vert[3] > 0)))
		            {
					  flag = 1;
		            }
                    if(flag == 1){
                    	p->vert[0] = p->vert[0]-1;
						cout<<"After:";
						cout<<p->vert[0];
						p->vert[1] = p->vert[1]-1;
						p->vert[2] = p->vert[2]-1;
						p->vert[3] = p->vert[3]-1;
                    }
				}
				if(temp==5)
					sscanf(str,"%d %d %d %d %d %d",&temp,&p->vert[0],&p->vert[1],&p->vert[2],&p->vert[3],&p->vert[4]);

				printf("%d %d %d %d %d \n",temp,p->vert[0],p->vert[1],p->vert[2],p->vert[3]);

				if((p->vert[0] >= num_Vertices) || (p->vert[1] >= num_Vertices) || (p->vert[2] >= num_Vertices) || (p->vert[3] >= num_Vertices) || (p->vert[4] >= num_Vertices)){
					cout << "ply file is corrupt : line '" << str << "' has one or more vertices which is out of bounds : should be in the range : |0 ... (total number of vertices - 1)|";
					exit(-1);
				}

				p->index = i;
				p->hasBeenProcessedForDual = false;

				//////////////////////////
				for(int x = 0; x < polygonSize; x++){
					Edge* e = new Edge();
					int m = p->vert[x], n = p->vert[(x+1)%polygonSize];
					e->start_vertex_index = min(m,n);
					e->end_vertex_index = max(m,n);

					Edge* anyExistingEdge = findIfEdgeWithGivenVertexIndicesIsAlreadyConstructed(e->start_vertex_index, e->end_vertex_index);

					if(anyExistingEdge)
						e = anyExistingEdge;
					else
						edges_Array.push_back(e);

					p->edges.push_back(e);
					e->associatedPolygons.push_back(p);
				}
				//////////////////////////

				polygons_Array.push_back(p);

				fgets(str,300,file);
				i++;

				}while(i<num_Poly);

		fclose(file);
		cout << "load ok";

	}

	void computeNormals(Vertex* v1, Vertex* v2, Vertex* v3) const
	{
		float x_n, y_n, z_n;
        //calculate x normal
		x_n = (v1->y)*((v2->z)-(v3->z))+(v2->y)*((v3->z)-(v1->z))+(v3->y)*((v1->z)-(v2->z));
        //calculate y normal
		y_n = (v1->z)*((v2->x)-(v3->x))+(v2->z)*((v3->x)-(v1->x))+(v3->z)*((v1->x)-(v2->x));
        //calculate z normal
		z_n = (v1->x)*((v2->y)-(v3->y))+(v2->x)*((v3->y)-(v1->y))+(v3->x)*((v1->y)-(v2->y));
        //render the normal computed
		glNormal3f(x_n, y_n, z_n);
	}

	void triangulate()
	{
		cout << "before triangulation()------------------ " << endl;
		vector<MyPolygon*> new_Poly;
		if(polygonSize==4){
			for(int i=0;i<num_Poly;i++)
			{
				MyPolygon *p1 = new MyPolygon;
				MyPolygon *p2 = new MyPolygon;

				cout << "3 ";
				cout << (p1->vert[0] = polygons_Array[i]->vert[0]) << " ";
				cout << (p1->vert[1] = polygons_Array[i]->vert[1]) << " ";
				cout << (p1->vert[2] = polygons_Array[i]->vert[2]) << " ";
				cout << endl;

				cout << "3 ";
				cout << (p2->vert[0] = polygons_Array[i]->vert[0]) << " ";
				cout << (p2->vert[1] = polygons_Array[i]->vert[2]) << " ";
				cout << (p2->vert[2] = polygons_Array[i]->vert[3]) << " ";
				cout << endl;

				new_Poly.push_back(p1);
				new_Poly.push_back(p2);

			}
		}
		polygons_Array.clear();
		polygonSize = 3;
		polygons_Array =new_Poly;
		num_Poly = new_Poly.size();
		cout << "after triangulation()------------------ " << endl;
	}


	void constructDual(){
		int nvert = polygonSize;

		if((polygons_Array.size() == 0) || (nvert == 0))
			return;

		vector<MyPolygon*>::iterator iter;
		int i = 0;
		for(iter = polygons_Array.begin(); iter != polygons_Array.end(); iter++, i++)
		{
			MyPolygon* pol = *iter;

			if(pol == NULL)
			   continue;

			Vertex* dualVertex = new Vertex();
			dualVertex->index = i;
			dualVertex->x = dualVertex->y = dualVertex->z = 0;

			for(int j = 0; j < nvert; j++)
			{
			   unsigned int indexOfSomeVertex = pol->vert[j];

			   Vertex* vert = vertices_Array[indexOfSomeVertex];

			   dualVertex->x += (vert->x/nvert);
			   dualVertex->y += (vert->y/nvert);
			   dualVertex->z += (vert->z/nvert);
			}

			pol->myCentroidDualVertex = dualVertex;
		}
	}

	Edge* getRandomEdge(){
		if(edges_Array.size() == 0)
			return NULL;
		srand( time( NULL ) );
		unsigned int ran = rand() % edges_Array.size();
		cout << "got random edge with vertex indices : " << edges_Array[ran]->start_vertex_index << ", " << edges_Array[ran]->end_vertex_index << endl;
		return edges_Array[ran];
	}

	bool checkAndPickAPolychordInQuadrilateralMeshStartingAtEdge(Edge* startingEdge){
		if(startingEdge->associatedPolygons.size() == 0){
			cout << "no polygons share this edge with vertex indices : " << startingEdge->start_vertex_index << ", " << startingEdge->end_vertex_index << endl;
			cout << "hence cannot pick any polychord containing this edge" << endl;
			return false;
		}

		vector<MyPolygon*> polyChord;
		unsigned int countOfPolygonsVisited = 0;
		bool foundPolyChord = false;

		Edge* currentEdge = startingEdge;
		std::set<MyPolygon*> setOfVisitedPolygons;

		do {
			cout << "~~~ in polychord detection loop : picked up edge -> " << currentEdge->start_vertex_index << ", " << currentEdge->end_vertex_index << endl;

			if(currentEdge->associatedPolygons.size() < 2){
				cout << " we have hit an edge which belongs to only one quadrilateral - we need at least two to consider this sequence a polychord : halting polychord computation : try with another starting edge" << endl;
				break;
			}

			MyPolygon* currentPolygon = currentEdge->associatedPolygons[0];

			if(setOfVisitedPolygons.find(currentPolygon) != setOfVisitedPolygons.end())//found in our set => already visited
				currentPolygon = currentEdge->associatedPolygons[1];
			else
				setOfVisitedPolygons.insert(currentPolygon);

			polyChord.push_back(currentPolygon);

			currentEdge = currentPolygon->getOppositeEdgeOfQuadrilateral(currentEdge);

			if(currentEdge == startingEdge){//inner limit, narrower limit
				foundPolyChord = true;
				break;
			}

			countOfPolygonsVisited++;
		} while(countOfPolygonsVisited < polygons_Array.size());//outer limit, broader limit

		if(foundPolyChord)
			polyChords_Array.push_back(polyChord);

		return foundPolyChord;
	}

	void renderDual(){
		glPushMatrix();

		unsigned int countOfPolygonsVisited = 0;
		queue<MyPolygon*> queueOfPolygons;

		if(polygons_Array.size() == 0)
			return;

		for(unsigned int i = 0; i < polygons_Array.size(); i++)
			polygons_Array[i]->hasBeenProcessedForDual = false;/*initialize all polygons' dual processing status to false*/

		queueOfPolygons.push(polygons_Array[0]);

		while(! queueOfPolygons.empty())//&& (countOfPolygonsVisited < polygons_Array.size())
		{
			MyPolygon* pol = queueOfPolygons.front();

			queueOfPolygons.pop();

			if((pol == NULL) || (pol->hasBeenProcessedForDual))
			   continue;

			cout << "BEGIN : polygon " << pol->index << " visited for dual computation" << endl;

			//queue up neighbours to process next

			vector<MyPolygon*> neighboursOfPol = pol->getNeighbourQuadrilaterals();

			vector<MyPolygon*>::iterator iter;
			for(iter = neighboursOfPol.begin(); iter != neighboursOfPol.end(); iter++)
			{
				   MyPolygon* neighbourPol = *iter;
				   if(! neighbourPol->hasBeenProcessedForDual){
					   glBegin(GL_LINES);
                       //color setting for dual
					   //glColor3f(0.0f, 1.0f, 1.0f);
                       glColor3f(0.0f, 0.0f, 2.0f);
					   glVertex3f(pol->myCentroidDualVertex->x, pol->myCentroidDualVertex->y,  pol->myCentroidDualVertex->z);
					   //glColor3f(0.0f, 1.0f, 1.0f);
                       glColor3f(0.0f, 0.0f, 2.0f);
					   glVertex3f(neighbourPol->myCentroidDualVertex->x, neighbourPol->myCentroidDualVertex->y,  neighbourPol->myCentroidDualVertex->z);

					   glEnd();

					   queueOfPolygons.push(neighbourPol);
					   cout << ">>> QUEUE NEIGHBOUR : polygon " << neighbourPol->index << endl;
				   }
			}

			pol->hasBeenProcessedForDual = true;

			cout << "END : polygon " << pol->index << " visited for dual computation rendering" << endl;

			countOfPolygonsVisited++;
		}

		cout << "DUAL COMPUTATION RENDERING : " << countOfPolygonsVisited << " polygons have been visited out of a total of " << polygons_Array.size() << endl;

		glPopMatrix();
	}

	void renderPolychords(){

		glPushMatrix();

		glRotatef(-2, 0.0, 0.0, 1.0);

		vector< vector<MyPolygon*> >::iterator outeriter;
		for(outeriter = polyChords_Array.begin(); outeriter != polyChords_Array.end(); outeriter++)
		{
			vector<MyPolygon*> polyChord = *outeriter;

			vector<MyPolygon*>::iterator iter;
			for(iter = polyChord.begin(); iter != polyChord.end(); iter++)
			{
				   MyPolygon* pol = *iter;

				   if(pol == NULL)
					   continue;

				   glBegin(GL_LINE_LOOP);

				   for(int j = 0; j < polygonSize; j++){
					   unsigned int indexOfSomeVertex = pol->vert[j];
					   Vertex* vert = vertices_Array[indexOfSomeVertex];
					   if(vert == NULL)
						   continue;

					   cout << "POLYCHORD!!!!!!!!    ->    " << vert->x << "," << vert->y << "," <<  vert->z << endl;
					   //GLfloat d1[] = { 0.2, 0.5, 0.8, 1.0 };
					   //glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,d1);
					   //color setting for polychords
					   //glColor3f(1.0f, 1.0f, 1.0f);
                       glColor3f(2.0f, 0.0f, 0.0f);
					   glVertex3f(vert->x, vert->y,  vert->z);
				   }

				   glEnd();
			}
		}

		glPopMatrix();
	}

	void renderPolygonsMesh()
	{
		int nvert;
		vector<MyPolygon*>::iterator iter;
		Vertex *vert;
		MyPolygon* pol;

		if(polygonSize <= 2){
			cout << "weird polysize : " << polygonSize << ", has to be 3 or more" << endl;
			return;
		}
		else if(polygonSize == 3)
		{
			cout << "rendering TRIANGLES" << endl;
			//glBegin(GL_TRIANGLES);
			nvert = 3;
		}
		else if(polygonSize == 4)
		{
			cout << "rendering QUADS" << endl;
			//glBegin(GL_QUADS);
			nvert = 4;
		}
		else
		{
			cout << "rendering POLYGON" << endl;
			//glBegin(GL_POLYGON);
			nvert = 5;
		}

		int i=0;
		for(iter = polygons_Array.begin(); iter != polygons_Array.end(); iter++, i++)
		{
			   pol = *iter;
			   if(pol == NULL)
				   continue;

			   glBegin(GL_LINE_LOOP);

			   computeNormals(vertices_Array[pol->vert[0]], vertices_Array[pol->vert[1]], vertices_Array[pol->vert[2]]);//3 vertices suffice to define a plane
			   //glNormal3f(0.0f, 0.0f, 1.0f);

			   for(int j = 0; j < nvert; j++)
			   {
				   unsigned int indexOfSomeVertex = pol->vert[j];
				   if(indexOfSomeVertex >= vertices_Array.size()){
					   cout << i << "th polygon's " << j << "th vertex is out of bounds of vertices_Array's total size" << endl;
					   continue;
				   }

				   vert = vertices_Array[indexOfSomeVertex];
				   if(vert == NULL){
					   cout << i << "th polygon's " << j << "th vertex is null - check it" << endl;
					   continue;
				   }

				   cout << "    ->    " << vert->x << "," << vert->y << "," <<  vert->z << endl;
				   //color setting for rendering mesh
				   //glColor3f(1.0f, 0.0f, 0.0f);
                   glColor3f(0.5f, 0.5f, 0.5f);
				   glVertex3f(vert->x, vert->y,  vert->z);
			   }

			   cout << "polygon " << i << " done" << endl;

			   glEnd();
		}

		//glEnd();
	}

	void render(bool showPolygonMesh, bool showDual, bool showPolychords){
		if(showPolygonMesh)
			renderPolygonsMesh();
		if(showDual)
			renderDual();
		if(showPolychords)
			renderPolychords();
	}
	void quadVertexCollapse(){

		    int n_size;
		    vector<Vertex*> vertCurrentArray;
		    vector<Face*> faceCurrentArray;
		    vector<Face*> facePrevArray;
		    float sum_x, sum_y, sum_z;
			Vertex *vert_1, *vert_2, *vert_New;
		 	//Edge *edge;
		 	vector<int> neighbs;
		    int numVert = 0;
		    float  weight_2 = 0;
		     for(int i = 0;  i < numVert; i++)
		     {

		         n_size = numVert*i;
		         if(n_size == 3)       weight_2 = 0;
		         else if(n_size == 4)  weight_2 = 1;
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
void quadmeshsimplify(){
	        vector<MyPolygon*> polyChord;
			//unsigned int countOfPolygonsVisited = 0;
			bool foundPolyChord = false;

			//Edge* currentEdge;
			std::set<MyPolygon*> setOfVisitedPolygons;
	        do{
	            quadVertexCollapse();
	            quadEdgeCollapse();
	            //update foundPolyChord

	        }while(!foundPolyChord);

	}
	void quadEdgeCollapse(){
		 	Edge *e[3], *e_current1, *e_current2;
			//Face *f_one[3];
		 	//Vertex *v_norm[3]
		 	Vertex *v_ortho[3];
		    vector<Edge*> edgeCurrentArray;
		 	int k_1;

		 	int f_size = 0;
		 	for(int f_oneindx = 0;  f_oneindx < f_size; f_oneindx++)
		 	{
		 		e[1] = e[0]->next;
				e[2] = e[1]->next;
				int v_indx = 0;
				for(int k = 0;  k < 4;  k++)
				{    if(e[1])
		             v_indx = k;

		 		}
				v_indx++;
		 		for(int k = 0;  k < 3;  k++)
		 		{
		 			e_current1 = new Edge();
					e_current2 = new Edge();
					e[k]->next = e_current1;
		            e[k]->prev = e_current2;
		 			k_1 = (k+2)%3;
		            e_current1->prev = e[k];
		            e_current1->next = e_current2;
		            e_current2->prev = e_current1;
		            e_current2->next = e[k];
		 			v_ortho[k_1]->edge = e_current2;

		 		}

		}
		}


};

#endif
