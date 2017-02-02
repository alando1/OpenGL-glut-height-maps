#include "Terrain.h"

Terrain::Terrain(float len, float wid) :
	length(len), width(wid), tileSize(16), heightRatio(1500.0f), scalar(40.0f)
{
	heights = new float[width*length];
}

Terrain::~Terrain()
{
	delete[] heights;
}
void Terrain::loadFile(const char* fileName, int nsize)
{
	nSize = nsize;
	FILE *pFile = fopen(fileName, "rb");
	//cout << ">Error #1: " << strerror(errno) << endl;
	if(pFile == NULL)
	{
		cout << ">Error, could not open " << fileName << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		unsigned char* buffer = new unsigned char[nSize*nSize];
		int readIsGood = fread(buffer, 1, nSize*nSize, pFile);
		if(readIsGood != 0)
		{
			/*cout << ">fread worked properly." << endl;*/
			buildHeightMap(buffer);
		}
		else
		{
			cout << ">Error, could not read " << fileName << endl;
			cout << ">Error #2: " << strerror(errno) << endl;
			exit(EXIT_FAILURE);
		}


		delete[] buffer;
	}
	fclose(pFile);
}	
void Terrain::draw(bool wireFrame)
{
	Vec3 tmpVec(0,0,0);

	if(wireFrame)
		glPolygonMode(GL_FRONT, GL_LINE);
	else
		glPolygonMode(GL_FRONT,GL_FILL);

	glBegin(GL_QUADS);

	for(int i=0; i < (nSize-1); i++)
	{
		for(int j=0; j < (nSize-1); j++)
		{
			setVertexColor(i, j);
			setVertexColor(i, j + 1);
			setVertexColor(i + 1, j + 1);
			setVertexColor(i + 1, j);
		}
	}
	glEnd();
}

void Terrain::setVertexColor(int x, int z)
{

	int index = z * nSize + x;
	float Y = heights[index];
	float X = tileSize * x;
	float Z = tileSize * z;

	if(Y<0.1f)												
		glColor4f(Y*0.5f, 0.7f, 0.5f, 1.0f);				//water layer
	else if(Y>0.1f && Y<0.3f)								
		glColor4f(Y*0.5f, 0.156f+Y*0.2f, Y*0.3f, 1.0f);		//green layer
	else if(Y>0.50f && Y<0.54f)								
		glColor4f(Y-0.2f, Y-0.2f, 0.1f, 1.0f);				//gold rock layer
	else if(Y>0.65f && Y<=0.8f)																	
		glColor4f(Y*0.371f, Y/4.67f, Y*(0.1f-Y), 1.0f);		//red rock blend color
	else if(Y>0.8f)											
		glColor4f(Y-0.1f, Y-0.2f, Y-0.3f, 1.0f);			//snow layer
	else
		glColor4f(Y*0.50f, 0.156f, 0.0f, 1.0f);
															//glColor3f(Y, Y*0.7f, Y*0.2f);//Sand Mountain
	glVertex3f(4*X,((heightRatio*Y)-190.0f),4*Z);
	//remove 4's for 1024 by 1024 ratio images
}

void Terrain::draw2(bool wireFrame)
{
	Vec3 tmpVec(0,0,0);

	if(wireFrame)
	{
		glPolygonMode(GL_FRONT,GL_FILL);

		glBegin(GL_QUADS);

		for(int i=0; i < (nSize-1); i++)
		{
			for(int j=0; j < (nSize-1); j++)
			{
				setVertexColor2(i, j);
				setVertexColor2(i, j + 1);
				setVertexColor2(i + 1, j + 1);
				setVertexColor2(i + 1, j);
			}
		}
		glEnd();
	}
}

void Terrain::setVertexColor2(int x, int z)
{

	int index = z * nSize + x;
	float Y = heights[index];
	float X = tileSize * x;
	float Z = tileSize * z;

	glColor4f(0,0,0,0.5f);//Sand Mountain
	glVertex3f(4*X,((heightRatio*Y)-190.0f),4*Z);
}


void Terrain::buildHeightMap(unsigned char* buffa)
{
	cout << "> Building Height Map ..." << endl;
	auto start = chrono::steady_clock::now();

	for(int i=0; i < (nSize*nSize); i++)
	{
		heights[i] = static_cast<float>(buffa[i])/255.0f;
	}
	auto finish = chrono::steady_clock::now();
	float dt = chrono::duration_cast<chrono::duration<float, milli>>(finish-start).count();

	cout << "> Height Map built in " << dt << " milliseconds." << endl;
}

float* Terrain::returnHeights()
{
	return heights;
}
