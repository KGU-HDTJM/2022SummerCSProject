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
#define VOXEL_SIZE_QUADS 24
#define VOXEL_SIZE_TRIANGLES 36

#define SIZE_Y 6
#define SIZE_Z 5
#define SIZE_X 5
#define SIZE_MAX SIZE_Y * SIZE_Z * SIZE_X

const int sizeH = 1000;
const int sizeW = 1000;

size_t FrameCount;
int bim = 0;
int View = 1;
int angleX = 0;
int angleZ = 0;
int angleY = 0;

voxelCount = 0;
volatile float ModelY = 1;

typedef struct {
	GLfloat Voxel[36][3];
}VoxelModel;


GLfloat R = 0.7;
GLfloat camx = 0, camy = 0, camz = 1;
GLfloat hrzndegree = 0, vrtcldegree = 0;



void Init_Light(void);
void Keyboard(unsigned char key, int x, int y);
void Display(void);
void Reshape(int width, int height);
GLubyte* LoadBmp(const char* imagePath, int* width, int* height);
void InitTexture(unsigned char* data, int width, int height);
int GetVoxelCount(int* arr, int sizeMax);

void ReVoxelSize(Vector3f_t* tempVoxel, GLfloat size);
void VoxelTrans(Vector3f_t* stdVoxel, Vector3f_t* tempVoxel, GLfloat x, GLfloat y, GLfloat z, GLfloat voxelSize);
void CreateVoxelMap(int sizeX, int sizeY, int sizeZ, int* dataBuf, float voxelSize, Voxel_t* voxelArr, Voxel_t* gridArr);
void CreateMapBuffer(Voxel_t* VoxelArr, Voxel_t* gridArr, int voxelCount, VoxelModel* modelBuf, VoxelModel* gridBuf, int gridSize);
void DrawMap(void);


void DrawFrame(void)
{
	glutPostRedisplay();
	glutTimerFunc(1, DrawFrame, 0);
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


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);

	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.0, 0.0, 0.0);//rgba (a: 투명도)

	glutCreateWindow("Lighting");
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	Init_Light();// window 생성후 빛 초기화


	// 은면제거  
	//glEnable(GL_CULL_FACE);

	//int imageWidht, imageHeight;
	glEnable(GL_TEXTURE_2D);
	//GLubyte* Texture = LoadBmp("woodBox.bmp", &imageWidht, &imageHeight);
	//InitTexture(Texture, imageWidht, imageHeight);
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(0, DrawFrame, 0);

	_beginthread(FrameRating, 0, (void*)0);


	while (1)
	{
		glutMainLoopEvent();
	}

	_endthread();


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

	// 바이트 배열에서 int 변수를 읽습니다. 
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	*width = *(int*)&(header[0x12]);
	*height = *(int*)&(header[0x16]);


	if (imageSize == 0)    imageSize = *width * *height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	data = (GLubyte*)malloc(sizeof(GLubyte) * imageSize);

	// 파일에서 버퍼로 실제 데이터 넣기. 
	fread(data, 1, imageSize, file);

	//이제 모두 메모리 안에 있으니까, 파일을 닫습니다. 
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

	//glutSolidCube(0.5);
	DrawMap();

	glFlush();

	//glutSwapBuffers();
}
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	GLfloat factor_w = (GLfloat)width / (GLfloat)sizeW;
	GLfloat factor_h = (GLfloat)height / (GLfloat)sizeH;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1 * factor_w, 1 * factor_w, -1 * factor_h, 1 * factor_h, -10, 10);

}
void Init_Light(void)
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
	GLfloat matrial_0_diffuse[] = { 1.0F,1.0F,1.0F,1.0F }; // 물체의 색상을 가장 직접적으로 결정
	GLfloat matrial_0_specular[] = { 1.0F , 1.0F ,1.0F ,1.0F };
	GLfloat matrial_0_shininess[] = { 25.0F };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matrial_0_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matrial_0_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matrial_0_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matrial_0_shininess);

	//물체 색상 정의 & 지정



}
void Keyboard(unsigned char key, int x, int y)
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
	}

}

