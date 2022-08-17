#define _CRT_SECURE_NO_WARNINGS
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include <process.h>
#include "LinearAlgebra.h"
#include "HDTJMType.h"


#define W_H 3000
#define W_W 3000
#define VOXEL_SIZE 24

int SizeH = 1000;
int SizeW = 1000;

typedef struct {
	GLfloat Voxel[24][5];
}VoxelModel;

size_t FrameCount;
int bim = 0;
int View = 1;
int angleX = 0;
int angleZ = 0;
int angleY = 0;
VoxelModel* modelBuf;
voxelCount = 0;
volatile float ModelY = 1;
GLfloat R = 0.7;
GLfloat camx = 0, camy = 0, camz = 1;
GLfloat hrzndegree = 0, vrtcldegree = 0;



void init_light(void);
void keyboard(unsigned char key, int x, int y);
void Display(void);
void Reshape(int width, int height);
GLubyte* LoadBmp(const char* imagePath, int* width, int* height);
void InitTexture(unsigned char* data, int width, int height);
void Modeling(void);
void CreateVoxel(Vector3f_t* vertex, Voxel_t* outVoxel);
void ModelLoad(Voxel_t* VoxelArr, int VexelCount, VoxelModel* modelBuf);

void Draw(void)
{
	glutPostRedisplay();      //�����츦 �ٽ� �׸����� ��û
	glutTimerFunc(8, Draw, 0); //���� Ÿ�̸� �̺�Ʈ�� 30�и�����Ʈ ��  ȣ���.
}


void ThreadMove(void)
{
	while (1)
	{
		Sleep(1000);
		ModelY -= 0.1F;
	}

}

void FrameRating(void)
{
	while (1)
	{
		Sleep(1000);
		printf("Fps:%d\n", FrameCount);
		FrameCount = 0;
	}

}


int main(int argc, char** argv)
{
	// IO �ܰ�� �ű� �ֵ�---------------
	/*
	�̺κ��� �ѹ��� �ص� �Ǽ� ���� ������ �ȵ�
	*/
	voxelCount = 4;
	Vector3f_t vertex[4][8] = {
		{
		{0.00F ,-0.10F ,0.00F},
		{0.00F ,-0.10F ,0.10F},
		{0.00F , 0.00F ,0.10F},
		{0.00F , 0.00F ,0.00F},
		{0.10F ,-0.10F ,0.00F},
		{0.10F , 0.00F ,0.00F},
		{0.10F , 0.00F ,0.10F},
		{0.10F ,-0.10F ,0.10F},
		},
		{
		{0.00F ,-0.20F ,0.10F},
		{0.00F ,-0.20F ,0.20F},
		{0.00F ,-0.10F ,0.20F},
		{0.00F ,-0.10F ,0.10F},
		{0.10F ,-0.20F ,0.10F},
		{0.10F ,-0.10F ,0.10F},
		{0.10F ,-0.10F ,0.20F},
		{0.10F ,-0.20F ,0.20F},
		},
		{
		{0.00F ,-0.10F ,0.10F},
		{0.00F ,-0.10F ,0.20F},
		{0.00F ,0.00F ,0.20F},
		{0.00F ,0.00F ,0.10F},
		{0.10F ,-0.10F ,0.10F},
		{0.10F ,0.00F ,0.10F},
		{0.10F ,0.00F ,0.20F},
		{0.10F ,-0.10F ,0.20F},
		},
		{
		{0.00F ,0.00F ,0.10F},
		{0.00F ,0.00F ,0.20F},
		{0.00F ,0.10F ,0.20F},
		{0.00F ,0.10F ,0.10F},
		{0.10F ,0.00F ,0.10F},
		{0.10F ,0.10F ,0.10F},
		{0.10F ,0.10F ,0.20F},
		{0.10F ,0.00F ,0.20F}
		}
	};  // 
	// �ʱ� IO ���� PLY �ҷ����� �� ��

	Voxel_t* VoxelArr = malloc(sizeof(Voxel_t) * voxelCount);

	for (int i = 0; i < voxelCount; i++)
	{// ���� ����
		CreateVoxel(vertex[i], VoxelArr + i); // ���� ���� ����
	}
	modelBuf = malloc(sizeof(VoxelModel) * voxelCount);// ȭ�� ��� ���� ����'
	ModelLoad(VoxelArr, voxelCount, modelBuf); // ȭ�� ����� ���� ä���

	//-----------------------------------modelBuf�� ������ ����
	

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE );

	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.0, 0.0, 0.0);//rgba (a: ����)

	glutCreateWindow("Lighting");
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	init_light();// window ������ �� �ʱ�ȭ
	

	// ��������  
	//glEnable(GL_CULL_FACE);

	int imageWidht, imageHeight;
	glEnable(GL_TEXTURE_2D);
	GLubyte* Texture = LoadBmp("woodBox.bmp", &imageWidht, &imageHeight);
	InitTexture(Texture, imageWidht, imageHeight);
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(0, Draw, 0);
	
	_beginthread(ThreadMove,0,(void*)0);
	_beginthread(FrameRating, 0, (void*)0);


	while (1) 
	{	
		glutMainLoopEvent();
	}
	
	_endthread();

	free(VoxelArr);
	free(modelBuf);
	return 0;
}

