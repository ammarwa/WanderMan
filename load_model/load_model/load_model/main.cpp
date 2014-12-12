#include "stdafx.h"
//#include "..\stdafx.h"
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

#include <tchar.h>
using namespace std;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "GLAUX.lib")

void output(void);
void fill_array(void);

int array[251][21];

int lives;

int camera_mode;

double eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ;

int numOfObs = 10;
int i = 0;

int windowWidth = 1024;
int windowHeight = 668;

int manX, manZ;

bool pause;
bool game_over;
bool level_1, level_2, level_3;

int score;

float aspect = float(windowWidth) / float(windowHeight);

double eye [] = { -10, 1, -.5 };

double center [] = { 0, 0, 0 };

double up [] = { 0, 1, 0 };

double camModeX = eye[0];
double camModeZ = eye[2];


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

void fill_array(){
	int random;
	srand(time(NULL));
	//level 1
	for (int i = 15; i < 85; i+=15)
	{
		for (int j = 0; j < 21; j++)
		{
			random = rand()%10+1;
			if(random == 2){
				if(array[i][j-1] == 2)
				break;
			}
			array[i][j] = random;
		}
	}
	//level 2
	for (int i = 85; i < 170; i+=10)
	{
		for (int j = 0; j < 21; j++)
		{
			random = rand()%10+1;
			if(random == 2){
				if(array[i][j-1] == 2)
				break;
			}
			array[i][j] = random;
		}
	}
	//level 3
	for (int i = 170; i < 251; i+=5)
	{
		for (int j = 0; j < 21; j++)
		{
			random = rand()%10+1;
			if(random == 2){
				if(array[i][j-1] == 2)
				break;
			}
			array[i][j] = random;
		}
	}
	//Game Borders
	for (int i = 0; i < 251; i++)
	{
		array[i][0] = 2;
		array[i][20] = 2;
	}
	for (int i = 0; i < 21; i++)
	{
		array[250][i] = 2;
	}
}

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

void drawObs(int x, int z){
		glPushMatrix();
		if(mve>40-manX){
			mve = 0;
			mve = -manX;
			j++;
		}
		//glTranslated(30-mve,0,posX[j]);
		glTranslated(x, 0, z);
		drawmodel_obs();
		glPopMatrix();
}

void draw_all_obs(){
	for (int i = 0; i < manX + 60; i++)
	{
		for (int j = 0; j < 21; j++)
		{
			if(array[i][j] == 2)
			{
				drawObs(i, j);
			}
		}
	}
}

void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(camModeX+manX+camContX, eye[1]+camContY, camModeZ+camContZ, center[0]+manX, center[1], center[2], up[0], up[1], up[2]);
	gluLookAt(eyeX+manX, eyeY, eyeZ, centerX+manX, centerY, centerZ, upX, upY, upZ);

	//Sea
	glPushMatrix();
	CreateFromBMP(&texBufferID, "3.bmp"); 
	glEnable(GL_TEXTURE_2D);
	glTranslated(200,0,50);
	glScaled(50,1,25);
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
	glTranslated(850, 0, 10);
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
	glTranslated(manX, 0, manZ);
	glRotated(90, 0, 1, 0);
	glScaled(0.7, 0.7, 0.7);
	drawmodel_char();
	glPopMatrix();

	//Obs
	draw_all_obs();

	glFlush();
	glutSwapBuffers();

}

void anim(void){
	if(!game_over && !pause)
	{
		if(manX < 248){
			if(array[manX+2][manZ] == 2){
				lives--;
				score-=10;
				cout << "You have hit a wall, you have " << to_string(lives);
				if(lives == 1)cout << " life left, it's your last chance" << '\n' << '\n';
				else cout << " lives left" << '\n' << '\n';
				if(lives == 0)
				{
					game_over = true;
					cout << "You lose !! Game is Over !!" << "your score is: " << to_string(score) << '\n' << '\n';
				}
				else {
					cout << "Press p to proceed" << '\n' << '\n';
					pause = true;
				}
			}
			else{
				manX+=1;
				if(manX < 85 && !level_1){
					level_1 = true;
					score += 30;
					cout << "You are now in level 1" << '\n' << '\n';
				}
				else if(manX >= 85 && manX < 170 && !level_2){
					level_2 = true;
					score += 30;
					cout << "You entered now level 2" << '\n' << '\n';
				}
				else if(manX >= 170 && manX < 251 && !level_3){
					level_3 = true;
					score += 30;
					cout << "You entered now level 3, it's the final level !!" << '\n' << '\n';
				}
				glutPostRedisplay();
			}
		}
		else{
			game_over = true;
			cout << "You win !! Game is Over !!" << "your score is: " << to_string(score) << '\n' << '\n';
		}
		flag = true;
		mve+=1;
	}
}

