#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>  

#define sizeY 3
#define sizeZ 5
#define sizeX 5
#define sizeMax sizeY * sizeZ * sizeX


void PrintMap(int* arr);
int IsFullFloor(int* arr, const int floor);
void BreakFullFloor(int* arr, const int floor);

int main()
{
	int tempMap[3][5][5] = //편하게 보라고 만든거
	{
		{
			{1,1,1,1,1},
			{1,1,1,1,1},
			{1,1,1,1,1},
			{1,1,1,1,1},
			{1,1,1,1,1},
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
	};
	int* map = (int*)malloc(sizeof(int) * sizeMax);
	for (int k = 0; k < sizeY; k++)
	{
		for (int j = 0; j < sizeZ; j++)
		{
			for (int i = 0; i < sizeX; i++)
			{
				map[(sizeZ * sizeX) * k + sizeX * j + i] = tempMap[k][j][i];
			}
		}
	}
	PrintMap(map);
	for (int i = 0; i < sizeY; i++)
	{
		if (IsFullFloor(map, i))
		{
			BreakFullFloor(map, i);
		}
	}
	PrintMap(map);
}

void PrintMap(int* arr)
{
	for (int k = 0; k < sizeY; k++)
	{
		for (int j = 0; j < sizeZ; j++)
		{
			for (int i = 0; i < sizeX; i++)
			{
				printf("%d ", arr[(sizeZ * sizeX) * k + sizeX * j + i]);
			}
			printf("\n");
		}
		printf("\n");
	}
}
int IsFullFloor(int* arr, const int floor)
{
	for (int i = 0; i < sizeZ * sizeX; i++)
	{
		if (arr[i + (sizeZ * sizeX) * floor] != 1)
			return 0;
	}
	return 1;
}
void BreakFullFloor(int* arr, const int floor)
{
	for (int i = floor; i < sizeY - 1; i++)
	{
		for (int j = 0; j < sizeZ * sizeX; j++)
		{
			arr[(sizeZ * sizeX) * i + j] = arr[(sizeZ * sizeX) * (i + 1) + j];
		}
	}
	for (int i = 0; i < sizeZ * sizeX; i++)
	{
		arr[(sizeZ * sizeX) * (sizeY - 1) + i] = 0;
	}
}