GLubyte* LoadBmp(const char* imagePath, int* width, int* height)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char* data;

	FILE* file = fopen(imagePath, "rb");
	if (!file) { printf("Image could not be opened\n"); return 0; }


	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return 0;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	// ����Ʈ �迭���� int ������ �н��ϴ�. 
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	*width = *(int*)&(header[0x12]);
	*height = *(int*)&(header[0x16]);


	if (imageSize == 0)    imageSize = *width * *height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	data = (GLubyte*)malloc(sizeof(GLubyte) * imageSize);

	// ���Ͽ��� ���۷� ���� ������ �ֱ�. 
	fread(data, 1, imageSize, file);

	//���� ��� �޸� �ȿ� �����ϱ�, ������ �ݽ��ϴ�. 
	//Everything is in memory now, the file can be closed
	fclose(file);
	return data;
}

void InitTexture(unsigned char* data, int width, int height)
{
	GLuint tex_id = 1;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glEnable(GL_TEXTURE_2D);
}

void Modeling(void)
{
	//IO�� �� �ڵ� ������ ���� �ڵ常 ����
	glInterleavedArrays(GL_T2F_V3F, 0, modelBuf);// opengl �� ���� ����
	glDrawArrays(GL_QUADS, 0, VOXEL_SIZE * voxelCount); // ���

}

void ModelLoad(Voxel_t* VoxelArr, int VexelCount, VoxelModel* modelBuf)
{
	GLuint face[VOXEL_SIZE] = {
		6,2,0,4,
		3,7,5,1,
		2,6,7,3,
		1,5,4,0,
		7,6,4,5,
		2,3,1,0
	};

	GLfloat texCoord[4][2] = {
		{0.F , 0.F},
		{1.F , 0.F},
		{1.F , 1.F},
		{0.F , 1.F}
	};




	for (int Vidx = 0; Vidx < VexelCount; Vidx++)
	{
		for (int idx = 0; idx < VOXEL_SIZE; idx++)
		{
			//Texture
			modelBuf[Vidx].Voxel[idx][0] = texCoord[idx % 4][0];//w
			modelBuf[Vidx].Voxel[idx][1] = texCoord[idx % 4][1];//h
			//Vertex
			modelBuf[Vidx].Voxel[idx][2] = VoxelArr[Vidx].Vertex[face[idx]].X;
			modelBuf[Vidx].Voxel[idx][3] = VoxelArr[Vidx].Vertex[face[idx]].Y;
			modelBuf[Vidx].Voxel[idx][4] = VoxelArr[Vidx].Vertex[face[idx]].Z;
		}


	}



}

