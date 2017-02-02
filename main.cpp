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

#define GRAVITY -9.810f
#define imageSize 1024

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
float crouchFactor = 1.0f;
float jumpVelocity = 100.0f;
float initialJumpHeight = 0.0f;

/*------Texture-Data------*/
GLUquadricObj* quadric;
int centerX, centerY, texture, face, grass;
void* font = GLUT_BITMAP_HELVETICA_18;
bool loadHiRes, wireFrame = FALSE;

/*------Terrain-Data------*/
float* mapHeights;
Terrain terrain(imageSize,imageSize);
int xt = 0; int zt = 0; int index = 0;

/*--Camera's-Look-Vector---*/
/*--Camera's-Position-----*/
Vec3 camLook(0.0f,0.0f,-1.0f);
Vec3 camPos(1725.0f,5.0f,4308.0f);

void Clear_Keyboard_Input_Buffer()
{
	while (cin.get() != '\n');
}

void handleFunc(float dt)
{
	float vecx, vecy, vecz;

	/*---------Fly-Foward---------*/
	if(keyStates['q'] || keyStates['Q'])
	{
		camPos+=camLook*(camSpeed*13.5f*dt);
	}
	/*--------Move-Foward---------*/
	if(keyStates['w'] || keyStates['W'])
	{

		camPos.x+=camLook.x*(crouchFactor*camSpeed*dt);
		//camPos.y+=camLook.y;	//comment out to walk forward on plane
		camPos.z+=camLook.z*(crouchFactor*camSpeed*dt);
	}
	if(keyStates['r'] || keyStates['R'])
	{
		camPos.x+=camLook.x*(crouchFactor*camSpeed*3.0f*dt);
		//camPos.y+=camLook.y;	//comment out to walk forward on plane
		camPos.z+=camLook.z*(crouchFactor*camSpeed*3.0f*dt);
	}
	/*--------Move-Backwards------*/
	if(keyStates['s'] || keyStates['S'])
	{
		camPos.x-=camLook.x*(crouchFactor*camSpeed*dt);
		//y-=ly*speed;  //comment out to walk backwards on plane
		camPos.z-=camLook.z*(crouchFactor*camSpeed*dt);
	}
	/*--------Move-Left----------*/
	if(keyStates['a'] || keyStates['A'])
	{
		vecx = camLook.z;
		vecy = camLook.y;
		vecz = -camLook.x;
		camPos.x+= vecx*(crouchFactor*camSpeed*dt);
		camPos.z+= vecz*(crouchFactor*camSpeed*dt);
	}
	/*--------Move-Right----------*/
	if(keyStates['d'] || keyStates['D'])
	{

		vecx = camLook.z;
		vecy = camLook.y;
		vecz = -camLook.x;
		camPos.x-= vecx*(crouchFactor*camSpeed*dt);
		camPos.z-= vecz*(crouchFactor*camSpeed*dt);		

	}
	/*---------crouch-------------*/
	if(keyTaps['c'] || keyTaps['C'])
	{
		if(!isCrouched)
		{
			camPos.y-=1.75f;
			crouchFactor = 0.45f;
		}
		else 
		{
			camPos.y+=1.75f;
			crouchFactor = 1.0f;
		}
		isCrouched = !isCrouched;

		
	}
	if(keyStates['x'] || keyStates['X'])
		camPos.y+=100.0f;

	if(keyStates['Z'] || keyStates['z'])
		camPos.y-=100.0f;
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


		if(camPos.y <= mapHeights[index] * 1000.0f * 1.5f - 170.0f)//if(camPos.y <= 5.0f)
		{
			camPos.y = mapHeights[index] * 1000.0f * 1.5f - 170.0f;
			inJump = FALSE;
		}
	}
	if(keyTaps['t'] || keyTaps['T'])
	{
		wireFrame = !wireFrame;
	}
}

void updateCamHeight()
{
	zt = static_cast<int>(camPos.z)/16;
	xt = static_cast<int>(camPos.x)/16; 
	index = zt * imageSize + xt;
	camPos.y = mapHeights[index] * 1000.0f * 1.5f - 170.0f;
}
bool floorDistance()
{
	zt = static_cast<int>(camPos.z)/16;
	xt = static_cast<int>(camPos.x)/16; 
	index = zt * imageSize + xt;
	if(camPos.y > (mapHeights[index] * 1000.0f * 1.5f - 150.0f))
		return TRUE;
	else
		return FALSE;
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
		if(!floorDistance()) 
			updateCamHeight();
	}

	handleFunc(dt);

	// Reset transformations

	glLoadIdentity();

	glPushMatrix();
	glTranslatef(0,1000,0);	//test plane in view
	for(int i=0; i<16; i++)
	{
		for(int j=0; j<16; j++)
		{
			texturePlane(grass, 1000.0f,1000.0f);
			glTranslatef(1000,0,0);
		}
		glTranslatef(-10000,0,1000);
	}
	glPopMatrix();
	

	if(wireFrame)
	{
		glPushMatrix();
			gluLookAt(0,0,0,camLook.x,camLook.y,camLook.z,0,1,0);
			glEnable(GL_TEXTURE_2D);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBindTexture(GL_TEXTURE_2D, texture);
			glRotatef(90,0,1,0);
			glColor4f(1.0f,1.0f,1.0f,0.40f);
			gluSphere(quadric, 12000, 180, 180);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
		glPopMatrix();
	}



	glLoadIdentity();
	gluLookAt(camPos.x,camPos.y,camPos.z,camPos.x+camLook.x,camPos.y+camLook.y,camPos.z+camLook.z,0,1,0);
	terrain.draw(wireFrame);

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

	terrain.loadFile("./heightMaps/8bitHeightMap.raw", imageSize);
	mapHeights = new float[imageSize*imageSize];
	mapHeights = terrain.returnHeights();

	if(loadHiRes)
	{
		grass = SOIL_load_OGL_texture("./textures/Seamless_Grass.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		texture = SOIL_load_OGL_texture("./textures/bigStars.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		
	}
	else
	{
		grass = SOIL_load_OGL_texture("./textures/gridsphere.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		texture = grass;
	}
		face = SOIL_load_OGL_texture("./textures/showMeWhatYouGot.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);


	lastTime = chrono::steady_clock::now();
	glutMainLoop();
	glutSetCursor(GLUT_CURSOR_INHERIT);

	return 1;
}



