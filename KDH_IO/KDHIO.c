/*
* ���� ����
* 1. ������ ���� .ply ���Ͽ��� Voxel ����
* 2. ���簡 ���� ���� ������ ����
*
* 3-1. ���ĵ� ������ ������ ������ ���˿� �°� ����(.kdhr). �������� ��� binary�ξ���.
*		����� 8����Ʈ ���ڿ�, ũ�⸦ ��Ÿ���� ���ڴ� 8����Ʈ ����
* 3-2. header������ ���� �� �ִ� ����� ####head, ....head, ##vertex Offset Length
* =============================== ����, ���� (�ʿ��ϸ� ����)==========================================
* 4. (�������� �Բ�) �����̰� ����� �ִ� ���� ���� ������ ������ ������ �迭 ����� byte������ ���� ����,
*		ȸ���� ������ -1, ������ ��� ����� ������(�����߽� ��ǥ����Ƽ�, ����� ������ ����)
* ====================================================================================================
* 5.  .kdhr������ ��������.
* ���� �б�
* 1. .kdhr���� �о
* 2. ���ǵ� ����ü ä���
* 3. ����ü ����
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
	if (fp == NULL) //���Ͼ����� ����ó��
	{
		printf("���� ����.\n");
		return -1;
	}
	int lineCounter = 0;
	int elementVertex = 0; //�ݺ��� Ƚ�� ������ �� == aaa���Ͽ��� 72
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
	printf("%s\n", buf); //�ҷ��°� Ȯ�ο�
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

	//���̳ʸ� ����// --- file write ---

	FILE* w_out = fopen("../resource/testio.kdhr", "wb");
	if (w_out == NULL)
	{
		printf("���� ���� ����\n");
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
		SortVoxelArr(xyz + i, voxelArr + i); // ���� ���� ����
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
