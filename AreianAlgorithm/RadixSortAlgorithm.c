#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "LinearAlgebra.h"

#define EPSILON_F 1.19e-07F

#define EqualFloat(a, b) (fabs((a) - (b)) < EPSILON_F)
#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Max(a, b) ((a) < (b) ? (b) : (a))

typedef enum eAxis {
	AxisX, AxisY, AxisZ, AxisW
} eAxis_t;


void RadixSortVecterbyAxis(Vector4f_t* arr, int length, eAxis_t stdAxis)
{
	int zeroes = 0;

	Vector4f_t* tempArray = (Vector4f_t*)malloc(sizeof(Vector4f_t) * length);
	Vector4f_t* wArray = tempArray;

	for (int radix = 1; radix ; radix <<= 1)
	{
		Vector4i_t* iArray = (Vector4i_t*)arr;

		int count0 = 0;
		int count1 = 0;

		zeroes = 0;
		for (int i = 0; i < length; i++)
		{
			zeroes += !(iArray[i].V[stdAxis] & radix);
		}

		count1 = zeroes;

		for (int i = 0; i < length; i++)
		{
			if (iArray[i].V[stdAxis] & radix)
			{
				wArray[count1++] = arr[i];
			}
			else
			{
				wArray[count0++] = arr[i];
			}
		}

		for (int i = 0; i < length; i++)
		{
			arr[i] = wArray[i];
		}
	}

	if (zeroes < length)
	{
		memcpy(wArray + (length - zeroes), arr, zeroes * sizeof(Vector4f_t));

		for (int d = 0, i = length - 1; i >= zeroes; i--, d++)
		{
			wArray[d] = arr[i];
		}
		memcpy(arr, wArray, length * sizeof(float));
	}
	free(tempArray);
}


void PrintVertex4f(Vector4f_t vertex)
{
	printf("%.1f %.1f %.1f %.1f\n", vertex.X, vertex.Y, vertex.Z, vertex.W);
}
int main(int argc, char** argv)
{
	Vector4f_t vertex4[5] = {
		{0.4,0.5,0.6,0.7},
		{1.6,1.7,1.8,1.9},
		{0.0,0.1,0.2,0.3},
		{0.8,0.9,1.0,1.1},
		{1.2,1.3,1.4,1.5},
	};

	RadixSortVecterbyAxis(vertex4, 5, AxisZ);
	for (int i = 0; i < 5; i++)
	{
		PrintVertex4f(vertex4[i]);
	}
	return 0;
}