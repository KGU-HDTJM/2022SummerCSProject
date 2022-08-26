#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 6031)

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include <process.h>
#include "LinearAlgebra.h"
#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "StackAlloc.h"
#include <time.h>



 boolean_t H = FALSE;
 boolean_t D = FALSE;
 boolean_t T = FALSE;
 boolean_t J = FALSE;
 boolean_t M = FALSE;





int imageWidht, imageHeight;
GLubyte* Texture;
int gameLevel = 1;
int gameScore = 0;
int blockDownTime = 2000;
typedef struct
{
	Vector4f_t Vertex[8];
	int TextureID;
} Voxel_t;

#define GetVectorXZY(x, z, y)  { (int)(x), (int)(y), (int)(z) }

#define W_H 3000
#define W_W 3000
#define VOXEL_SIZE_QUADS 24
#define VOXEL_SIZE_TRIANGLES 36

#define VOXEL_SIZE 0.2F
#define WORLD_MAP_SIZE 4.F


//grid View mode
#define MAP_GRID_MODE 0
#define VOXEL_GRID_MODE 1

int inputPosX, inputPosZ, inputPosY;

int SizeY = 12;
int SizeZ = 4;
int SizeX = 4;
int SizeXZY;
int* mapDataBuf;

boolean_t bShouldRun = True;

#define NUMBER_OF_BLOCKS 5
#define SIZE_OF_BLOCK 4
#define FIRST_BLOCK 0
#define NO_SPIN_BLOCK 1
int BlockNumber = FIRST_BLOCK;
boolean_t bIsArrived = True;

const Vector3i_t BlockList[NUMBER_OF_BLOCKS + 1][SIZE_OF_BLOCK] = 
{
	{
		{0,0,0},{0,0,0},{0,0,0},{0,0,0},
	},
	{
		GetVectorXZY(0,0,0), GetVectorXZY(1,0,0),
		GetVectorXZY(0,1,0), GetVectorXZY(1,1,0),
	},					 			  
	{					 			 
		GetVectorXZY(0,0,0),				  
		GetVectorXZY(0,1,0),				  
		GetVectorXZY(0,2,0), GetVectorXZY(1,2,0),
	},					 				  
	{					 				  
		GetVectorXZY(0,0,0),				  
		GetVectorXZY(0,1,0), GetVectorXZY(1,1,0),
		GetVectorXZY(0,2,0),
	},
	{
		GetVectorXZY(0,0,0),
		GetVectorXZY(0,1,0),
		GetVectorXZY(0,2,0),
		GetVectorXZY(0,3,0),
	},					 
	{					 
		GetVectorXZY(0,0,0),
		GetVectorXZY(0,1,0), GetVectorXZY(1,1,0),
							 GetVectorXZY(1,2,0),
	},
};

Vector3i_t Block[SIZE_OF_BLOCK];

#define AXIS_X 0
#define AXIS_Z 2
#define AXIS_Y 1
#define DIR_RIGHT -1
#define DIR_LEFT 1

int spinAxis = AXIS_X;

const int sizeH = 1000;
const int sizeW = 1000;

size_t FrameCount;
int bim = 0;
int View = 1;
int angleX = 0;
int angleZ = 0;
int angleY = 0;
int gridMode = 0;


voxelCount = 0;
volatile float ModelY = 1;

typedef struct {
	GLfloat Voxel[36][3];
}VoxelModel;



GLubyte* textureMap;
pStack_t mapBufStack;
int voxelMode = 0;

//Model View mode
#define WIREMODE 0
#define SOLIDMODE 1
#define IMG_COUNT 5
int idx = 1;

typedef struct {
	GLfloat Voxel[36][2];
}VoxelTex_t;

typedef struct {
	GLfloat Voxel[36][5];
}RenderModel_t;
GLfloat eyeR = 2.5F;
GLfloat camx, camy, camz;
GLfloat hrzndegree = -60.0F, vrtcldegree = 60.0F;

typedef struct
{
	byte_t X;
	byte_t Y;
	byte_t Z;
	byte_t TextureID;
} VoxelInfo_t;

VoxelInfo_t* drawObjectArray;




void Update();
void InitLight(void);
void Keyboard(unsigned char key, int x, int y);
void Display(void);
void Reshape(int width, int height);
GLubyte* LoadBmp(const char* imagePath, int* width, int* height);
void InitTexture(unsigned char* data, int width, int height);
int GetVoxelCount(int* arr, const int sizeMax);

