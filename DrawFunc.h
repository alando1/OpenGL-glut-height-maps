#pragma once
#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include "vector.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <SOIL.h>

void drawgrid();
void otherGrid();
void drawFloor();
void texturePlane(int testTexture, float length, float width);
//void setVSync(int i);