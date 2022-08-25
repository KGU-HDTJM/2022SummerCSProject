#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include "HDTJMType.h"
#include "LinearAlgebra.h"

#define AXISX 1
#define AXISY 2
#define AXISZ 3

Matrix4_t Trans(Vector4f_t transVector)
{
	Matrix4_t mat;
	mat._11 = 1; mat._12 = 0; mat._13 = 0; mat._14 = transVector.X;
	mat._21 = 0; mat._22 = 1; mat._23 = 0; mat._24 = transVector.Y;
	mat._31 = 0; mat._32 = 0; mat._33 = 1; mat._34 = transVector.Z;
	mat._41 = 0; mat._42 = 0; mat._43 = 0; mat._44 = 1;

	return mat;

}


Matrix4_t Rot(int axis,int angle)
{	
	Matrix4_t mat ;
	if (axis == AXISX) 
	{
		mat._11 = 1.F;		mat._12 = 0.F;		   mat._13 = 0.F;		  mat._14 = 0.F;
		mat._21 = 0.F;		mat._22 = cos(angle);  mat._23 = -sin(angle); mat._24 = 0.F;
		mat._31 = 0.F;		mat._32 = sin(angle);  mat._33 = cos(angle);  mat._34 = 0.F;
		mat._41 = 0.F;		mat._42 = 0.F;		   mat._43 = 0.F;		  mat._44 = 1.F;
	}
	else if (axis == AXISY)
	{
		mat._11 = cos(angle);mat._12 = 0.F;		   mat._13 = sin(angle);  mat._14 = 0.F;
		mat._21 = 0.F;		 mat._22 = 1.F;		   mat._23 = 0.F;		  mat._24 = 0.F;
		mat._31 = sin(angle);mat._32 = 0.F;		   mat._33 = cos(angle);  mat._34 = 0.F;
		mat._41 = 0.F;		 mat._42 = 0.F;		   mat._43 = 0.F;		  mat._44 = 1.F;
	}
	else if (axis == AXISZ)
	{
		mat._11 = cos(angle);mat._12 = -sin(angle);mat._13 = 0.F;		  mat._14 = 0.F;
		mat._21 = sin(angle);mat._22 = cos(angle); mat._23 = 0.F;		  mat._24 = 0.F;
		mat._31 = 0.F;		 mat._32 = 0.F;		   mat._33 = 0.F;		  mat._34 = 1.F;
		mat._41 = 0.F;		 mat._42 = 0.F;		   mat._43 = 0.F;		  mat._44 = 1.F;
	}
	return mat;
}


Matrix4_t Scale(float Scale)
{
	Matrix4_t mat;
	mat._11 = Scale;	mat._12 = 0.F;		   mat._13 = 0.F;		    mat._14 = 0.F;
	mat._21 = 0.F;		mat._22 = Scale;	   mat._23 = 0.F;			mat._24 = 0.F;
	mat._31 = 0.F;		mat._32 = 0.F;		   mat._33 = Scale;			mat._34 = 0.F;		
	mat._41 = 0.F;		mat._42 = 0.F;		   mat._43 = 0.F;			mat._44 = 1.F;

	return mat;
}



Vector4f_t  Nomalization(Vector4f_t vector)
{
	float vectorSize = sqrt(pow(vector.X, 2)
							+ pow(vector.Y, 2)
							+ pow(vector.Z, 2)
							);

	Vector4f_t nomalVector;
	nomalVector.X = vector.X / vectorSize;
	nomalVector.Y = vector.Y / vectorSize;
	nomalVector.Z = vector.Z / vectorSize;
	nomalVector.W = 1;
	return nomalVector;

}

Vector4f_t VectorCross(Vector4f_t vector1, Vector4f_t vector2)
{
	Vector4f_t out;
	out.X = vector1.Y* vector2.Z - vector1.Z*vector2.Y;
	out.Y = vector1.Z * vector2.X - vector1.X * vector2.Z;
	out.Z = vector1.X * vector2.Y - vector1.Y * vector2.X;
}
//-----------------------------------------------------------

//Matrix4_t GetWorldTransRotScale(float scale,int axis, int angle,float transX,float transY , float transZ )
//{
//	Vector4f_t trans;
//	trans.X = transX;
//	trans.Y = transY;
//	trans.Z = transZ;
//
//	Matrix4_t sMat = Scale(scale);
//	Matrix4_t rMat = Rot(axis, angle);
//	Matrix4_t tMat = Trans(trans);
//	
//	Matrix4_t rsMat = MulMatrix4(&rMat, &sMat);
//	Matrix4_t retMAt = MulMatrix4(&tMat, &rsMat);
//	return retMAt;
//}

