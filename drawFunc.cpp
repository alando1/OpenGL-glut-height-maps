#include "drawFunc.h"
extern Vec3 campos, camlook;

void drawgrid()
{
	glBegin(GL_LINES);
	float xx;
	for (int a = -200; a < 200; a++)
		{	
			xx= (float)(a+200)/(400.0f);
			glColor3f(1.0f, xx/2, a);
			glVertex3f(-200, 0, a);
			glVertex3f(200, 0, a);

			glColor3f(1.0f, xx/2, a);
			glVertex3f(a, 0, -200);
			glVertex3f(a, 0, 200);

			glColor3f(a, 1.0f, xx/2);
			glVertex3f(a, -200, 0);
			glVertex3f(a, 200, 0);

			glColor3f(a, 1.0f, xx/2);
			glVertex3f(-200, a, 0);
			glVertex3f(200, a, 0);

			glColor3f(xx/2, a, 1.0f);
			glVertex3f(0, a, -200.0f);
			glVertex3f(0, a, 200.0f);

			glColor3f(xx/2, a, 1.0f);
			glVertex3f(0, -200, a);
			glVertex3f(0, 200, a);
		}
	glEnd();
}

void otherGrid()
{
	glBegin(GL_LINES);
	glColor3f(0.250f, 0.325f, 0.75f);
	float w;
	for (int a = -2000; a < -1000; a++)
		{	
			w= (float)(a+2000)/1000.0f;
			glColor3f(1.0f, w/2, a);
			glVertex3f(-2000, -2000, a);
			glVertex3f(-1000, -2000, a);

			glColor3f(1.0f, w/2, a);
			glVertex3f(a, -2000, -2000);
			glVertex3f(a, -2000, -1000);

			glColor3f(a, 1.0f, w/2);
			glVertex3f(a, -2000, -2000);
			glVertex3f(a, -1000, -2000);

			glColor3f(a, 1.0f, w/2);
			glVertex3f(-2000, a, -2000);
			glVertex3f(-1000, a, -2000);

			glColor3f(w/2, a, 1.0f);
			glVertex3f(-2000, a, -2000.0f);
			glVertex3f(-2000, a, -1000.0f);

			glColor3f(w/2, a, 1.0f);
			glVertex3f(-2000, -2000, a);
			glVertex3f(-2000, -1000, a);
		}
	glEnd();
}

void drawFloor()
{
	glBegin(GL_QUADS);
		glVertex3f(-200.0f, 0.0f, -200.0f);
		glVertex3f(-200.0f, 0.0f,  200.0f);
		glVertex3f( 200.0f, 0.0f,  200.0f);
		glVertex3f( 200.0f, 0.0f, -200.0f);
	glEnd();
}

void texturePlane(int testTexture, float length, float width)
{
	length = length/2.0f;
	width = width/2.0f;

	/*-------test texturing----*/
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, testTexture);
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	float scale = 10.0f;
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-length, 0.0f, width);
		glTexCoord2f(scale, 0);
		glVertex3f( length, 0.0f, width);
		glTexCoord2f(scale, scale);
		glVertex3f( length, 0.0f, -width);
		glTexCoord2f(0, scale);
		glVertex3f(-length, 0.0f, -width);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

/*
void setVSync(int interval=1)
{

	typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

  const GLubyte* extensions= glGetString(GL_EXTENSIONS);
  cout << endl << endl << "Printing extensions:" <<endl << endl <<extensions << endl << endl;

  if( strstr( reinterpret_cast<const char*>(extensions), "WGL_EXT_swap_control" ) == 0 )
    {cout << "\n>Vsync not supported.\n";
    return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n";
	}
  else
  {
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

    if( wglSwapIntervalEXT ){
      wglSwapIntervalEXT(interval);
      cout << "\n>Vsync set.\n";
    }
  }
}
*/
