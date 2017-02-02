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

#define SUN		0
#define MERCURY 1
#define VENUS	2
#define EARTH	3
#define MARS	4
#define JUPITER	5
#define SATURN	6
#define URANUS	7
#define NEPTUNE 8
#define	PLUTO	9

using namespace std;

class Planet
{

private:
	string name;
	float revolution, rotation;
	float radius;
	Vec3 pos;
	GLUquadricObj* quadric;
	int texture;
	Vec3 vector;

public:
	Planet();
	string getName(){return name;}
	float getAngle(){return revolution;}
	float getRadius(){return radius;}
	float getRotation(){return rotation;}
	void drawPlanet();
	Vec3 getPosition();
	float getDistance(){return pos.x;}
	void initialize(float X, float Y, float Z, GLUquadricObj* Quadric, string Name, 
						float Radius, float Revolution, float Rotation, int Texture);
};