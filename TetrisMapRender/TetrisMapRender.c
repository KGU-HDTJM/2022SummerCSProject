#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include <process.h>
#include "LinearAlgebra.h"
#include "HDTJMType.h"
#include "StackAlloc.h"

#define W_H 3000
#define W_W 3000
#define VOXEL_DATA_SIZE_QUADS 24
#define VOXEL_DATA_SIZE_TRIANGLES 36

#define SIZE_Y 6
#define SIZE_Z 5
#define SIZE_X 5
#define GRID_MAX_SIZE (SIZE_Y * SIZE_Z * SIZE_X)
#define VOXEL_SIZE 0.2F

//View mode
#define LINE_MODE 0
#define SOLID_MODE 1

int inputPosX, inputPosZ, inputPosY;

int mapDataBuf[GRID_MAX_SIZE] = {// xzy
		
			1,1,1,1,1,
			1,1,1,1,1,
			1,1,1,1,1,
			1,1,1,1,1,
			1,1,1,1,1,
		
			0,0,0,0,0,
			0,0,0,0,0,
			0,0,1,0,0,
			0,0,0,0,0,
			0,0,0,0,0,
	
			0,0,0,0,0,
			0,0,0,0,0,
			0,0,1,0,0,
			0,0,0,0,0,
			0,0,0,0,0,
	
			0,0,0,0,0,
			0,0,0,0,0,
			0,0,1,0,0,
			0,0,0,0,0,
			0,0,0,0,0,
	
			0,0,0,0,0,
			0,0,0,0,0,
			0,0,1,0,0,
			0,0,0,0,0,
			0,0,0,0,0,
	
			0,0,0,0,0,
			0,0,0,0,0,
			0,0,1,0,0,
			0,0,0,0,0,
			0,0,0,0,0,
};

const int sizeH = 1000;
const int sizeW = 1000;

size_t FrameCount;
int bim = 0;
int View = 1;
int angleX = 0;
int angleZ = 0;
int angleY = 0;
int gridMode = 0;
int voxelMode = 0;
voxelCount = 0;
volatile float ModelY = 1;

typedef struct {
	GLfloat Voxel[36][3];
}VoxelModel;

GLfloat R = 0.7F;
GLfloat camx = 0, camy = 0, camz = 1;
GLfloat hrzndegree = 0, vrtcldegree = 0;

pStack_t mapBufStack;

void InitLight(void);
void Keyboard(unsigned char key, int x, int y);
void Display(void);
void Reshape(int width, int height);
GLubyte* LoadBmp(const char* imagePath, int* width, int* height);
void InitTexture(unsigned char* data, int width, int height);
int GetVoxelCount(int* arr, int sizeMax);

void ReVoxelSize(Vector3f_t* tempVoxel, GLfloat size);
void VoxelTrans(Vector3f_t* stdVoxel, Vector3f_t* tempVoxel, GLfloat x, GLfloat y, GLfloat z, GLfloat voxelSize);
void CreateVoxelMap(int sizeX, int sizeY, int sizeZ, int* dataBuf, float voxelSize, Voxel_t* voxelArr, Voxel_t* gridArr);
void CreateMapBuffer(Voxel_t* VoxelArr, Voxel_t* gridArr, int voxelCount, VoxelModel* modelBuf, VoxelModel* gridBuf, int gridSize, GLfloat voxelSize);
void DrawTetrisMap(int* mapDataBuf,int sizeX, int sizeZ , int sizeY, GLfloat voxelSize);
void DrawFrame(int num);
void InputPosition(void* func);
int IsFullFloor(int* arr, const int floor);
void BreakFullFloor(int* arr, const int floor);

void LoadMapBuf(int* mapDataBuf, int sizeX, int sizeY, int sizeZ, GLfloat voxelSize, int voxelCount,
	VoxelModel* outVoxelAdress, VoxelModel* outGridAdress, Voxel_t* voxelArr, Voxel_t* gridArr);
void RenderMap(int* mapDataBuf, int gridSize, int voxelCount, int gridMode,
	VoxelModel* outVoxelAdress, VoxelModel* outGridAdress);


//thread
void FrameRating(void)
{
	while (1)
	{
		Sleep(1000);
		printf("Fps:%llu\n", FrameCount);
		FrameCount = 0;
	}

}

int main(int argc, char** argv)
{

	mapBufStack = CreateStack(
		(sizeof(Voxel_t) + sizeof(VoxelModel)) * GRID_MAX_SIZE * 2
	);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);

	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.0, 0.0, 0.0);//rgba (a: 투명도)

	glutCreateWindow("Lighting");
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	InitLight();// window 생성후 빛 초기화


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


	//_beginthread(FrameRating, 0, (void*)0);
	_beginthread(InputPosition, 0, NULL);

	glutMainLoop();

	_endthread();
	ReleaseStack(mapBufStack);
	//free(Texture);
	return 0;
}


