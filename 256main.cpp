#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>
#include "DrawFunc.h"
#include "suppliedGlutFuncs.h"
#include "Terrain.h"
#include "vector.h"
#include <SOIL.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

#define GRAVITY -98.10f
#define imageSize 256

/*-----Points-in-Time-----*/
chrono::steady_clock::time_point currentTime, lastTime, jumpStart;

/*------Bool-Inputs-------*/
bool keyStates[256], keyTaps[256];
bool isCrouched = FALSE;
bool inJump = FALSE;

/*------View-Angles-------*/
float heading = 0.0f;
float pitch = 0.0f;
float perspective = 45.0f;
float aspectRatio = 1024.0f/768.0f;	

/*------Frame-Data--------*/
float FPS = 0.0f;
float fpsUpdateTime = 0.0f;
int frameCount = 0;

/*------Velocities--------*/
float camSpeed = 0.05f;
float addCrouch = 0.0f;
float crouchFactor = 1.0f;
float jumpVelocity = 300.0f;
float initialJumpHeight = 0.0f;

/*------Texture-Data------*/
GLUquadricObj* quadric;
int centerX, centerY, texture, face, grass;
void* font = GLUT_BITMAP_HELVETICA_18;
bool loadHiRes, wireFrame = FALSE;

/*------Terrain-Data------*/
float* mapHeights0;
float* mapHeights1;
float* mapHeights2;
float* mapHeights3;

Terrain terrain(imageSize,imageSize);
Terrain terrain1(imageSize,imageSize);
Terrain terrain2(imageSize,imageSize);
Terrain terrain3(imageSize,imageSize);

int xt = 0; int zt = 0; int index = 0;

/*--Camera's-Look-Vector---*/
/*--Camera's-Position-----*/
Vec3 camLook(0.0f,0.0f,-1.0f);
Vec3 camPos(5005.821f, 1214.412f, 9530.075f);//camPos(1725.0f,5.0f,4308.0f);

void Clear_Keyboard_Input_Buffer()
{
	while (cin.get() != '\n');
}

void handleFunc(float dt)
{
	float vecx, vecy, vecz;

	/*--------change perspective---------*/
	if(keyStates['`'])
	{
		perspective -= 1.0f;
		if(perspective <= 1.0f)
			perspective = 180.0f;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(perspective, aspectRatio, 0.1f, 40000.0f);
		glMatrixMode(GL_MODELVIEW);
	}
	if(keyStates['1'])
	{
		perspective= 45.0f;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(perspective, aspectRatio, 0.1f, 40000.0f);
		glMatrixMode(GL_MODELVIEW);
	}
	if(keyStates['2'])
	{
		perspective += 1.0f;
		if(perspective >= 180.0f)
			perspective = 0.0f;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(perspective, aspectRatio, 0.1f, 40000.0f);
		glMatrixMode(GL_MODELVIEW);
	}

	/*---------Fly-Foward---------*/
	if(keyStates['q'] || keyStates['Q'])
	{
		camPos+=camLook*(camSpeed*13.5f*dt);
	}
	/*--------Move-Foward---------*/
	if(keyStates['w'] || keyStates['W'])
	{

		camPos.x+=camLook.x*(crouchFactor*camSpeed*5.0f*dt);
		//camPos.y+=camLook.y;	//comment out to walk forward on plane
		camPos.z+=camLook.z*(crouchFactor*camSpeed*5.0f*dt);
	}
	if(keyStates['r'] || keyStates['R'])
	{
		camPos.x+=camLook.x*(crouchFactor*camSpeed*7.0f*dt);
		//camPos.y+=camLook.y;	//comment out to walk forward on plane
		camPos.z+=camLook.z*(crouchFactor*camSpeed*7.0f*dt);
	}
	/*--------Move-Backwards------*/
	if(keyStates['s'] || keyStates['S'])
	{
		camPos.x-=camLook.x*(crouchFactor*camSpeed*5.0f*dt);
		//y-=ly*speed;  //comment out to walk backwards on plane
		camPos.z-=camLook.z*(crouchFactor*camSpeed*5.0f*dt);
	}
	/*--------Move-Left----------*/
	if(keyStates['a'] || keyStates['A'])
	{
		vecx = camLook.z;
		vecy = camLook.y;
		vecz = -camLook.x;
		camPos.x+= vecx*(crouchFactor*camSpeed*5.0f*dt);
		camPos.z+= vecz*(crouchFactor*camSpeed*5.0f*dt);
	}
	/*--------Move-Right----------*/
	if(keyStates['d'] || keyStates['D'])
	{

		vecx = camLook.z;
		vecy = camLook.y;
		vecz = -camLook.x;
		camPos.x-= vecx*(crouchFactor*camSpeed*5.0f*dt);
		camPos.z-= vecz*(crouchFactor*camSpeed*5.0f*dt);		

	}
	/*---------crouch-------------*/
	if(keyTaps['c'] || keyTaps['C'])
	{
		if(!isCrouched)
		{
			addCrouch = -40.0f;
			crouchFactor = 0.45f;
		}
		else 
		{
			addCrouch = 0.0f;
			crouchFactor = 1.0f;
		}
		isCrouched = !isCrouched;		
	}
	/*----------up/down-----------*/
	if(keyStates['x'] || keyStates['X'])
		camPos.y += (crouchFactor*camSpeed*10.0f*dt);

	if(keyStates['Z'] || keyStates['z'])
		camPos.y -= (crouchFactor*camSpeed*10.0f*dt);

	if(keyTaps['t'] || keyTaps['T'])
		wireFrame = !wireFrame;

	/*----------jump--------------*/
	if(keyStates[' '])
	{
		inJump = TRUE;
		initialJumpHeight = camPos.y;
		jumpStart = chrono::steady_clock::now();
	}
	if(inJump)
	{	
		auto now = chrono::steady_clock::now();
		float tmpTime = chrono::duration_cast<chrono::duration<float, milli>>(now-jumpStart).count();
		tmpTime = tmpTime/1000.0f;

		float tmpVelocity = jumpVelocity + GRAVITY * tmpTime;
		camPos.y = initialJumpHeight + tmpVelocity*tmpTime + ((0.5f) * (GRAVITY) * (tmpTime) * (tmpTime));

		if((xt < 255) && (zt < 255))			//0						
		{
			index = zt * imageSize + xt;
			if(camPos.y <= (mapHeights0[index] * 1500.0f - 115.0f))
			{
				camPos.y = mapHeights0[index] * 1500.0f - 115.0f;			
				inJump = FALSE;
			}						
		}
		else if(camPos.y <= -30000.0f)
		{
			Vec3 tmp(5005.821f, 2214.412f, 9530.075f);
			camPos = tmp;
		}
	}
}

