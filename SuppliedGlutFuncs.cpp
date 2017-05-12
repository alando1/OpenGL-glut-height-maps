#include "SuppliedGlutFuncs.h"


extern float pitch, heading, boost, aspectRatio, perspective;
extern int centerX, centerY;
extern bool keyStates[256], keyTaps[256], inJump;
extern void* font;
extern float FPS;
extern Vec3 camPos, camLook;

float x= camPos.x;
float y= camPos.y; 
float z= camPos.z;
float lx= camLook.x;
float ly= camLook.y;
float lz= camLook.z;

void GLinitialize(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1024,768);
	glutCreateWindow("\t\t\t\t\t\tTEST MODE");
	glutFullScreen();

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(pressNormalKeys);
	glutKeyboardUpFunc(releaseNormalKeys);
	glutSpecialFunc(pressSpecialKeys);
	glutSpecialUpFunc(releaseSpecialKeys);

	// here are the two new functions
	glutMouseFunc(mouseButton);
	glutPassiveMotionFunc(mouseMove);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	memset(keyStates, 0, 256*sizeof(bool));
	memset(keyTaps, 0, 256*sizeof(bool));

	// enter GLUT event processing cycle
	glutSetCursor(GLUT_CURSOR_NONE);
	centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
}

void mouseMove(int x, int y)
{
	if (!(x == centerX && y == centerY))
	{
		int dx = x - centerX;
		int dy = y - centerY;
	
		heading -= static_cast<float>(dx)*0.2f;
		pitch -= static_cast<float>(dy)*0.5f;

		Vec3 newlook(0,0,-1);
		Vec3 newpos;

		// rotate around the X axis by 'pitch' degrees
		newpos.x = newlook.x;
		newpos.y = newlook.y * cosf(pitch * PI_OVER_180) - newlook.z * sinf(pitch * PI_OVER_180);
		newpos.z = newlook.y * sinf(pitch * PI_OVER_180) + newlook.z * cosf(pitch * PI_OVER_180);
		
		// rotate resulting vector around Y axis by 'heading' degrees,
		// then store into camLook.x,Y,Z
		camLook.x = newpos.x * cos(heading * PI_OVER_180) + newpos.z * sin(heading * PI_OVER_180);
		camLook.y = newpos.y;
		camLook.z = -newpos.x * sin(heading * PI_OVER_180) + newpos.z * cos(heading * PI_OVER_180);
		glutWarpPointer(centerX, centerY);

	}
}

void mouseButton(int button, int state, int x, int y) 
{}

void pressNormalKeys(unsigned char key, int xx, int yy) 
{
	if(key == 27)
	{	cout<< "> ESC:\tExiting Program." << endl;
		exit(EXIT_FAILURE);
	}
	keyStates[key]=true;
	keyTaps[key]=true;
}

void releaseNormalKeys(unsigned char key, int xx, int yy)
{
/*	if(key == keyStates['q'] || key == keyStates['Q'])
		inJump = TRUE;*/
	keyStates[key]=false;
	keyTaps[key]=false;
}

void pressSpecialKeys(int key, int xx, int yy) 
{
		if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
	{

	}
} 

void releaseSpecialKeys(int key, int x, int y) 
{
	if (!(glutGetModifiers() & GLUT_ACTIVE_SHIFT))
	{

	}
} 

void renderBitmapString(float d, float e, float f, void *font, char *string)
{
    char *c;
    glRasterPos3f(d,e,f);
 
    for (c = string; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}

void renderText2D(float a, float b, void* font, char* string)
{
    glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
            glPushMatrix();
                glLoadIdentity();
                glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, 1.0, 5000.0);
                renderBitmapString(a, b, -1.0f, font, string);
            glPopMatrix();
        glMatrixMode(GL_MODELVIEW);    
    glPopMatrix();
}

void printScreenText()
{
	char msg[80];
	char msg2[80];
	char msg3[80];
	char msg4[80];
    glColor3f(0, 0.6f, 0);
 
    sprintf(msg, "cam pos: %.3f, %.3f, %.3f", camPos.x, camPos.y, camPos.z);
    renderText2D(5.0f, 20.0f, font, msg);

    sprintf(msg2, "FOV: %.1f", perspective);
    renderText2D(5.0f, 40.0f, font, msg2);

    sprintf(msg3, "look vector: %.3f, %.3f, %.3f", camLook.x, camLook.y, camLook.z);
    renderText2D(5.0f, 60.0f, font, msg3);

    sprintf(msg4, "FPS: %.3f", FPS);
    renderText2D(5.0f, 80.0f, font, msg4);
}

void changeSize(int w, int h) 
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio =  w * 1.0 / h;
	aspectRatio = ratio;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(perspective, aspectRatio, 0.1f, 40000.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}