// Texture
GLubyte* LoadBmp(const char* imagePath, int* width, int* height)// 사용시 반환되는 이미지 버퍼의 주소를 꼭 free 시켜주세요
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char* data = NULL;

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
	if (data == NULL) { return NULL; }
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

//glut mainLoop callback functiong
void Display(void)
{
	//_asm int 3;
	if (ModelY < -1) ModelY = 1;
	FrameCount++;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	camy = (GLfloat)R * (GLfloat)sin(vrtcldegree);
	camx = (GLfloat)pow(R * R + camy * camy, (GLfloat)0.5) * (GLfloat)cos(hrzndegree);
	camz = (GLfloat)pow(R * R + camy * camy, (GLfloat)0.5) * (GLfloat)sin(hrzndegree);
	gluLookAt(camx, camy, camz, 0, 0, 0, 0, 1, 0);

	for (int i = 0; i < SIZE_Y; i++)
	{
		if (IsFullFloor(mapDataBuf, i))
		{
			BreakFullFloor(mapDataBuf, i);
		}
	}
	//
	//-------------------------------------
	//
	DrawTetrisMap(mapDataBuf,SIZE_X,SIZE_Z,SIZE_Y,0.1F);

	glFlush();

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
void InitLight(void)
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
			vrtcldegree += 0.1F;
		break;
	case '2':
		if (vrtcldegree > -1)
			vrtcldegree -= 0.1F;
		break;
	case '4':
		hrzndegree += 0.1F;
		break;
	case '6':
		hrzndegree -= 0.1F;
		break;
	case 'm':
		gridMode = (gridMode + 1) % 3;
		break;
	case 'v':
		voxelMode = (voxelMode + 1) % 2;
		break;
	default:
		break;
	}

}
void DrawFrame(int num)
{
	glutPostRedisplay();
	glutTimerFunc(1, DrawFrame, 0);
}