void CreateVoxel(Vector3f_t* vertex, Voxel_t* outVoxel)
{
	Vector3f_t tempVert[2];
	tempVert[1].X = tempVert[0].X = vertex[0].X;
	tempVert[1].Y = tempVert[0].Y = vertex[0].Y;
	tempVert[1].Z = tempVert[0].Z = vertex[0].Z;

	for (int idx = 0; idx < 8; idx++)
	{
		if (vertex[idx].X > tempVert[1].X)tempVert[1].X = vertex[idx].X;
		else if (vertex[idx].X < tempVert[0].X)tempVert[0].X = vertex[idx].X;

		if (vertex[idx].Y > tempVert[1].Y)tempVert[1].Y = vertex[idx].Y;
		else if (vertex[idx].Y < tempVert[0].Y)tempVert[0].Y = vertex[idx].Y;

		if (vertex[idx].Z > tempVert[1].Z)tempVert[1].Z = vertex[idx].Z;
		else if (vertex[idx].Z < tempVert[0].Z)tempVert[0].Z = vertex[idx].Z;
	}


	int idx_x = 0;
	int idx_y = 0;
	for (int idx = 0; idx < 8; idx++)
	{
		outVoxel->Vertex[idx].X = tempVert[idx_x].X;
		if ((idx + 1) % 4 == 0) idx_x = 1;

		outVoxel->Vertex[idx].Y = tempVert[idx_y].Y;
		if ((idx + 1) % 2 == 0) idx_y = (idx_y + 1) % 2;

		outVoxel->Vertex[idx].Z = tempVert[idx % 2].Z;
	}
}


void Display(void)
{
	//_asm int 3;
	if (ModelY < -1) ModelY = 1;
	FrameCount++;
	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	camy = R * sin(vrtcldegree);
	camx = pow(R * R + camy * camy, (double)0.5) * cos(hrzndegree);
	camz = pow(R * R + camy * camy, (double)0.5) * sin(hrzndegree);
	gluLookAt(camx, camy, camz, 0, 0, 0, 0, 1, 0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPushMatrix();
	glRotatef(angleY, 0, 1, 0);
	glRotatef(angleZ, 0, 0, 1);
	glRotatef(angleX, 1, 0, 0);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0,ModelY,0);
	
	Modeling();// �׸��� ��
	glPopMatrix();
	
	glFlush();
	
	//glutSwapBuffers();
}
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	GLfloat factor_w = (GLfloat)width / (GLfloat)SizeW;
	GLfloat factor_h = (GLfloat)height / (GLfloat)SizeH;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1 * factor_w, 1 * factor_w, -1 * factor_h, 1 * factor_h, -10, 10);

}
void init_light(void)
{
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	GLfloat light_global_ambient[] = { 0.3F,0.3F,0.3F,1.0F };
	GLfloat light_0_pos[] = { 2.0F , 2.0F , -2.0F , 1.0F };

	GLfloat light_0_ambient[] = { 1.0F , 1.0F , 1.0F , 1.0F };
	GLfloat light_0_diffuse[] = { 1.0F , 1.0F , 1.0F , 1.0F };
	GLfloat light_0_specular[] = { 1.0F ,1.0F , 1.0F, 1.0F };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_global_ambient);
	glEnable(GL_LIGHT0);


	glLightfv(GL_LIGHT0, GL_AMBIENT, light_0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_0_pos);




	glDisable(GL_COLOR_MATERIAL);
	GLfloat matrial_0_ambient[] = { 0.3F,0.3F,0.3F,1.0F };
	GLfloat matrial_0_diffuse[] = { 1.0F,1.0F,1.0F,1.0F }; // ��ü�� ������ ���� ���������� ����
	GLfloat matrial_0_specular[] = { 1.0F , 1.0F ,1.0F ,1.0F };
	GLfloat matrial_0_shininess[] = { 25.0F };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matrial_0_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matrial_0_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matrial_0_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matrial_0_shininess);

	//��ü ���� ���� & ����



}
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '8':
		if (vrtcldegree < 1)
			vrtcldegree += 0.1;
		break;
	case '2':
		if (vrtcldegree > -1)
			vrtcldegree -= 0.1;
		break;
	case '4':
		hrzndegree += 0.1;
		break;
	case '6':
		hrzndegree -= 0.1;
		break;
	case 'x':
		angleX += 90;
		break;
	case 'y':
		angleY += 90;
		break;
	case 'z':
		angleZ += 90;
		break;
	case '+':
		SizeH += 100;
		SizeW += 100;
		break;
	case '-':
		SizeH -= 100;
		SizeW -= 100;
		break;
	}

}
