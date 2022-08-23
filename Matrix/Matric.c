#include <math.h>
#include "HDTJMType.h"
#include "LinearAlgebra.h"

#define AXISX 1
#define AXISY 2
#define AXISZ 3

void Trans(Vector4f_t transVector)
{
	Matrix4_t mat;
	mat._11 = 1; mat._12 = 0; mat._13 = 0; mat._14 = transVector.X;
	mat._21 = 0; mat._22 = 1; mat._23 = 0; mat._24 = transVector.Y;
	mat._31 = 0; mat._32 = 0; mat._33 = 1; mat._34 = transVector.Z;
	mat._41 = 0; mat._42 = 0; mat._43 = 0; mat._44 = 1;

}


void Rot(Vector4f_t transVector,int axis,int angle)
{	
	Matrix4_t mat;
	if (axis == AXISX) 
	{
		mat._11 = 1;		mat._12 = 0;		   mat._13 = 0;		      mat._14 = 0;
		mat._21 = 0;		mat._22 = cos(angle);  mat._23 = -sin(angle); mat._24 = 0;
		mat._31 = 0;		mat._32 = sin(angle);  mat._33 = cos(angle);  mat._34 = 0;
		mat._41 = 0;		mat._42 = 0;		   mat._43 = 0;			  mat._44 = 1;
	}
	else if (axis == AXISY)
	{
		mat._11 = cos(angle);mat._12 = 0;		   mat._13 = sin(angle);  mat._14 = 0;
		mat._21 = 0;		 mat._22 = 1;		   mat._23 = 0;			  mat._24 = 0;
		mat._31 = sin(angle);mat._32 = 0;		   mat._33 = cos(angle);  mat._34 = 0;
		mat._41 = 0;		 mat._42 = 0;		   mat._43 = 0;			  mat._44 = 1;
	}
	else if (axis == AXISZ)
	{
		mat._11 = cos(angle);mat._12 = -sin(angle);mat._13 = 0;			  mat._14 = 0;
		mat._21 = sin(angle);mat._22 = cos(angle); mat._23 = 0;			  mat._24 = 0;
		mat._31 = 0;		 mat._32 = 0;		   mat._33 = 0;			  mat._34 = 1;
		mat._41 = 0;		 mat._42 = 0;		   mat._43 = 0;			  mat._44 = 1;
	}
	
}


void Scale(float Scale)
{
	Matrix4_t mat;
	mat._11 = Scale;	mat._12 = 0;		   mat._13 = 0;		    mat._14 = 0;
	mat._21 = 0;		mat._22 = Scale;	   mat._23 = 0;			mat._24 = 0;
	mat._31 = 0;		mat._32 = 0;		   mat._33 = Scale;		mat._34 = 0;
	mat._41 = 0;		mat._42 = 0;		   mat._43 = 0;			mat._44 = 1;
}

void Nomalization(Vector4f_t vector)
{
	float vectorSize = sqrt(pow(vector.X, 2)
							+ pow(vector.Y, 2)
							+ pow(vector.Z, 2)
							);

	Vector4f_t nomalVector;
	nomalVector.X = vector.X / vectorSize;
	nomalVector.Y = vector.Y / vectorSize;
	nomalVector.Z = vector.Z / vectorSize;

}

void Projection(,float w, float h)
