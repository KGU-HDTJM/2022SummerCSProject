/*
* 파일 생성
* 1. 다현이 만든 .ply 파일에서 Voxel 추출
* 2. 민재가 만든 복셀 정렬을 적용
*
* 3-1. 정렬된 복셀을 스스로 지정한 포맷에 맞게 쓴다(.kdhr). 데이터의 경우 binary로쓴다.
*		헤더는 8바이트 문자열, 크기를 나타내는 숫자는 8바이트 정수
* 3-2. header정보를 얻을 수 있는 헤더를 ####head, ....head, ##vertex Offset Length
* =============================== 민재, 다현 (필요하면 태윤)==========================================
* 4. (민재형과 함께) 태윤이가 만들고 있는 복셀 맵핑 구조로 복셀을 맵핑한 배열 만들기 byte단위로 맵핑 예정,
*		회전의 기준점 -1, 기준점 잡는 방식은 민재형(무게중심 좌표를잡아서, 가까운 공간에 맵핑)
* ====================================================================================================
* 5.  .kdhr파일을 내보낸다.
* 파일 읽기
* 1. .kdhr파일 읽어서
* 2. 정의된 구조체 채우기
* 3. 구조체 리턴
*/

// Kdhr data;
// memcpy(vertexArr, data.Source + data.VertexOffset, data.VertexLength);

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 6031)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HDTJMType.h"

#define GetVector4f(x, y, z, w) ( (Vector4f_t){ (float)(x), (float)(y), (float)(z), (float)(w) } )
#define BUFFER_SIZE 0x80
#define GetRGBA(red, green, blue, alpha)\
	(RGBA_t){ (byte_t)red, (byte_t)green, (byte_t)blue, (byte_t)alpha };
#define MAX_FILE_SIZE 0x8000000

typedef union
{
	struct
	{
		byte_t Red;
		byte_t Green;
		byte_t Blue;
		byte_t Alpha;
	};
	int rgba;
} RGBA_t;


char buf[BUFFER_SIZE] = { 0, };

void StoreVertex(
	Vector3f_t* outVertex, FILE* fp, int* colorIndex,
	RGBA_t* colorArr, int* vertexIndexArr, int elementVertex
)
{
	float x = 0;
	float y = 0;
	float z = 0;
	int tempR = 0;
	int tempG = 0;
	int tempB = 0;
	RGBA_t tempColor = { 0, };
	for (size_t i = 0, j = 0, k = 0; i < elementVertex; i++)
	{
		fgets(buf, BUFFER_SIZE, fp);
		sscanf(buf, "%f %f %f %d %d %d", &x, &y, &z, &tempR, &tempG, &tempB);
		/*	tempColor.Red = tempR;
		tempColor.Green = tempG;
		tempColor.Blue = tempB;*/
		tempColor = GetRGBA(tempR, tempG, tempB, 0);

		// CMP_COLOR: // color_count == voxel_count; 
		for (j = 0; j < *colorIndex; j++) // 2(1)
		{
			if (colorArr[j].rgba == tempColor.rgba)
			{
				goto CMP_ELEMENTS;
			}
		}
		colorArr[*colorIndex].rgba = tempColor.rgba;
		(*colorIndex)++; // 1(0 -> 1), 
	CMP_ELEMENTS:
		for (k = 0; k < vertexIndexArr[j]; k++) // 0
		{
			if (outVertex[8 * j + k].X == x
				&& outVertex[8 * j + k].Y == y
				&& outVertex[8 * j + k].Z == z)
			{
				goto ELEMENT_EXIST;
			}
		}
		outVertex[8 * j + k].X = x;
		outVertex[8 * j + k].Y = y;
		outVertex[8 * j + k].Z = z;
		vertexIndexArr[j]++; // 1(0->1);
	ELEMENT_EXIST:;
	}
}

void SortVoxelArr(Vector3f_t* vertex, Voxel_t* outVoxel)
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

int VertexIO(void)
{
	FILE* fp = fopen("../resource/testio.ply", "r");
	if (fp == NULL) //파일없으면 에러처리
	{
		printf("파일 없다.\n");
		return -1;
	}
	int lineCounter = 0;
	int elementVertex = 0; //반복할 횟수 저장할 곳 == aaa파일에서 72
	int colorIndex = 0;
	Vector3f_t* xyz = NULL;
	RGBA_t* colorArr = NULL;
	int* vertexIndex = NULL;
	RGBA_t tempColor = { 0, };

	char element[9];
	char vertex[7];

	for (lineCounter = 0; lineCounter < 4; lineCounter++)
	{
		if (NULL == fgets(buf, BUFFER_SIZE, fp))
		{
			return -1;
		}
	}

	sscanf(buf, "%s %s %d", element, vertex, &elementVertex);
	printf("%s\n", buf); //불러온거 확인용
	printf("%d\n", elementVertex);

	xyz = malloc(elementVertex * sizeof(Vector3f_t));
	colorArr = calloc(elementVertex / 8, sizeof(RGBA_t));
	vertexIndex = calloc(elementVertex / 8, sizeof(int));

	for (; lineCounter < 13; lineCounter++)
	{
		if (NULL == fgets(buf, BUFFER_SIZE, fp))
		{
			return -1;
			goto PROC_END;
		}
	}
	printf("%s\n", buf);
	StoreVertex(xyz, fp, &colorIndex, colorArr, vertexIndex, elementVertex);

	// --- file in end ---
	int vertexCount = 0;

	for (size_t i = 0; i < colorIndex; i++)
	{
		printf("\nvoxel%llu\n", i);
		for (size_t j = 0; j < 8; j++)
		{
			printf("    %f %f %f\n", xyz[8 * i + j].X, xyz[8 * i + j].Y, xyz[8 * i + j].Z);
			vertexCount++;
		}
	}
	printf("\nVertex count : % d\nVoxel count : % d\n", vertexCount, colorIndex);

	fclose(fp);

	//바이너리 쓰기// --- file write ---

	FILE* w_out = fopen("../resource/testio.kdhr", "wb");
	if (w_out == NULL)
	{
		printf("파일 생성 실패\n");
		return -1;
	}

	const char* w_HEAD_STR = "\0\0\0\0KDHR";
	//float* w_vector = malloc(sizeof(float) * 4 * 16);
	char* w_writeBuffer = malloc(MAX_FILE_SIZE);
	size_t w_vertexCount;
	memcpy(w_writeBuffer, w_HEAD_STR, 8);
	Vector3f_t* w_arr = NULL;
	size_t w_fileSize = 8;

	Voxel_t* voxelArr = malloc(sizeof(Voxel_t) * colorIndex);
	for (int i = 0; i < colorIndex; i++)
	{
		SortVoxelArr(xyz + i, voxelArr + i); // 복쉘 포멧 정리
		printf("\n");
	}

	for (size_t i = 0; i < w_vertexCount; i++)
	{
		*(Vector4f_t*)w_writeBuffer = GetVector4f(w_arr[i].X, w_arr[i].Y, w_arr[i].Z, 1.F);
	}
	w_fileSize += sizeof(float) * 4 * 16;
	fwrite(w_writeBuffer, 1, w_fileSize, w_out);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			printf("W%f %f %f\n",
				w_arr[i].X,
				w_arr[i].Y,
				w_arr[i].Z);
		}
	}

	fclose(w_out);
	free(voxelArr);


PROC_END:
	free(vertexIndex);
	free(colorArr);
	free(xyz);

}

int main(void)
{
	VertexIO();

	return 0;
}