void ReVoxelSize(Vector3f_t* tempVoxel, GLfloat size);
void VoxelTrans(Vector3f_t* stdVoxel, Vector3f_t* tempVoxel, GLfloat x, GLfloat y, GLfloat z, GLfloat voxelSize);
void CreateVoxelMap(int sizeX, int sizeY, int sizeZ, int* dataBuf, float voxelSize, Voxel_t* voxelArr, Voxel_t* gridArr);
void CreateMapBuffer(Voxel_t* VoxelArr, Voxel_t* gridArr, int voxelCount, VoxelModel* modelBuf, VoxelModel* gridBuf, int gridSize, GLfloat voxelSize);

void DrawTetrisMap(int* mapDataBuf, int sizeX, int sizeZ, int sizeY, GLfloat voxelSize);
void VoxelTexMapping(int texturID, Vector2f_t* texBuf, int imgCount);
void DrawControlModel(Vector3i_t* blockModel, int voxelCount, int sizeX, int sizeY, int sizeZ, int* dataBuf, float voxelSize, int textureID, int imgCount);
void DrawAxis(void);
void InputPositionThread(void);
void ToggleThisPosition(int* arr, const int x, const int z, const int y);

int IsFullFloor(int* arr, const int floor);
void BreakFullFloor(int* arr, const int floor);

void MoveBlock(int dx, int dz, int dy, Vector3i_t* block);
boolean_t IsEnabledToMoveBlockToNextPosition(int dx, int dz, int dy, Vector3i_t* block, int* arr);
void FixBlockInMap(int blockNumber, Vector3i_t* block, int* arr);
void SetNewBlock(Vector3i_t* block, const Vector3i_t* newBlock);

void SpinBlock(int spinAxis, int dir, Vector3i_t* block, int* arr);

void PushVoxelInfo(pStack_t stack, VoxelInfo_t data);
void PrintMap(int* map);

void DrawFrame(int num)
{
	glutPostRedisplay();
	glutTimerFunc(1, DrawFrame, 0);
}
void GameInfoRatingThread(void)
{
	while (True)
	{
		if (!bShouldRun)
		{
			_endthread();
			return;
		}
		Sleep(1000);
		if (bShouldRun)
		{
			system("cls");
			printf("FPS:%llu\n<LEVEL %d> \nSCORE : %d\n", FrameCount,gameLevel, gameScore);
			FrameCount = 0;
		}
		
	
	}
	
}
void BlockDownThread(void) {
	while(bShouldRun)
	{
		if (!bShouldRun) {
			_endthread();
		}
		Sleep(blockDownTime);
		MoveBlock(0, 0, -1, Block);
	}
	
}

void DrawWorld(float size, int imgCount, int levelTexture);
void GameShutDown();



//라이브러리 들어오면 대체할 함수 
void VertexFaceMapping(int* face, size_t facelen, Vector3f_t* vertex, Vector3f_t* vertexBuffer)
{
	for (int i = 0; i < facelen; i++)
	{
		vertexBuffer[i].X = vertex[face[i]].X;
		vertexBuffer[i].Y = vertex[face[i]].Y;
		vertexBuffer[i].Z = vertex[face[i]].Z;
	}
}

void CreateTexBuffer(int* mapDataBuf, int sizeX, int sizeZ, int sizeY, VoxelTex_t* texBuf, int imgCount);
void LoadMapBuf(int* mapDataBuf, int sizeX, int sizeY, int sizeZ, GLfloat voxelSize, int voxelCount,
	VoxelModel* outVoxelAddress, VoxelModel* outGridAddress, Voxel_t* voxelArr, Voxel_t* gridArr, VoxelTex_t* outTexBufAddress);

void LoadMapBuf(int* mapDataBuf, int sizeX, int sizeY, int sizeZ, GLfloat voxelSize, int voxelCount,
	VoxelModel* outVoxelAddress, VoxelModel* outGridAddress, Voxel_t* voxelArr, Voxel_t* gridArr, VoxelTex_t* outTexBufAddress)
{
	int gridSize = sizeX * sizeZ * sizeY;
	CreateVoxelMap(sizeX, sizeY, sizeZ, mapDataBuf, voxelSize, voxelArr, gridArr);
	CreateMapBuffer(voxelArr, gridArr, voxelCount, outVoxelAddress, outGridAddress, gridSize, voxelSize);
	CreateTexBuffer(mapDataBuf, sizeX, sizeZ, sizeY, outTexBufAddress, IMG_COUNT);


}
void VoxelTexMapping(int texturID, Vector2f_t* texBuf, int imgCount)
{
	Vector2f_t texCoord[6] = {
		   {0.F ,0.F},// w h
		   {1.F, 0.F},
		   {1.F, 1.F},

		   {1.F, 1.F},
		   {0.F, 1.F},
		   {0.F ,0.F}
	};
	for (int i = 0; i < VOXEL_SIZE_TRIANGLES; i++) {
		texBuf[i].X = texCoord[i % 6].X / imgCount + (texturID - 1 % imgCount) * (1.F / imgCount);
		texBuf[i].X = texCoord[i % 6].X / imgCount + (texturID-1 % imgCount) * (1.F / imgCount);
		texBuf[i].Y = texCoord[i % 6].Y;
	}
}
void CreateTexBuffer(int* mapDataBuf, int sizeX, int sizeZ, int sizeY, VoxelTex_t* texBuf, int imgCount)
{

	int voxelArrIdx = 0;
	int x, y, z;
	for (y = 0; y < sizeY; y++)
	{
		for (z = 0; z < sizeZ; z++)
		{
			for (x = 0; x < sizeX; x++)
			{
				// 복쉘 채우기
				int texturID = mapDataBuf[(sizeZ * sizeX) * y + sizeX * z + x];
				if (texturID != 0)
				{
					VoxelTexMapping(texturID, texBuf + voxelArrIdx, imgCount);
					voxelArrIdx++;
				}
			}
		}
	}


}