void HandleSpecialKey(int k,int mx,int my)
{
	if(!game_over && !pause){
		switch(k){

			//Speeding Char
			case GLUT_KEY_UP: 
				if(startFlag)manX+=3;
				if(manX < 248){
					if(array[manX][manZ] == 2 || array[manX+1][manZ] == 2 || array[manX+2][manZ] == 2){
						lives--;
						score-=10;
						cout << "You have hit a wall, you have " << to_string(lives);
						if(lives == 1)cout << " life left, it's your last chance" << '\n' << '\n';
						else cout << " lives left" << '\n' << '\n';
						if(lives == 0)
						{
							game_over = true;
							cout << "You lose !! Game is Over !!"  << "your score is: " << to_string(score) << '\n' << '\n';
						}
						else {
							cout << "Press p to proceed" << '\n' << '\n';
							pause = true;
						}
						if(array[manX][manZ] == 2)manX-=2;
						else if(array[manX+1][manZ] == 2)manX-=1;
					}
				}
				else{
					manX = 248;
					game_over = true;
					cout << "You win !! Game is Over !!" << "your score is: " << to_string(score) << '\n' << '\n';
				}
				break;

			//Moving Char
			case GLUT_KEY_LEFT: 
				if(manZ > 1){
					manZ--;eyeZ--;centerZ--;
					break;
				}

			case GLUT_KEY_RIGHT: 
				if(manZ < 19){
					manZ++;eyeZ++;centerZ++;
					break;
				}
	
			default:;break;

		}
		glutPostRedisplay();
	}
}

void HandleHandleSpecialKeyPress( unsigned char k, int x, int y)
{
	switch(k){

	//Camera Modes
	//Far
	case 'z':
		if(camera_mode == 0){
			eyeX-=5;centerX-=5;eyeY+=0.5;
			camModeX = eye[0];camModeZ = eye[2];
			camera_mode = 1;
			cout << "Camera mode set to third person view" << '\n' << '\n';
		}
		break;
	//near
	case 'x':
		if(camera_mode == 1){
			eyeX+=5;centerX+=5;eyeY-=0.5;
			camModeX = eye[0]+4.5;camModeZ = eye[2];
			camera_mode = 0;
			cout << "Camera mode set to first person view" << '\n' << '\n';
		}
		break;

	//Camera Controls
	case 'w': camContX += 0;camContY += 1;camContZ += 1;break;
	case 'a': camContX -= 1;camContY += 0;camContZ -= 1;break;
	case 's': camContX += 0;camContY -= 1;camContZ -= 1;break;
	case 'd': camContX += 1;camContY += 0;camContZ += 1;break;

	//Game Start
	case 'n': startFlag = true;glutIdleFunc(anim);break;

	case 'r': eyeX+=1;break;
	case 'f': eyeX-=1;break;
	case 't': eyeY+=1;break;
	case 'g': eyeY-=1;break;
	case 'y': eyeZ+=1;break;
	case 'h': eyeZ-=1;break;
	case 'u': centerX+=1;break;
	case 'j': centerX-=1;break;
	case 'i': centerY+=1;break;
	case 'k': centerY-=1;break;
	case 'o': centerZ+=1;break;
	case 'l': centerZ-=1;break;
	
	case 'p': 
		if(pause){
			manX+=4;
			pause = false;
			cout << "Proceeding !!" << '\n' << '\n';
		}
		break;

	}
	glutPostRedisplay();
}

void output(){
	cout << "eyeX: " << to_string(eyeX) << '\n' << '\n';
	cout << "eyeY: " << to_string(eyeY) << '\n' << '\n';
	cout << "eyeZ: " << to_string(eyeZ) << '\n' << '\n';
	cout << "centerX: " << to_string(centerX) << '\n' << '\n';
	cout << "centerY: " << to_string(centerY) << '\n' << '\n';
	cout << "centerZ: " << to_string(centerZ) << '\n' << '\n';
}

void welcome(){
	cout << "Welcome to Wander Man !!!" << '\n' << '\n';
	cout << ":::Game Instructions:::" << '\n';
	cout << "Press n to start" << '\n';
	cout << "Use left and right arrows to direct your player" << '\n';
	cout << "Use up key to accelerate" << '\n';
	cout << "Use x key for first person view mode and z key for third person view mode" << '\n' << '\n';
}

int main(void)
{
	genPos();
	welcome();
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
	fill_array();
	eyeX = -5;
	eyeY = 1;
	eyeZ = 9.7;
	centerX = 0;
	centerY = 0;
	centerZ = 10;
	upX = 0;
	upY = 1;
	upZ = 0;
	manX = 0;
	manZ = 10;
	lives = 3;
	camera_mode = 1;
	glutMainLoop();
	return 0;
}