void updateCamHeight()
{
	xt = static_cast<int>(camPos.x)/64;
	zt = static_cast<int>(camPos.z)/64;

	if((xt < 255) && (zt < 255))			//0						
	{																								//		v-*-*
		index = zt * imageSize + xt;									
		camPos.y = mapHeights0[index] * 1500.0f - 115.0f;			
	}
}
bool floorDistance()
{
	zt = static_cast<int>(camPos.z)/64;
	xt = static_cast<int>(camPos.x)/64; 

	if((xt < 255) && (zt < 255))			//0						
	{																								//		v-*-*
		index = zt * imageSize + xt;	
		if(camPos.y > (mapHeights0[index] * 1500.0f - 25.0f))
			return FALSE;
		else if(camPos.y < (mapHeights0[index] * 1500.0f - 300.0f))
			return FALSE;
		else
			return TRUE;											
	}
}
void renderScene(void)
{
	currentTime = chrono::steady_clock::now();
	float dt = chrono::duration_cast<chrono::duration<float, milli>>(currentTime-lastTime).count();
	//cout << "dt: " << dt << endl;
    lastTime = currentTime;

    fpsUpdateTime += dt;
	

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(camPos.x >= 0 && camPos.z >= 0)
	{
		if(floorDistance()) 
			updateCamHeight();
	}

	handleFunc(dt);

	// Reset transformations

	glLoadIdentity();

/*----star background-------*/
/*		glPushMatrix();
			gluLookAt(0,0,0,camLook.x,camLook.y,camLook.z,0,1,0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
			glRotatef(90,0,1,0);
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			gluSphere(quadric, 12000.0f, 180.0f, 180.0f);
			glDisable(GL_TEXTURE_2D);
		glPopMatrix();*/

	glLoadIdentity();
	gluLookAt(camPos.x,camPos.y+addCrouch,camPos.z,camPos.x+camLook.x,camPos.y+addCrouch+camLook.y,camPos.z+camLook.z,0,1,0);

	/*-----Clockwise-------*/
	terrain.draw(wireFrame);
	terrain.draw2(wireFrame);

	glTranslatef(16320.0f,0.0f,0.0f);
	terrain1.draw(wireFrame);
	terrain1.draw2(wireFrame);

	glTranslatef(0.0f,0.0f,16320.0f);
	terrain2.draw(wireFrame);
	terrain2.draw2(wireFrame);

	glTranslatef(-16320.0f,0.0f,0.0f);
	terrain3.draw(wireFrame);
	terrain3.draw2(wireFrame);


	memset(keyTaps,0,256*sizeof(bool));

	if (fpsUpdateTime > 100.0f)
	{
		FPS = static_cast<float>(frameCount) / fpsUpdateTime * 1000.0f;
		frameCount = 0;
		fpsUpdateTime = 0.0f;
	}

    printScreenText();
    frameCount++;

	glutSwapBuffers();
}

int main(int argc, char **argv)
{

	if(argc == 1)
	{
		loadHiRes = FALSE;
		cout <<"> Loading Lo-Res textures ..." << endl;
	}
	else
	{
		if(argv[1][0] == '0')
		{
			cout <<"> Loading Lo-Res textures ..." << endl;
			loadHiRes = FALSE;
		}
		else if(argv[1][0] == '1')
		{
			cout <<"> Loading Hi-Res textures ..." << endl;
			loadHiRes = TRUE;
		}
		else 
		{
			cout <<"> Invalid arguements, loading Lo-Res textures ..." << endl;
			loadHiRes = FALSE;
		}

	}

	// init GLUT and create window
	GLinitialize(argc, argv);
	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);

	terrain.loadFile("./heightMaps/vertical1.raw", imageSize);
	terrain1.loadFile("./heightMaps/horizontal1.raw", imageSize);
	terrain2.loadFile("./heightMaps/vertical2.raw", imageSize);
	terrain3.loadFile("./heightMaps/horizontal2.raw", imageSize);


	mapHeights0 = new float[imageSize*imageSize];
	mapHeights0 = terrain.returnHeights();
	mapHeights1 = new float[imageSize*imageSize];
	mapHeights1 = terrain1.returnHeights();
	mapHeights2 = new float[imageSize*imageSize];
	mapHeights2 = terrain2.returnHeights();
	mapHeights3 = new float[imageSize*imageSize];
	mapHeights3 = terrain3.returnHeights();

//	texture = SOIL_load_OGL_texture("./textures/face.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

	lastTime = chrono::steady_clock::now();
	glutMainLoop();
	glutSetCursor(GLUT_CURSOR_INHERIT);

	return 1;
}