Matrix4_t GetLookatMat(float eyeX,float eyeY,float eyeZ,float centerX, float centerY,float centerZ,float upX,float upY,float upZ)
{

	Vector4f_t forward;
	forward.X = eyeX - centerX;
	forward.Y = eyeY - centerY;
	forward.Z = eyeZ - centerZ;
	forward = Nomalization(forward);
	Vector4f_t upVector;
	upVector.X = upX;
	upVector.Y = upY;
	upVector.Z = upZ;
	upVector = forward = Nomalization(upVector);
	Vector4f_t side = VectorCross(upVector, forward);
	
	Matrix4_t matEye;
	matEye._11 = 1.F; matEye._12 = 0.F; matEye._13 = 0.F; matEye._14 = 0.F;
	matEye._21 = 0.F; matEye._22 = 1.F; matEye._23 = 0.F; matEye._24 = 0.F;
	matEye._31 = 0.F; matEye._32 = 0.F; matEye._33 = 1.F; matEye._34 = 0.F;
	matEye._41 = eyeX; matEye._42 = eyeY; matEye._43 = eyeZ; matEye._44 = 1.F;


	Matrix4_t matSideUp;
	matSideUp._11 = side.X; matSideUp._12 = -upVector.X; matSideUp._13 = forward.X; matSideUp._14 = 0.F;
	matSideUp._21 = side.Y; matSideUp._22 = -upVector.Y; matSideUp._23 = forward.Y; matSideUp._24 = 0.F;
	matSideUp._31 = side.Z; matSideUp._32 = -upVector.Z; matSideUp._33 = forward.Z; matSideUp._34 = 0.F;
	matSideUp._41 = 0.F; matSideUp._42 = 0.F; matSideUp._43 = 0.F; matSideUp._44 = 1.F;

	Matrix4_t retMat = MulMatrix4(&matSideUp, &matEye);
	return retMat;
}

Matrix4_t GetFrustumMat(float left, float right,float bottom ,float top ,float near,float far)
{
	Matrix4_t mat;
	mat._11 = (2*near)/(right - left);		mat._12 = 0.F;		   mat._13 = (right+left)/(right - left);mat._14 = 0.F;
	mat._21 = 0.F;							mat._22 = (2 * near) / (top - bottom);		mat._23 = (top + bottom)/(top - bottom); mat._24 = 0.F;
	mat._31 = 0.F;							mat._32 = 0.F;		    mat._33 = -(far + near) / (far - near);  mat._34 = -2 * (near + far) / (far - near);
	mat._41 = 0.F;							mat._42 = 0.F;		   mat._43 = -1.F;			  mat._44 = 0.F;
	return mat;
}

Matrix4_t GetViewPortMat(int left,int bottom ,int width,int height)
{
	Matrix4_t mat;
	mat._11 = width * 0.5F;		mat._12 = 0.F;		   mat._13 = 0.F; mat._14 = left+width*0.5F;
	mat._21 = 0.F;				mat._22 = height*0.5F; mat._23 = 0.F; mat._24 = 0.F;
	mat._31 = 0.F;				mat._32 = 0.F;		   mat._33 = 0.F; mat._34 = bottom + height*0.5F;
	mat._41 = 0.F;				mat._42 = 0.F;		   mat._43 = -1.F;			  mat._44 = 0.F;
	return mat;
}

void examplesUse(void)
{
	Vector4f_t exampelVector;
	float scale = 0; int axis = 0; int angle = 0; float transX = 0; float transY = 0; float transZ = 0;
	//크기 / 변환 축 / 각 / 좌표 이동 X , Y , Z
	Matrix4_t matTRS = GetWorldTransRotScale(scale, axis, angle, transX, transY, transZ);
	MulVectorMatrix4(&exampelVector, &exampelVector,1,&matTRS);

	float eyeX = 0; float eyeY = 0; float eyeZ = 0; 
	float centerX = 0; float centerY = 0; float centerZ = 0; 
	float upX = 0; float upY = 0; float upZ = 0;
	// eye X , Y , Z  시점 center X, Y ,Z 보는 방향 , up X Y Z 시점과 보는 방향에서 수직으로 위로 보는 방향
	Matrix4_t matLook = GetLookatMat(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
	MulVectorMatrix4(&exampelVector, &exampelVector, 1,&matLook);

	float projectionLeft = 0; float projectionRight = 0; float projectionBottom = 0; float  projectionTop = 0;
	float near = 0; float far = 0;
	//left,right,bottom,top 물체 입체적으로 나타는 공간의 형태
	//near : 카메라 부터 어디서부터 볼지 far 어디까지 볼지 
	Matrix4_t matProjection = GetFrustumMat(projectionLeft, projectionRight, projectionBottom, projectionTop, near, far);
	MulVectorMatrix4(&exampelVector, &exampelVector, 1, &matProjection);

	int screenLeft = 0; int screenBottom = 0; int screenWidth = 0; int screenHeight = 0;
	//우리가 보게 될 2차원 window 사이즈
	Matrix4_t matView = GetViewPortMat(screenLeft, screenBottom, screenWidth, screenHeight);
	MulVectorMatrix4(&exampelVector, &exampelVector, 1, &matView);
}


int main(void)
{
	return 0;
}