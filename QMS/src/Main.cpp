#include <iostream>
#include <fstream>
#include "Mesh.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdexcept>
#include <cmath>
using namespace std;

Mesh *mesh;
int MAX_ATTEMPTS_FIND_ONE_POLYCHORD = 10;
bool showMesh = true, showDual = false, showChords = false;
int angle = 0;
float zoomLevel = 120.0f;

void initializeMeshToBePassedToGlut(char* plyfilename)
{
	cout << "ply : " << plyfilename << endl;
	mesh = new Mesh();
	//mesh->meshFileLoader("hand.ply");
	mesh->meshFileLoader(plyfilename);
	mesh->constructDual();

	//mesh->getRandomEdge();//test that we are picking random edges correctly from the original polygon mesh

	bool hasAtleastOnePolychordBeenFound = false;
	for(int i = 0; i < MAX_ATTEMPTS_FIND_ONE_POLYCHORD; i++){
		hasAtleastOnePolychordBeenFound = mesh->checkAndPickAPolychordInQuadrilateralMeshStartingAtEdge(mesh->getRandomEdge());
		if(hasAtleastOnePolychordBeenFound)
			break;
	}

	if(hasAtleastOnePolychordBeenFound)
		cout << "CELEBRATE : FOUND ONE POLYCHORD <quad chain loop for hexahedral meshes, as in the research paper pdf>" << endl;
	else{
		cout << "xxxxxxxxxxxxxxxxxxxxx haven't found any POLYCHORD xxxxxxxxxxxxxxxxxxxxxxx even after << " << MAX_ATTEMPTS_FIND_ONE_POLYCHORD << " attempts - exiting! bye!" << endl;
		//exit(1);
	}
}

void initializeSaneStaticGlutDefaults(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (640, 640);
	glutInitWindowPosition (50, 50);
	glutCreateWindow ("Quadrilateral Mesh Simplification - using Polychord collapse");
    //background color setting for screen
	//glClearColor(0.3f,0.4f,0.5f,0.3f);
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

}

void reshape(int width, int height) {
	cout << "RESHAPE CALLED ------------------------------------------------->>>>>>>>>>>>>>>>>>>" << endl;
	if (height == 0) height = 1;
	glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(0, width-1, 0, height-1, 0, 1);
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

void display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective( /* field of view in degree */ 45.0,
	    /* aspect ratio */ 1.0,
	    /* Z near */ 1.0, /* Z far */ 100.0);
	glMatrixMode(GL_MODELVIEW);

	/* Adjust cube position to be asthetic angle. */
	glPushMatrix();
	gluLookAt(	0, 0, 3,		  /* eye is at (0,0,5) */
				0, 0, 0,		      /* center is at (0,0,0) */
				0.0, 1.0, 0.0);    /* up is in positive Y direction */
	//glRotatef(angle, 1.0, 1.0, 0.0);
	glScalef(zoomLevel,zoomLevel,zoomLevel);
	mesh->render(showMesh, showDual, showChords);
	glPopMatrix();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	if((key == 'm') || (key == 'M'))
		showMesh = !showMesh;
	else if((key == 'd') || (key == 'D'))
		showDual = !showDual;
	else if((key == 'c') || (key == 'C'))
		showChords = !showChords;
	else if((key == 'z') || (key == 'Z'))
		zoomLevel = zoomLevel-5;
	else if((key == 'x') || (key == 'X'))
		zoomLevel = zoomLevel+5;

	//glutPostRedisplay();
}

void changeAngle(int value) {
    angle = (angle+5) % 360;

    glutTimerFunc(500, changeAngle, 0);

    glutPostRedisplay();
}

void initializeDynamicGlutConfiguration(){
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
//	glutSpecialFunc(processSpecialKeys);
    glutTimerFunc(500, changeAngle, 0);
	glutMainLoop();
}


int main(int argc, char** argv)
{
   if(argc < 2){
   		cout <<  "need two things : executablename plyfilenameinsamefolder ... will continue with some default ply file" << endl;
   		argv[1] = (char *)"Ringquad.ply";
   }

   initializeSaneStaticGlutDefaults(argc, argv);
   initializeMeshToBePassedToGlut(argv[1]);

   initializeDynamicGlutConfiguration();

   return 0;
}
