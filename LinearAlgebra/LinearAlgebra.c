#include <math.h>

#define LINEAR_ALGEBRA_EXPORTS
#include "LinearAlgebra.h"

Vector3f_t __cdecl Cross3f(const Vector3f_t* a, const Vector3f_t* b)
{
	Vector3f_t n = GetVector3f(
		(a->Y * b->Z) - (a->Z * b->Y),
		(a->Z * b->X) - (a->X * b->Z),
		(a->X * b->Y) - (a->Y * b->X)
	);
	
	return n;
}

Vector4f_t __cdecl Cross4f(const Vector4f_t* a, const Vector4f_t* b)
{
	Vector4f_t n = GetVector4f(
		(a->Y * b->Z) - (a->Z * b->Y),
		(a->Z * b->X) - (a->X * b->Z),
		(a->X * b->Y) - (a->Y * b->X),
		a->W || b->W
	);

	return n;
}

Vector3f_t __cdecl Normalize3(const Vector3f_t* a)
{
	return DivScalar3f(sqrt(Dot3f(*a, *a)), *a);
}

Vector4f_t __cdecl Normalize4(const Vector4f_t* a)
{
	return DivScalar4f(sqrt(Dot4f(*a, *a)), *a);
}

Matrix3_t __cdecl MulMatrix3(const Matrix3_t* mA, const Matrix3_t* mB)
{
	Matrix3_t result;

	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			result.M[j][i] = 0;
			for (size_t k = 0; k < 3; k++)
			{
				result.M[i][j] += mA->M[i][k] * mB->M[k][j];
			}
		}
	}

	return result;
}

Matrix4_t __cdecl MulMatrix4(const Matrix4_t* mA, const Matrix4_t* mB)
{
	Matrix4_t result;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			result.M[j][i] = 0;
			for (size_t k = 0; k < 4; k++)
			{
				result.M[i][j] += mA->M[i][k] * mB->M[k][j];
			}
		}
	}

	return result;
}

void __cdecl MulVectorMatrix3(Vector3f_t* out, const Vector3f_t* arr, unsigned long long length, const Matrix3_t* m)
{
	if (out == arr)
	{
		Vector3f_t temp;
		for (size_t i = 0; i < length; i++)
		{
			temp = arr[i];
			for (size_t j = 0; j < 3; j++)
			{
				out[i].V[j] = 0;
				for (size_t k = 0; k < 3; k++)
				{
					out[i].V[j] += temp.V[k] * m->M[k][j];
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < length; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				out[i].V[j] = 0;
				for (size_t k = 0; k < 3; k++)
				{
					out[i].V[j] += arr[i].V[k] * m->M[k][j];
				}
			}
		}
	}
}

void __cdecl MulVectorMatrix4(Vector4f_t* out, const Vector4f_t* arr, unsigned long long length, const Matrix4_t* m)
{
	if (out == arr)
	{
		Vector4f_t temp;
		for (size_t i = 0; i < length; i++)
		{
			temp = arr[i];
			for (size_t j = 0; j < 4; j++)
			{
				out[i].V[j] = 0;
				for (size_t k = 0; k < 4; k++)
				{
					out[i].V[j] += temp.V[k] * m->M[k][j];
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < length; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				out[i].V[j] = 0;
				for (size_t k = 0; k < 4; k++)
				{
					out[i].V[j] += arr[i].V[k] * m->M[k][j];
				}
			}
		}
	}
}
