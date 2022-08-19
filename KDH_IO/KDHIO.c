#pragma warning (disable : 6031)
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HDTJMType.h"
#include "KDHIO.h"
#define BUFFER_SIZE 0x80
#define GetRGBA(red, green, blue, alpha)\
	(RGBA_t){ (byte_t)red, (byte_t)green, (byte_t)blue, (byte_t)alpha };
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
		(* colorIndex)++; // 1(0 -> 1), 
	CMP_ELEMENTS:
		for (k = 0; k < vertexIndexArr[j]; k++) // 0
		{
			if (outVertex[8 * j + k].X == x 
				&& outVertex[8 * j + k].Y == y 
				&& outVertex[8 * j + k].Z == z )
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


int main(void)
{
	int ret = 0;

	int lineCounter = 0;
	int element_vertex = 0; //�ݺ��� Ƚ�� ������ �� == aaa���Ͽ��� 72
	int colorIndex = 0;
	Vector3f_t* xyz = NULL;
	RGBA_t* colorArr = NULL;
	int* vertexIndex = NULL;
	RGBA_t tempColor = { 0, };

	char element[9];
	char vertex[7];



	FILE* fp = fopen("TetrisBlock1.ply", "r"); //���� ����
	if (fp == NULL) //���Ͼ����� ����ó��
	{
		printf("���� ����.\n");
		return -1;
	}

	for (lineCounter = 0; lineCounter < 4; lineCounter++)
	{
		if (NULL == fgets(buf, BUFFER_SIZE, fp))
		{
			return -1;
		}
	}

	sscanf(buf, "%s %s %d", element, vertex, &element_vertex);
	printf("%s\n", buf); //�ҷ��°� Ȯ�ο�
	printf("%d\n", element_vertex);

	xyz = malloc(element_vertex * sizeof(Vector3f_t));
	colorArr = calloc(element_vertex / 8, sizeof(RGBA_t));
	vertexIndex = calloc(element_vertex / 8, sizeof(int));

	for (; lineCounter < 13; lineCounter++)
	{
		if (NULL == fgets(buf, BUFFER_SIZE, fp))
		{
			return -1;
			goto PROC_END;
		}
	}
	printf("%s\n", buf);
	StoreVertex(xyz, fp, &colorIndex, colorArr, vertexIndex, element_vertex);

	// --- file in end ---
	int vertexCount = 0;
	
	for (size_t i = 0; i < colorIndex * 8; i++)
	{
		printf("{%.2fF ,%.2fF ,%.2fF}\t\n", xyz[i].X, xyz[i].Y, xyz[i].Z);
		vertexCount++;
		if ( ((i+1) % 8) == 0 )printf("\n");
	}
	printf("\nVertex count : % d\nVoxel count : % d\n", vertexCount, colorIndex);

	PROC_END:
	free(vertexIndex);
	free(colorArr);
	free(xyz);

	fclose(fp);//���� �ݱ�

	return ret;
}