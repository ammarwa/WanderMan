#pragma once

#include <windows.h>									// Header File For The Windows Library
#include <stdio.h>										// Header File For The Standard Input/Output Functions
#include <stdlib.h>										// Header File For The Standard Library
#include <math.h>										// Header File For The Math header file
#include ".\gl\gl.h"											// Header File For The OpenGL32 Library
#include ".\gl\glu.h"									// Header File For The GLu32 Library
#include "glaux.h"									// Header File For The Glaux Library


#define GL_CLAMP_TO_EDGE	0x812F						// 


class MTexture
{
public:
	MTexture();
	~MTexture();
	bool CreateFromBMP(UINT *textureID, LPSTR strFileName);
};