//Tetris Map  설계

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
		tempVoxel[i].X = stdVoxel[i].X + x + voxelSize * 0.5F;
		tempVoxel[i].Y = stdVoxel[i].Y + y;
		tempVoxel[i].Z = stdVoxel[i].Z + z + voxelSize * 0.5F;
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

	ReVoxelSize(stdVoxel, voxelSize * (0.5F));

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
				tempY = -1 * (((0.5F) * sizeY - y) * voxelSize - voxelSize / 2);
				tempZ = ((-0.5F) * sizeZ + z) * voxelSize;
				VoxelTrans(stdVoxel, tempVoxel, tempX, tempY, tempZ, voxelSize);

				if (gridMode == VOXEL_GRID_MODE)
				{
					//그리드 라인 생성
					for (int i = 0; i < 8; i++)
					{
						gridArr[gridArrIdx].Vertex[i].X = tempVoxel[i].X;
						gridArr[gridArrIdx].Vertex[i].Y = tempVoxel[i].Y;
						gridArr[gridArrIdx].Vertex[i].Z = tempVoxel[i].Z;
					}
					gridArrIdx++;
				}


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
void CreateMapBuffer(Voxel_t* VoxelArr, Voxel_t* gridArr, int voxelCount, VoxelModel* modelBuf, VoxelModel* gridBuf, int gridSize, GLfloat voxelSize)
{

	GLuint faceQuads[VOXEL_DATA_SIZE_QUADS] = {
		6,2,0,4,
		3,7,5,1,
		2,6,7,3,
		1,5,4,0,
		7,6,4,5,
		2,3,1,0
	};

	GLuint faceTriangles[VOXEL_DATA_SIZE_TRIANGLES] = {
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

	if (gridMode == VOXEL_GRID_MODE)
	{
		for (int Vidx = 0; Vidx < gridSize; Vidx++)
		{
			for (int idx = 0; idx < VOXEL_DATA_SIZE_QUADS; idx++)
			{
				//gridVertex 
				gridBuf[Vidx].Voxel[idx][0] = gridArr[Vidx].Vertex[faceQuads[idx]].X;
				gridBuf[Vidx].Voxel[idx][1] = gridArr[Vidx].Vertex[faceQuads[idx]].Y;
				gridBuf[Vidx].Voxel[idx][2] = gridArr[Vidx].Vertex[faceQuads[idx]].Z;
			}


		}
	}
	else if (gridMode == MAP_GRID_MODE)
	{
		for (int idx = 0; idx < VOXEL_DATA_SIZE_QUADS; idx++)
		{
			//gridVertex 
			gridBuf[0].Voxel[idx][0] = mapGrid[faceQuads[idx]].X;
			gridBuf[0].Voxel[idx][1] = mapGrid[faceQuads[idx]].Y;
			gridBuf[0].Voxel[idx][2] = mapGrid[faceQuads[idx]].Z;

		}


	}


	for (int Vidx = 0; Vidx < voxelCount; Vidx++)
	{
		for (int idx = 0; idx < VOXEL_DATA_SIZE_TRIANGLES; idx++)
		{
			//voxelVertex
			modelBuf[Vidx].Voxel[idx][0] = VoxelArr[Vidx].Vertex[faceTriangles[idx]].X;
			modelBuf[Vidx].Voxel[idx][1] = VoxelArr[Vidx].Vertex[faceTriangles[idx]].Y;
			modelBuf[Vidx].Voxel[idx][2] = VoxelArr[Vidx].Vertex[faceTriangles[idx]].Z;

		}


	}


}
void DrawTetrisMap(int* mapDataBuf, int sizeX, int sizeZ, int sizeY,GLfloat voxelSize)
{
	int voxelCount;
	int gridMaxSize = sizeX * sizeZ * sizeY;
	voxelCount = GetVoxelCount(mapDataBuf, gridMaxSize);

	
	Push(mapBufStack, &mapBufStack->BP);
	mapBufStack->BP = mapBufStack->SP;
	
	Voxel_t* voxelArr = SubSP(mapBufStack, sizeof(Voxel_t) * voxelCount);
	Voxel_t* gridArr = SubSP(mapBufStack, sizeof(Voxel_t) * gridMaxSize);
	VoxelModel* voxelBufAdress = SubSP(mapBufStack, sizeof(VoxelModel) * voxelCount);
	VoxelModel* gridBufAdress = SubSP(mapBufStack, sizeof(VoxelModel) * gridMaxSize);

	LoadMapBuf(mapDataBuf, sizeX, sizeY, sizeZ, voxelSize, voxelCount,voxelBufAdress,gridBufAdress,voxelArr,gridArr);

	RenderMap(mapDataBuf, voxelSize, voxelCount, gridMode, voxelBufAdress, gridBufAdress);

	mapBufStack->SP = mapBufStack->BP;
	mapBufStack->BP = *mapBufStack->SP;
	(void**)Pop(mapBufStack);
	
}
void LoadMapBuf(int* mapDataBuf, int sizeX, int sizeY, int sizeZ, GLfloat voxelSize, int voxelCount,
	VoxelModel* outVoxelAdress, VoxelModel* outGridAdress, Voxel_t* voxelArr, Voxel_t* gridArr)
{
	int gridSize = sizeX * sizeZ * sizeY;
	CreateVoxelMap(sizeX, sizeY, sizeZ, mapDataBuf, voxelSize, voxelArr, gridArr);
	CreateMapBuffer(voxelArr, gridArr, voxelCount, outVoxelAdress, outGridAdress, gridSize, voxelSize);
}

void RenderMap(int* mapDataBuf, int gridSize, int voxelCount, int gridMode,
	VoxelModel* outVoxelAdress, VoxelModel* outGridAdress)
{

	//필요 인자 input:RenderBuf(gridBuf + voxelBuf)/ SIZE_MAX_SIZE(그리드 크기) / voxelCount / mode(?_GRID_MODE)
	if (gridMode == VOXEL_GRID_MODE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glInterleavedArrays(GL_V3F, 0, outGridAdress);
		glDrawArrays(GL_QUADS, 0, VOXEL_DATA_SIZE_TRIANGLES * GRID_MAX_SIZE);
	}
	else if (gridMode == MAP_GRID_MODE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glInterleavedArrays(GL_V3F, 0, outGridAdress);
		glDrawArrays(GL_QUADS, 0, VOXEL_DATA_SIZE_QUADS);
	}
	//Voxel
	if (voxelMode == SOLIDMODE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	glInterleavedArrays(GL_V3F, 0, outVoxelAdress);
	glDrawArrays(GL_TRIANGLES, 0, VOXEL_DATA_SIZE_TRIANGLES * voxelCount);

}



//기능 분리
int GetVoxelCount(int* arr, const int sizeMax)
{
	int voxelCount = 0;
	for (int i = 0; i < sizeMax; i++)
	{
		voxelCount += arr[i];
	}
	return voxelCount;
}
void Render(VoxelModel* DrawBufferAddress,int drawMode,GLsizei buffeCount)
{
	if (drawMode == LINE_MODE) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (drawMode == SOLID_MODE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glInterleavedArrays(GL_V3F, 0, DrawBufferAddress);
	glDrawArrays(GL_QUADS, 0, buffeCount);
}
void VoxelFaceMapping(int* face,int faceSize ,Voxel_t inVoxel,Vector3f_t* outVoxelVert)
{
	for (int idx = 0; idx < faceSize; idx++)
	{
		sizeof(Voxel_t);
		//gridVertex 
		outVoxelVert[idx].X = inVoxel.Vertex[idx].X;
		outVoxelVert[idx].Y = inVoxel.Vertex[idx].Y;
		outVoxelVert[idx].Z = inVoxel.Vertex[idx].Z;
	}
}
void MulScale(Vector3f_t* tempVoxel, GLfloat size,int vertexSize)
{
	for (int i = 0; i < vertexSize; i++)
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
		tempVoxel[i].X = stdVoxel[i].X + x + voxelSize * 0.5F;
		tempVoxel[i].Y = stdVoxel[i].Y + y;
		tempVoxel[i].Z = stdVoxel[i].Z + z + voxelSize * 0.5F;
	}
}




// 재설계
{
	GLuint faceQuads[VOXEL_DATA_SIZE_QUADS] = {
		6,2,0,4,
		3,7,5,1,
		2,6,7,3,
		1,5,4,0,
		7,6,4,5,
		2,3,1,0
	};

	GLuint faceTriangles[VOXEL_DATA_SIZE_TRIANGLES] = {
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

	GLfloat xMaxPos = SIZE_X * voxelSize * 0.5F;
	GLfloat yMaxPos = SIZE_Y * voxelSize * 0.5F;
	GLfloat zMaxPos = SIZE_Z * voxelSize * 0.5F;

	Vector3f_t mapGrid[8] = {
		{	  xMaxPos,-1 * yMaxPos,-1 * zMaxPos},
		{	  xMaxPos,-1 * yMaxPos,	   zMaxPos},
		{	  xMaxPos,	   yMaxPos,-1 * zMaxPos},
		{	  xMaxPos,	   yMaxPos,	   zMaxPos},

		{-1 * xMaxPos,-1 * yMaxPos,-1 * zMaxPos},
		{-1 * xMaxPos,-1 * yMaxPos,	   zMaxPos},
		{-1 * xMaxPos,	   yMaxPos,-1 * zMaxPos},
		{-1 * xMaxPos,	   yMaxPos,	   zMaxPos}
	};//테트리스의 각 끝 좌표



	int voxelCount;
	int gridMaxSize = sizeX * sizeZ * sizeY;
	voxelCount = GetVoxelCount(mapDataBuf, gridMaxSize);


	Push(mapBufStack, &mapBufStack->BP);
	mapBufStack->BP = mapBufStack->SP;

	Voxel_t* voxelArr = SubSP(mapBufStack, sizeof(Voxel_t) * voxelCount);
	Voxel_t* gridArr = SubSP(mapBufStack, sizeof(Voxel_t) * gridMaxSize);
	VoxelModel* voxelBufAdress = SubSP(mapBufStack, sizeof(VoxelModel) * voxelCount);
	VoxelModel* gridBufAdress = SubSP(mapBufStack, sizeof(VoxelModel) * gridMaxSize);


	//LoadMapBuf(mapDataBuf, sizeX, sizeY, sizeZ, voxelSize, voxelCount, voxelBufAdress, gridBufAdress, voxelArr, gridArr);

		//CreateVoxelMap(sizeX, sizeY, sizeZ, mapDataBuf, voxelSize, voxelArr, gridArr);
				

		//CreateMapBuffer(voxelArr, gridArr, voxelCount, outVoxelAdress, outGridAdress, gridSize, voxelSize);
				





	//RenderMap(mapDataBuf, voxelSize, voxelCount, gridMode, voxelBufAdress, gridBufAdress);





	mapBufStack->SP = mapBufStack->BP;
	mapBufStack->BP = *mapBufStack->SP;
	(void**)Pop(mapBufStack);
}

































int IsFullFloor(int* arr, const int floor)
{
	for (int i = 0; i < SIZE_Z * SIZE_X; i++)
	{
		if (arr[i + (SIZE_Z * SIZE_X) * floor] != 1)
			return 0;
	}
	return 1;
}
void BreakFullFloor(int* arr, const int floor)
{
	for (int i = floor; i < SIZE_Y; i++)
	{
		for (int j = 0; j < SIZE_Z * SIZE_X; j++)
		{
			arr[(SIZE_Z * SIZE_X) * i + j] = arr[(SIZE_Z * SIZE_X) * (i + 1) + j];
		}
	}
	for (int i = 0; i < SIZE_Z * SIZE_X; i++)
	{
		arr[(SIZE_Z * SIZE_X) * (SIZE_Y - 1) + i] = 0;
	}
}
void InputPosition(void* func)
{
	while (True)
	{
		Sleep(100);
		
		printf("Input Position:");
		if (scanf("%d %d %d", &inputPosX, &inputPosZ, &inputPosY))
		{	
			printf("input: X %d Z %d Y %d\n", inputPosX, inputPosZ, inputPosY);
		}
		


		// 기능 추가
	}
}