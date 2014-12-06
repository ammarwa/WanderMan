//
//  main.cpp
//  WanderMan
//
//  Created by Ammar Mohamed ELWazir on 12/6/14.
//  Copyright (c) 2014 Ammar Mohamed ELWazir. All rights reserved.
//

#include <iostream>
#include <GLUT/glut.h>

void Display(){
    
}

void anim(){
    
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(1000,600);
    glutInitWindowPosition(200, 150);
    glutCreateWindow("WanderMan");
    glutDisplayFunc(Display);
    //glutKeyboardFunc(HandleHandleSpecialKeyPress);
    //glutSpecialFunc(HandleSpecialKey);
    glutIdleFunc(anim);
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(1.0,1.0,1.0,0.0);
    glutMainLoop();
    
}
