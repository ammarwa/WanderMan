#include <stdafx.h>
#include <stdlib.h>
//#include <windows.h>
#include "glew.h"
#include "glaux.h"
#include <glut.h>
#include "glm.h"
#include "TextureBuilder.h"
#include <math.h>
#include "glut.h"
#include <stdio.h>
#include<string.h>
#include <iostream>
#include <time.h>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "GLAUX.lib")

int numOfObs = 10;
int i = 0;

int windowWidth = 1024;
int windowHeight = 668;

double move = 0;

float aspect = float(windowWidth) / float(windowHeight);

double eye [] = { -10, 1, -.5 };

double center [] = { 0, 0, 0 };

double up [] = { 0, 1, 0 };

double camModeX = eye[0];
double camModeZ = eye[2];
double moveD = 0;

double camContX = 0;
double camContY = 0;
double camContZ = 0;

GLMmodel* pmodel1 = NULL;
GLMmodel* pmodel2 = NULL;


GLuint texBufferID;
GLuint texBufferID1;

float xrot, yrot;

double posX[100];
double posZ[100];

int j = 0;

bool startFlag = false;

bool flag  = true;

int mve = 0;

int varb = 0;

void CreateFromBMP(UINT *textureID, LPSTR strFileName)
{
	AUX_RGBImageRec *pBitmap = NULL;
	FILE *pFile = NULL;									// The File Handle we will use to read the bitmap

	fopen_s(&pFile, strFileName,"r");						// Check To See If The File Exists

	if(pFile)											// If we have a valid file pointer we found the file
	{
		pBitmap = auxDIBImageLoadA(strFileName);			// Load the bitmap and store the data
	}
	else												// If we can't find the file, quit!
	{													// Prompt the error message
		MessageBoxA(NULL, "Couldn't find a texture!", "Error!", MB_OK);
		exit(0);
	}

	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, textureID);


	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, *textureID);

	// Build Mipmaps (builds different versions of the picture for distances - looks better)
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pBitmap->sizeX, pBitmap->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pBitmap->data);

	// Lastly, we need to tell OpenGL the quality of our texture map.  GL_LINEAR_MIPMAP_LINEAR
	// is the smoothest.  GL_LINEAR_MIPMAP_NEAREST is faster than GL_LINEAR_MIPMAP_LINEAR, 
	// but looks blochy and pixilated.  Good for slower computers though.  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// The default GL_TEXTURE_WRAP_S and ""_WRAP_T property is GL_REPEAT.
	// We need to turn this to GL_CLAMP_TO_EDGE, otherwise it creates ugly seems
	// in our sky box.  GL_CLAMP_TO_EDGE does not repeat when bound to an object.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Now we need to free the bitmap data that we loaded since openGL stored it as a texture

	if (pBitmap)										// If we loaded the bitmap
	{
		if (pBitmap->data)								// If there is texture data
		{
			free(pBitmap->data);						// Free the texture data, we don't need it anymore
		}

		free(pBitmap);									// Free the bitmap structure
	}
}

void CreateLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);
	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);
	// Define Light source 0 ambient light
	GLfloat ambient[]={0.1f, 0.1f, 0.1, 1.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	// Define Light source 0 diffuse light
	GLfloat diffuse[]={0.4f, 0.4f, 0.4f, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	// Define Light source 0 Specular light
	GLfloat specular[]={1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = {2.0f, 2.0f, 2.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void UseSurfaceMaterials()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);
	// Set Material Properties which will be assigned by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	// Set Material's Shine value (0->128)
	GLfloat shininess[] = {96.0f};
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void drawmodel_char(void)
{
	// Load the model only if it hasn't been loaded before
	// If it's been loaded then pmodel1 should be a pointer to the model geometry data...otherwise it's null
	if (!pmodel1)
	{
		// this is the call that actualy reads the OBJ and creates the model object
		pmodel1 = glmReadOBJ("Silver_Surfer.obj");
		if (!pmodel1) exit(0);
		// This will rescale the object to fit into the unity matrix
		// Depending on your project you might want to keep the original size and positions you had in 3DS Max or GMAX so you may have to comment this.
		glmUnitize(pmodel1);
		// These 2 functions calculate triangle and vertex normals from the geometry data.
		// To be honest I had some problem with very complex models that didn't look to good because of how vertex normals were calculated
		// So if you can export these directly from you modeling tool do it and comment these line
		// 3DS Max can calculate these for you and GLM is perfectly capable of loading them
		glmFacetNormals(pmodel1);
		glmVertexNormals(pmodel1, 90.0);
	}
	// This is the call that will actualy draw the model
	// Don't forget to tell it if you want textures or not :))
	glmDraw(pmodel1, GLM_SMOOTH | GLM_TEXTURE);

}


void drawmodel_obs(void)
{
	// Load the model only if it hasn't been loaded before
	// If it's been loaded then pmodel1 should be a pointer to the model geometry data...otherwise it's null
	if (!pmodel2)
	{
		// this is the call that actualy reads the OBJ and creates the model object
		pmodel2 = glmReadOBJ("box.obj");
		if (!pmodel2) exit(0);
		// This will rescale the object to fit into the unity matrix
		// Depending on your project you might want to keep the original size and positions you had in 3DS Max or GMAX so you may have to comment this.
		glmUnitize(pmodel2);
		// These 2 functions calculate triangle and vertex normals from the geometry data.
		// To be honest I had some problem with very complex models that didn't look to good because of how vertex normals were calculated
		// So if you can export these directly from you modeling tool do it and comment these line
		// 3DS Max can calculate these for you and GLM is perfectly capable of loading them
		glmFacetNormals(pmodel2);
		glmVertexNormals(pmodel2, 90.0);
	}
	// This is the call that will actualy draw the model
	// Don't forget to tell it if you want textures or not :))
	glmDraw(pmodel2, GLM_SMOOTH | GLM_TEXTURE);

}

void genPos(){
	for(int i = 0; i < 100; i++){
		srand(time(0));
		int xx = rand();
		srand(time(0));
		int zz = rand();
		if(i%2==0){
			posX[i] = xx%5;
		}else{
			posX[i] = -1*(zz%5);
		}
	}
}

void drawObs(){
		glPushMatrix();
		if(mve>40-move){
			mve = 0;
			mve = -move;
			j++;
		}
		glTranslated(30-mve,0,posX[j]);
		drawmodel_obs();
		glPopMatrix();
}

void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camModeX+move+camContX, eye[1]+camContY, camModeZ+camContZ, center[0]+move, center[1], center[2], up[0], up[1], up[2]);

	//Sea
	glPushMatrix();
	CreateFromBMP(&texBufferID, "3.bmp"); 
	glEnable(GL_TEXTURE_2D);
	glScaled(30,1,15);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-10.0f, -10.0f, -10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 10.0f, -10.0f, -10.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 10.0f, -10.0f,  10.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f, -10.0f,  10.0f);
	glEnd();
	glPopMatrix();

	//Sky
	glPushMatrix();
	CreateFromBMP(&texBufferID, "free_wallpaper_2.bmp"); 
	glEnable(GL_TEXTURE_2D);
	glTranslated(850,0,0);
	glScaled(1,20,50);
	glRotated(90,0,0,1);
	glRotated(90,0,1,0);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-10.0f, -10.0f, -10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 10.0f, -10.0f, -10.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 10.0f, -10.0f,  10.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f, -10.0f,  10.0f);
	glEnd();
	glPopMatrix();

	//Char
	glPushMatrix();
	glTranslated(-5+move,0,0+moveD);
	glRotated(90, 0, 1, 0);
	glScaled(0.7, 0.7, 0.7);
	drawmodel_char();
	glPopMatrix();

		//Obs
			drawObs();
		

	glFlush ();
	glutSwapBuffers();

}

void anim(void){
	move+=1;
	flag = true;
	mve+=1;
	glutPostRedisplay();
}

void HandleSpecialKey(int k,int mx,int my)
{
	
	switch(k){

	//Speeding Char
	case GLUT_KEY_UP: move+=3;break;

	//Moving Char
	case GLUT_KEY_LEFT: moveD--;break;
	case GLUT_KEY_RIGHT: moveD++;break;
	
	default:;break;

	}
	glutPostRedisplay();
}

void HandleHandleSpecialKeyPress( unsigned char k, int x, int y)
{
	switch(k){

	//Camera Modes
	//Far
	case 'z':
		camModeX = eye[0];camModeZ = eye[2];break;
	//near
	case 'x':
		camModeX = eye[0]+4.5;camModeZ = eye[2];break;

	//Camera Controls
	case 'w': camContX += 0;camContY += 1;camContZ += 1;break;
	case 'a': camContX -= 1;camContY += 0;camContZ -= 1;break;
	case 's': camContX += 0;camContY -= 1;camContZ -= 1;break;
	case 'd': camContX += 1;camContY += 0;camContZ += 1;break;

	//Game Start
	case 'n': startFlag = true;glutIdleFunc(anim);break;

	}
	glutPostRedisplay();
}

int main(void)
{
	genPos();
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("WanderMan");
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(30, aspect, 0.1, 1000);
	glutDisplayFunc(display);
	glutKeyboardFunc(HandleHandleSpecialKeyPress);
	glutSpecialFunc(HandleSpecialKey);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0.0,0.0,0.0,0.0);
	glutMainLoop();
	return 0;
}