int GetVoxelCount(int* arr, const int sizeMax)
{
	int voxelCount = 0;
	for (int i = 0; i < sizeMax; i++)
	{
		voxelCount += arr[i];
	}
	return voxelCount;
}
void ReVoxelSize(Vector3f_t* tempVoxel, GLfloat size)
{
	for (int i = 0; i < 8; i++)
	{
		tempVoxel[i].X *= size;
		tempVoxel[i].Y *= size;
		tempVoxel[i].Z *= size;
	}
}
void VoxelTrans(Vector3f_t* stdVoxel, Vector3f_t* tempVoxel, GLfloat x, GLfloat y, GLfloat z, GLfloat voxelSize)
{
	for (int i = 0; i < 8; i++)
	{
		tempVoxel[i].X = stdVoxel[i].X + x + voxelSize * 0.5;
		tempVoxel[i].Y = stdVoxel[i].Y + y;
		tempVoxel[i].Z = stdVoxel[i].Z + z + voxelSize * 0.5;
	}
}
void CreateVoxelMap(int sizeX, int sizeY, int sizeZ, int* dataBuf, float voxelSize, Voxel_t* voxelArr, Voxel_t* gridArr)
{
	Vector3f_t stdVoxel[8] = {
	{-1.F,-1.F,-1.F},
	{-1.F,-1.F, 1.F},
	{-1.F, 1.F,-1.F},
	{-1.F, 1.F, 1.F},

	{ 1.F,-1.F,-1.F},
	{ 1.F,-1.F, 1.F},
	{ 1.F, 1.F,-1.F},
	{ 1.F, 1.F, 1.F}
	};
	ReVoxelSize(stdVoxel, voxelSize * (0.5));

	int voxelArrIdx = 0;
	int gridArrIdx = 0;
	float tempX, tempY, tempZ;

	int x, y, z;
	for (y = 0; y < sizeY; y++)
	{
		for (z = 0; z < sizeZ; z++)
		{
			for (x = 0; x < sizeX; x++)
			{	

				Vector3f_t tempVoxel[8] = { 0, };
				tempX = ((-0.5F) * sizeX + x) * voxelSize;
				tempY = ((0.5F) * sizeY - y) * voxelSize;
				tempZ = ((-0.5F) * sizeZ + z) * voxelSize;
				VoxelTrans(stdVoxel, tempVoxel, tempX, tempY, tempZ, voxelSize);

				//그리드 라인 생성
				for (int i = 0; i < 8; i++)
				{
					gridArr[gridArrIdx].Vertex[i].X = tempVoxel[i].X;
					gridArr[gridArrIdx].Vertex[i].Y = tempVoxel[i].Y;
					gridArr[gridArrIdx].Vertex[i].Z = tempVoxel[i].Z;
				}
				gridArrIdx++;


				// 복쉘 채우기
				if (dataBuf[(sizeZ * sizeX) * y + sizeX * z + x] != 0)
				{
					for (int i = 0; i < 8; i++) {
						voxelArr[voxelArrIdx].Vertex[i].X = tempVoxel[i].X;
						voxelArr[voxelArrIdx].Vertex[i].Y = tempVoxel[i].Y;
						voxelArr[voxelArrIdx].Vertex[i].Z = tempVoxel[i].Z;
					}
					voxelArrIdx++;
				}



			}
		}
	}
}
void CreateMapBuffer(Voxel_t* VoxelArr ,Voxel_t* gridArr ,int voxelCount, VoxelModel* modelBuf, VoxelModel* gridBuf,int gridSize)
{
	GLuint faceQuads[VOXEL_SIZE_QUADS] = {
		6,2,0,4,
		3,7,5,1,
		2,6,7,3,
		1,5,4,0,
		7,6,4,5,
		2,3,1,0
	};

	GLuint faceTriangles[VOXEL_SIZE_TRIANGLES] = {
		6,2,0,
		0,4,6,

		3,7,5,
		5,1,3,

		2,6,7,
		7,3,2,

		1,5,4,
		4,0,1,

		7,6,4,
		4,5,7,

		2,3,1,
		1,0,2
	};



	for (int Vidx = 0; Vidx < gridSize; Vidx++)
	{
		for (int idx = 0; idx < VOXEL_SIZE_QUADS; idx++)
		{
			//gridVertex 
			gridBuf[Vidx].Voxel[idx][0] = gridArr[Vidx].Vertex[faceQuads[idx]].X;
			gridBuf[Vidx].Voxel[idx][1] = gridArr[Vidx].Vertex[faceQuads[idx]].Y;
			gridBuf[Vidx].Voxel[idx][2] = gridArr[Vidx].Vertex[faceQuads[idx]].Z;
			
		}


	}

	for (int Vidx = 0; Vidx < voxelCount; Vidx++)
	{
		for (int idx = 0; idx < VOXEL_SIZE_TRIANGLES; idx++)
		{
			//voxelVertex
			modelBuf[Vidx].Voxel[idx][0] = VoxelArr[Vidx].Vertex[faceTriangles[idx]].X;
			modelBuf[Vidx].Voxel[idx][1] = VoxelArr[Vidx].Vertex[faceTriangles[idx]].Y;
			modelBuf[Vidx].Voxel[idx][2] = VoxelArr[Vidx].Vertex[faceTriangles[idx]].Z;
		}


	}


}
void DrawMap(void)
{
	int voxelCount;


	int MapdataBuf[SIZE_Y][SIZE_Z][SIZE_X] = {// xzy
		{
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0,0,1,0,0},
			{0,0,0,0,0},
			{0,0,0,0,0},
		},

		{
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0,0,1,0,0},
			{0,0,0,0,0},
			{0,0,0,0,0},
		},

		{
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0,0,1,0,0},
			{0,0,0,0,0},
			{0,0,0,0,0},
		},

		{
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0,0,1,0,0},
			{0,0,0,0,0},
			{0,0,0,0,0},
		},

		{
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0,0,1,0,0},
			{0,0,0,0,0},
			{0,0,0,0,0},
		},
		{
			{1,1,1,1,1},
			{1,1,1,1,1},
			{1,1,1,1,1},
			{1,1,1,1,1},
			{1,1,1,1,1},
		},
	};

	voxelCount = GetVoxelCount(MapdataBuf, SIZE_MAX);

	Voxel_t* voxelArr = malloc(sizeof(Voxel_t) * voxelCount);
	VoxelModel* voxelBuf = malloc(sizeof(VoxelModel) * voxelCount);

	Voxel_t* gridArr = malloc(sizeof(Voxel_t) * SIZE_MAX);
	VoxelModel* gridBuf = malloc(sizeof(VoxelModel) * SIZE_MAX);

	CreateVoxelMap(SIZE_X, SIZE_Y, SIZE_Z, MapdataBuf, 0.2, voxelArr, gridArr);

	CreateMapBuffer(voxelArr, gridArr,voxelCount, voxelBuf , gridBuf, SIZE_MAX);


	//mapGrid
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glInterleavedArrays(GL_V3F, 0, gridBuf);
	glDrawArrays(GL_QUADS, 0, VOXEL_SIZE_TRIANGLES * SIZE_MAX);

	//Voxel
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glInterleavedArrays(GL_V3F, 0, voxelBuf);
	glDrawArrays(GL_TRIANGLES, 0, VOXEL_SIZE_TRIANGLES * voxelCount);

	free(voxelArr);
	free(voxelBuf);
	free(gridBuf);
}