void RenderMap(RenderModel_t* renderBuf, int* mapDataBuf, int gridSize, int voxelCount, int gridMode, int voxelMode,
	VoxelModel* outVoxelAdress, VoxelModel* outGridAdress, VoxelTex_t* textureBufAddress)
{	
	Vector2f_t texBuf[VOXEL_SIZE_QUADS] = {0,};
	if (bShouldRun) {
		if (gridMode == MAP_GRID_MODE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glTexCoordPointer(2, GL_FLOAT, 0, texBuf);
			glColor3f(0.F,0.F,0.F);
			glVertexPointer(3, GL_FLOAT, 0, outGridAdress);
			
			

			glDrawArrays(GL_QUADS, 0, VOXEL_SIZE_QUADS);
			DrawAxis();
			glColor3f(1.F,1.F,1.F,1.F);
		}
		//Voxel
		if (voxelMode == SOLIDMODE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		glVertexPointer(3, GL_FLOAT, 0, outVoxelAdress);
		glTexCoordPointer(2, GL_FLOAT, 0, textureBufAddress);
		glDrawArrays(GL_TRIANGLES, 0, VOXEL_SIZE_TRIANGLES * voxelCount);

	
	}
	else {// 게임 오버 렌더 
		Vector2f_t texCoord[4] = {
			{0.F, 0.F},
			{1.F ,0.F},
			{1.F, 1.F},
			{0.F, 1.F},
		};
		for (int i = 0; i < VOXEL_SIZE_QUADS; i++) {
			texBuf[i].X = texCoord[i % 4].X;
			texBuf[i].Y = texCoord[i % 4].Y;
		}
		Texture = LoadBmp("../resource/GameOver.bmp", &imageWidht, &imageHeight);
		
		InitTexture(Texture, imageWidht, imageHeight);
		glTexCoordPointer(2, GL_FLOAT, 0, texBuf);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glVertexPointer(3, GL_FLOAT, 0, outGridAdress);
	
		glDrawArrays(GL_QUADS, 0, VOXEL_SIZE_QUADS);
		glutSwapBuffers();
		HFree(Texture);
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

	
	for (int y = 0; y < sizeY; y++)
	{
		for (int z = 0; z < sizeZ; z++)
		{
			for (int x = 0; x < sizeX; x++)
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
					for (int i = 0; i < 8; i++) 
					{
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



	GLfloat xMaxPos = SizeX * voxelSize * 0.5F;
	GLfloat yMaxPos = SizeY * voxelSize * 0.5F;
	GLfloat zMaxPos = SizeZ * voxelSize * 0.5F;

	Vector3f_t mapGrid[8] = {
		{	  xMaxPos,-1 * yMaxPos,-1 * zMaxPos},
		{	  xMaxPos,-1 * yMaxPos,	   zMaxPos},
		{	  xMaxPos,	   yMaxPos,-1 * zMaxPos},
		{	  xMaxPos,	   yMaxPos,	   zMaxPos},

		{-1 * xMaxPos,-1 * yMaxPos,-1 * zMaxPos},
		{-1 * xMaxPos,-1 * yMaxPos,	   zMaxPos},
		{-1 * xMaxPos,	   yMaxPos,-1 * zMaxPos},
		{-1 * xMaxPos,	   yMaxPos,	   zMaxPos}
	};

	if (gridMode == MAP_GRID_MODE)
	{
		for (int idx = 0; idx < VOXEL_SIZE_QUADS; idx++)
		{


			//gridVertex 
			gridBuf[0].Voxel[idx][0] = mapGrid[faceQuads[idx]].X;
			gridBuf[0].Voxel[idx][1] = mapGrid[faceQuads[idx]].Y;
			gridBuf[0].Voxel[idx][2] = mapGrid[faceQuads[idx]].Z;

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

void DrawAxis(void)
{

	Vector3f_t axis[32] =
	{	

		{-0.4F, 0.F , -1 - 0.4F},
		{-0.2F, 0.F , -1 - 0.2F},
		{-0.2F, 0.F , -1 - 0.2F},
		{0.F , 0.F , -1 - 0.4F},
		{0.F , 0.F , -1 - 0.4F},
		{0.2F, 0.F , -1 - 0.2F},
		{0.2F, 0.F , -1 - 0.2F},
		{0.4F, 0.F , -1 - 0.4F},


		{0.1F, 0.F , +1 - 0.2F},
		{-0.1F, 0.F , +1},
		{-0.1F, 0.F , +1},
		{0.1F , 0.F , +1 + 0.2F},
		{0.1F , 0.F , +1 + 0.2F},
		{-0.1F, 0.F , +1 + 0.4F},


		{-1- 0.2F, 0.F , 0.4F},
		{-1- 0.4F, 0.F , 0.F},
		{-1- 0.4F, 0.F , 0.F},
		{-1- 0.6F, 0.F , 0.4F},
		{-1- 0.3F , 0.F , 0.2F},
		{-1- 0.5F, 0.F , 0.2F},

		{1 + 0.2F, 0.F ,  0.2F},
		{1 + 0.2F, 0.F , -0.2F},
		{1 + 0.2F, 0.F , 0.2F},
		{1 + 0.4F, 0.F , 0.15F},
		{1 + 0.4F, 0.F , 0.15F},
		{1 + 0.5F , 0.F , 0.0F},
		
		{1 + 0.5F , 0.F , 0.0F},
		{1 + 0.4F, 0.F , -0.15F},

		{1 + 0.4F, 0.F , -0.15F},
		{1 + 0.2F, 0.F , -0.2F},


	};
	Vector2f_t texClear[5] =
	{
		0,
	};
	
	glTexCoordPointer(2,GL_FLOAT,0, texClear);
	glVertexPointer(3, GL_FLOAT, 0, axis);
	glDrawArrays(GL_LINES, 0, 32);

}

void DrawTetrisMap(int* mapDataBuf, int sizeX, int sizeZ, int sizeY, GLfloat voxelSize)
{
	int voxelCount = GetVoxelCount(mapDataBuf, SizeXZY);


	Push(mapBufStack, &mapBufStack->BP);
	mapBufStack->BP = mapBufStack->SP;

	Voxel_t* voxelArr = SubSP(mapBufStack, sizeof(Voxel_t) * voxelCount);

	Voxel_t* gridArr = SubSP(mapBufStack, sizeof(Voxel_t) * SizeXZY);


	VoxelTex_t* textureBufAddress = SubSP(mapBufStack, sizeof(VoxelTex_t) * voxelCount);

	VoxelModel* voxelBufAddress = SubSP(mapBufStack, sizeof(VoxelModel) * voxelCount);
	
	VoxelModel* gridBufAddress = SubSP(mapBufStack, sizeof(VoxelModel) * SizeXZY);
	

	LoadMapBuf(mapDataBuf, sizeX, sizeY, sizeZ, voxelSize, voxelCount, voxelBufAddress, gridBufAddress, voxelArr, gridArr, textureBufAddress);

	RenderModel_t* renderBuf = SubSP(mapBufStack, sizeof(RenderModel_t) * voxelCount);
	RenderMap(renderBuf, mapDataBuf, voxelSize, voxelCount, gridMode, voxelMode, voxelBufAddress, gridBufAddress, textureBufAddress);

	mapBufStack->SP = mapBufStack->BP;
	mapBufStack->BP = *(void**)Pop(mapBufStack);
	

	
	
}
//modelTracer
void DrawControlModel(Vector3i_t* blockModel, int voxelCount, int sizeX, int sizeY, int sizeZ, int* dataBuf, float voxelSize, int textureID, int imgCount)
{
	if (textureID == 0 || !bShouldRun) return;
	float tempX, tempY, tempZ;
	int x, y, z;
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

	ReVoxelSize(stdVoxel, voxelSize * (0.5F));
	VoxelModel* modelBuffer = HAlloc(sizeof(VoxelModel) * voxelCount, False, NULL);
	VoxelTex_t* texBuffer = HAlloc(sizeof(VoxelTex_t) * voxelCount, False, NULL);


	for (int i = 0; i < voxelCount; i++)
	{
		x = blockModel[i].X;
		z = blockModel[i].Z;
		y = blockModel[i].Y;
		Vector3f_t tempVoxel[8] = { 0, };
		tempX = ((-0.5F) * sizeX + x) * voxelSize;
		tempY = -1 * (((0.5F) * sizeY - y) * voxelSize - voxelSize / 2);
		tempZ = ((-0.5F) * sizeZ + z) * voxelSize;
		VoxelTrans(stdVoxel, tempVoxel, tempX, tempY, tempZ, voxelSize);
		VertexFaceMapping(faceTriangles, VOXEL_SIZE_TRIANGLES, tempVoxel, modelBuffer + i);
		VoxelTexMapping(textureID, texBuffer + i, imgCount);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glVertexPointer(3, GL_FLOAT, 0, modelBuffer);
	glTexCoordPointer(2, GL_FLOAT, 0, texBuffer);


	glDrawArrays(GL_TRIANGLES, 0, VOXEL_SIZE_TRIANGLES * voxelCount);
	

	HFree(texBuffer);
	HFree(modelBuffer);


}
int mouseCursor = 1;
int curx = 0, cury = 0;
int dPI = 10;
void PassiveMouse(int x, int y)
{
	int dx = curx - x;
	int dy = cury - y;
	curx = x;
	cury = y;
	if (abs(dx) < 20 && abs(dy) < 20)
		hrzndegree += (float)dx;
	vrtcldegree += (float)dy;
	if (vrtcldegree < 0 || vrtcldegree > 180) {
		vrtcldegree -= (float)dy;
	}
}
#define PI 3.141592F;
void WheelMouse(int wheel, int direction, int x, int y)
{
	if (direction > 0 && eyeR > 2.5F) {

		eyeR -= 0.1F;
	}
	else if(eyeR < 4.F)
	{
		eyeR += 0.1F;
	}
}
void ClickMouse(int button, int state, int x, int y)
{


	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			spinAxis = AXIS_X;
		}
		else if (button == GLUT_MIDDLE_BUTTON)
		{
			spinAxis = AXIS_Z;

		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			spinAxis = AXIS_Y;

		}

		SpinBlock(spinAxis, -1, Block, mapDataBuf);
	}

}
int main(int argc, char** argv)
{
	InitMem(1 << 30);
	
	SizeXZY = SizeX * SizeZ * SizeY;

	mapBufStack = CreateStack(
		(sizeof(Voxel_t) + sizeof(VoxelModel)) * SizeXZY * 30
	);
	

	mapDataBuf = HAlloc(sizeof(int) * SizeXZY, False, NULL);
	

	for (int i = 0 ; i < SizeXZY ; i++)
	{	
		*(mapDataBuf + i) = 0 /*asdf[i]*/;
	}
	srand((unsigned int)time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(300, 40);


	glutCreateWindow("Tetris");
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	//InitLight();// window 생성후 빛 초기화
	Texture = LoadBmp("../resource/BlockTexture.bmp", &imageWidht, &imageHeight);
	InitTexture(Texture, imageWidht, imageHeight);
	// UPDATE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_TEXTURE_2D);

	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutPassiveMotionFunc(PassiveMouse);
	glutMouseFunc(ClickMouse);
	glutMouseWheelFunc(WheelMouse);


	_beginthread(GameInfoRatingThread, 0, (void*)0);
	//_beginthread((_beginthread_proc_type)InputPositionThread, 0, (void*)0);
	_beginthread(BlockDownThread,0,(void*)0);
	glutMainLoop();
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

	data = (GLubyte*)HAlloc(sizeof(GLubyte) * imageSize, False, NULL);
	if (data == NULL)
		return NULL;
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
	clock_t i = clock();

	

	//_asm int 3;
	if (ModelY < -1) ModelY = 1;
	FrameCount++;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(131.F / 255, 3.F / 255, 3.F / 255, 1.F);//rgba (a: 투명도) // 배경색
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	GLfloat radhrz = (90 - hrzndegree / 180) * PI;
	GLfloat radvrt = ((90 - vrtcldegree) / 180) * PI;
	camx = (GLfloat)eyeR * (GLfloat)cos(radvrt) * (GLfloat)cos(radhrz);
	camy = (GLfloat)eyeR * (GLfloat)sin(radvrt);
	camz = (GLfloat)eyeR * (GLfloat)cos(radvrt) * (GLfloat)sin(radhrz);
	gluLookAt(camx, camy, camz, 0, 0, 0, 0, 1, 0);


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glutSolidCube(0.5);


	Update();
	DrawControlModel(Block, SIZE_OF_BLOCK, SizeX, SizeY, SizeZ, (int*)mapDataBuf, VOXEL_SIZE, BlockNumber, IMG_COUNT);
	DrawTetrisMap((int*)mapDataBuf, SizeX, SizeZ, SizeY, VOXEL_SIZE);
	DrawWorld(WORLD_MAP_SIZE , IMG_COUNT, gameLevel);
	


	glFlush();
	if (!bShouldRun) 
	{
		GameShutDown();
	}
	
	while (clock() - i <= 6)
	{
		Sleep(1);
	}
	glutSwapBuffers();
	glutPostRedisplay();

}
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	GLfloat factor_w = (GLfloat)width / (GLfloat)sizeW;
	GLfloat factor_h = (GLfloat)height / (GLfloat)sizeH;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0F * factor_w, 1.0F * factor_w, -1.0F * factor_h, 1.0F * factor_h, 1.f, 20.0f);

}
void InitLight(void)
{
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	GLfloat light_global_ambient[] = { 1.F,1.F,1.F,1.F };
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
	GLfloat matrial_0_ambient[] = { 1.F,1.F,1.F,1.F };
	GLfloat matrial_0_diffuse[] = { 0.0F,0.0F,0.0F,1.0F }; // 물체의 색상을 가장 직접적으로 결정
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
		// 카메라 돌리기
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

	case 'M':
	case 'm':
		gridMode = (gridMode + 1) % 3;
		M = True;
		break;
		}
		// 블럭 움직이기
		{
	case 'W':
	case 'w':
		MoveBlock(0, -1, 0, Block);
		break;
	case 'S':
	case 's':
		MoveBlock(0, +1, 0, Block);
		break;
	case 'A':
	case 'a':
		MoveBlock(-1, 0, 0, Block);
		break;
	case 'D':
	case 'd':
		MoveBlock(+1, 0, 0, Block);
		D = True;
		break;
	case 9://tap
		MoveBlock(0, 0, -1, Block);
		break;
		}
		// 회전축 정하기
		{
	case 'Z':
	case 'z':
		spinAxis = AXIS_Z;
		break;
	case 'X':
	case 'x':
		spinAxis = AXIS_X;

		break;
	case 'C':
	case 'c':
		spinAxis = AXIS_Y;
		break;
	case 'R':
	case 'r':
		spinAxis = (spinAxis + 1) % 3;
		}
		// 블럭 회전시키기
		{
	case 'Q':
	case 'q':
		SpinBlock(spinAxis, DIR_LEFT, Block, mapDataBuf);
		break;
	case 'E':
	case 'e':
		SpinBlock(spinAxis, DIR_RIGHT, Block, mapDataBuf);
		break;
		}
		// 그 밖의 잡다한거
		{
	case 'v':
		voxelMode = (voxelMode + 1) % 2;
		break;
	case '+':
		dPI -= 1;
		break;
	case '-':
		dPI += 1;
		break;
	case 27:// esc
		system("PAUSE");
		break;
	case '1':
		PrintMap(mapDataBuf);
		break;
	case 'p':
		printf("%d\n", GetVoxelCount(mapDataBuf, SizeXZY));
		break;
	case 'H':
		H = True;
		break;
	case 'T':
		T = True;
		break;
	case 'J':
		J = True;
		break;
		}
	
	default:
		break;
	}
	if (H && D && T && J && M)
	{
		printf("Easter Egg Mode!!!!");
		Sleep(2000);
		Texture = LoadBmp("../resource/TeamFaceBlock.bmp", &imageWidht, &imageHeight);
		InitTexture(Texture, imageWidht, imageHeight);
		H = FALSE;
		D = FALSE;
		T = FALSE;
		J = FALSE;
		M = FALSE;
	}
}

int GetVoxelCount(int* arr, const int sizeMax)
{
	int voxelCount = 0;
	for (int i = 0; i < sizeMax; i++)
	{
		voxelCount += !!arr[i];
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
		tempVoxel[i].X = stdVoxel[i].X + x + voxelSize * 0.5F;
		tempVoxel[i].Y = stdVoxel[i].Y + y;
		tempVoxel[i].Z = stdVoxel[i].Z + z + voxelSize * 0.5F;
	}
}
void DrawWorld(float size, int imgCount, int levelTexture)
{
	Vector3f_t backGround[8] = {
	{-1.F,-1.F,-1.F},
	{-1.F,-1.F, 1.F},
	{-1.F, 1.F,-1.F},
	{-1.F, 1.F, 1.F},

	{ 1.F,-1.F,-1.F},
	{ 1.F,-1.F, 1.F},
	{ 1.F, 1.F,-1.F},
	{ 1.F, 1.F, 1.F}
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

	ReVoxelSize(backGround, size);
	VoxelModel* modelBuffer = HAlloc(sizeof(VoxelModel), False, NULL);
	VoxelTex_t* texBuffer = HAlloc(sizeof(VoxelTex_t), False, NULL);

	VertexFaceMapping(faceTriangles, VOXEL_SIZE_TRIANGLES, backGround, modelBuffer);
	VoxelTexMapping(levelTexture, texBuffer, imgCount);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glVertexPointer(3, GL_FLOAT, 0, modelBuffer);
	glTexCoordPointer(2, GL_FLOAT, 0, texBuffer);
	glDrawArrays(GL_TRIANGLES, 0, VOXEL_SIZE_TRIANGLES);
	HFree(texBuffer);
	HFree(modelBuffer);


}

int IsFullFloor(int* arr, const int floor)
{
	boolean_t bIsFullFloor = True;
	for (int i = 0; i < SizeZ * SizeX; i++)
	{
		bIsFullFloor = bIsFullFloor && !!arr[i + (SizeZ * SizeX) * floor];
		/*if (arr[i + (SizeZ * SizeX) * floor] != 0)
		{
			PushVoxelInfo(drawObjectArray, (VoxelInfo_t) { i% SizeX, floor, i / SizeX, arr[i + (SizeZ * SizeX) * floor] });
		}*/
	}
	/*if (bIsFullFloor)
	{
		AddSP(drawObjectArray, sizeof(VoxelInfo_t) * SizeZ * SizeX);
	}*/
	return bIsFullFloor;
}
void BreakFullFloor(int* arr, const int floor)
{
	for (int i = floor; i < SizeY; i++)
	{
		for (int j = 0; j < SizeZ * SizeX; j++)
		{
			arr[(SizeZ * SizeX) * i + j] = arr[(SizeZ * SizeX) * (i + 1) + j];
		}
	}
	for (int i = 0; i < SizeZ * SizeX; i++)
	{
		arr[(SizeZ * SizeX) * (SizeY - 1) + i] = 0;
	}
}
void InputPositionThread(void)
{
	while (True)
	{
		Sleep(100);
		printf("Input Position:");
		if (!scanf("%d %d %d", &inputPosX, &inputPosZ, &inputPosY))
			;
		printf("input: X %d Z %d Y %d\n", inputPosX, inputPosZ, inputPosY);

		ToggleThisPosition((int*)mapDataBuf, inputPosX, inputPosZ, inputPosY);
	}
}
void ToggleThisPosition(int* arr, const int x, const int z, const int y)
{
	arr[(SizeZ * SizeX) * inputPosY + SizeX * inputPosZ + inputPosX] = !arr[(SizeZ * SizeX) * inputPosY + SizeX * inputPosZ + inputPosX];
}

boolean_t IsEnabledToMoveBlockToNextPosition(int dx, int dz, int dy, Vector3i_t* block, int* arr)
{
	for (int i = 0; i < SIZE_OF_BLOCK; i++)
	{
		int nx = block[i].X + dx;
		int nz = block[i].Z + dz;
		int ny = block[i].Y + dy;

		if (ny < 0)
		{
			bIsArrived = True;
			return False;
		}
		if (nx < 0 || nx >= SizeX || nz < 0 || nz >= SizeZ)
		{
			return False;
		}
		if (dy == -1 && arr[(SizeZ * SizeX) * ny + SizeX * nz + nx] != 0)
		{
			bIsArrived = True;
			return False;
		}
		if (arr[(SizeZ * SizeX) * ny + SizeX * nz + nx] != 0)
		{
			return False;
		}
	}
	return True;
}
void MoveBlock(int dx, int dz, int dy, Vector3i_t* block)
{
	if (!IsEnabledToMoveBlockToNextPosition(dx, dz, dy, block, (int*)mapDataBuf))
		return;

	for (int i = 0; i < SIZE_OF_BLOCK; i++)
	{
		block[i].X += dx;
		block[i].Z += dz;
		block[i].Y += dy;
	}
	
}
void FixBlockInMap(int blockNumber, Vector3i_t* block, int* arr)
{
	for (int i = 0; i < SIZE_OF_BLOCK; i++)
	{
		arr[(SizeZ * SizeX) * block[i].Y + SizeX * block[i].Z + block[i].X] = blockNumber;
	}
}
void SetNewBlock(Vector3i_t* block, const Vector3i_t* newBlock)
{
	for (int i = 0; i < SIZE_OF_BLOCK; i++)
	{
		block[i].X = newBlock[i].X;
		block[i].Z = newBlock[i].Z;
		block[i].Y = newBlock[i].Y;
	}
}

void SpinBlock(int spinAxis, int dir, Vector3i_t* block, int* arr)
{
	const int centerBlockIdx = 1;
	Vector3i_t centerBlock = block[centerBlockIdx];
	Vector3i_t* distFromCenter = HAlloc(sizeof(Vector3i_t) * SIZE_OF_BLOCK, False, NULL);
	Vector3i_t* result = HAlloc(sizeof(Vector3i_t) * SIZE_OF_BLOCK, False, NULL);

	for (int i = 0; i < SIZE_OF_BLOCK; i++)
	{
		distFromCenter[i].X = block[i].X - centerBlock.X;
		distFromCenter[i].Z = block[i].Z - centerBlock.Z;
		distFromCenter[i].Y = block[i].Y - centerBlock.Y;
	}
	
	for (int i = 0; i < SIZE_OF_BLOCK; i++)
	{
		Vector3i_t temp = distFromCenter[i];
		distFromCenter[i].V[(3 + spinAxis - 1) % 3] = temp.V[(3 + spinAxis + 1) % 3];
		distFromCenter[i].V[(3 + spinAxis + 1) % 3] = temp.V[(3 + spinAxis - 1) % 3];

		distFromCenter[i].V[(3 + spinAxis + dir) % 3] *= -1;

		int nx = centerBlock.X + distFromCenter[i].X;
		int nz = centerBlock.Z + distFromCenter[i].Z;
		int ny = centerBlock.Y + distFromCenter[i].Y;

		if (nx < 0 || nx >= SizeX || nz < 0 || nz >= SizeZ || ny < 0 || ny >= SizeY)
		{
			HFree(distFromCenter);
			HFree(result);
			return;
		}
		if (arr[(SizeZ * SizeX) * ny + SizeX * nz + nx] != 0)
		{
			HFree(distFromCenter);
			HFree(result);
			return;
		}
		result[i] = (Vector3i_t){ nx, ny, nz };
	}
	
	for (int i = 0; i < NUMBER_OF_BLOCKS; i++)
	{
		block[i] = result[i];
	}
	
	HFree(distFromCenter);
	HFree(result);
}

void PushVoxelInfo(pStack_t stack, VoxelInfo_t data)
{
	*(VoxelInfo_t*)SubSP(stack, sizeof(VoxelInfo_t)) = data;
}

void PrintMap(int* map)
{
	for (int i = 0; i < SizeY; i++)
	{
		for (int j = 0; j < SizeZ; j++)
		{
			for (int k = 0; k < SizeX; k++)
			{
				printf("%d ", map[(SizeZ * SizeX) * i + SizeX * j + k]);

			}
			printf("\n");
		}
		printf("\n");
	}
}
void GameShutDown()
{
	system("cls");


	printf("+++++++++++++++++++++++\n");
	printf("+GAME OVER : Score: %d+\n", gameScore);
	printf("+++++++++++++++++++++++\n");

	

	printf("\n==========Close Program========== \n");

	for (int i = 0; i < 3; Sleep(1000))
	{
		printf("%dsec later game close\n", 3 - (i++));
		
	}
	system("pause");
	printf("\n==========memoryUsageHistory==========\n");
	DumpHeap(NULL);
	ReleaseStack(mapBufStack);
	ReleaseMainMem();
	exit(1);

}
void Update()
{
	if (bIsArrived == True) // 블록이 바닥(or 다른 블록)에 안착하면
	{	

		//~~~~~~~~~~~~~~ 8월 25 일 업데이트 
		gameScore++;
		if (gameScore >= 50 * (gameLevel + 1))
		{
			blockDownTime -= 200;
			gameLevel++;
		}
		//~~~~~~~~~~~~~~
		bIsArrived = False;
		if (BlockNumber != FIRST_BLOCK) // 맨 처음 실행하는게 아니면
		{
			FixBlockInMap(BlockNumber, Block, (int*)mapDataBuf); // blockNumber번째 블록을 맵에다가 넣고
		}
		
		int bonusScore = 0;
		for (int i = 0; i < SizeY; i++) // 모든 층에
		{
			if (IsFullFloor(mapDataBuf, i)) // 다 찬 층이 있는지 체크하고
			{
				printf("\n!Floor Break! Bonus +10score\n");
				gameScore += 10;
				BreakFullFloor(mapDataBuf, i); // 다 찼으면 부숴서 윗층들을 다 내리고
				i--;
				bonusScore++;
			}
		}
		gameScore += (bonusScore * (bonusScore + 1)) / 2 * 5;

		BlockNumber = rand() % NUMBER_OF_BLOCKS + 1; // 새로운 블럭을 정하고
		SetNewBlock(Block, BlockList[BlockNumber]);
		for (int i = 0; i < SIZE_OF_BLOCK; i++)
		{
			Block[i].Y = SizeY - 1;
		}
		if (mapDataBuf[SizeZ * SizeX * (SizeY - 1)] != 0) // 만약 블럭이 더 내려갈 수 없다면
		{
			bShouldRun = False; // 게임 종료
		}
	}
}