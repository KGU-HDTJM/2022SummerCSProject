#pragma once
#ifndef LINEARALGEBRA_H
#define LINEARALGEBRA_H

#if defined LINEAR_ALGEBRA_EXPORTS
#define LINEAR_ALGEBRA_API __declspec(dllexport)
#else
#define LINEAR_ALGEBRA_API __declspec(dllimport)
#endif

// LinearAlgebra.h
typedef struct
{
	union
	{
		struct
		{
			int X;
			int Y;
		};
		struct
		{
			int V[2];
		};
	};
} Vector2i_t;

typedef struct
{
	union
	{
		struct
		{
			float X;
			float Y;
		};
		struct
		{
			float V[2];
		};
	};
} Vector2f_t;

typedef struct
{
	union
	{
		struct
		{
			int X;
			int Y;
			int Z;
		};
		struct
		{
			int V[3];
		};
	};
} Vector3i_t;

typedef struct
{
	union
	{
		struct
		{
			float X;
			float Y;
			float Z;
		};
		struct
		{
			float V[3];
		};
	};
} Vector3f_t;

typedef struct
{
	union
	{
		struct
		{
			float X;
			float Y;
			float Z;
			float W;
		};
		struct
		{
			float V[4];
		};
	};
} Vector4f_t;

typedef struct
{
	union
	{
		struct
		{
			float _11;
			float _12;
			float _13;

			float _21;
			float _22;
			float _23;

			float _31;
			float _32;
			float _33;
		};
		struct
		{
			float M[3][3];
		};
	};
} Matrix3_t;

typedef struct
{
	union
	{
		struct
		{
			float _11;
			float _12;
			float _13;
			float _14;

			float _21;
			float _22;
			float _23;
			float _24;

			float _31;
			float _32;
			float _33;
			float _34;

			float _41;
			float _42;
			float _43;
			float _44;
		};
		struct
		{
			float M[4][4];
		};
	};
} Matrix4_t;

// Get Vector by scalar
#define GetVector2i(x, y) ( (Vector2i_t){ (int)(x), (int)(y) } )
#define GetVector2f(x, y) ( (Vector2f_t){ (float)(x), (float)(y) } )
#define GetVector3i(x, y, z) ( (Vector3i_t){ (int)(x), (int)(y), (int)(z) } )
#define GetVector3f(x, y, z) ( (Vector3f_t){ (float)(x), (float)(y), (float)(z) } )
#define GetVector4f(x, y, z, w) ( (Vector4f_t){ (float)(x), (float)(y), (float)(z), (float)(w) } )

// Get Matrix by scalar
#define GetMatrix3\
(\
	_11, _12, _13,\
	_21, _22, _23,\
	_31, _32, _33\
)\
( (Matrix3_t) \
{\
	(_11), (_12), (_13),\
	(_21), (_22), (_23),\
	(_31), (_32), (_33)\
} )

#define GetMatrix4\
(\
	_11, _12, _13, _14,\
	_21, _22, _23, _24,\
	_31, _32, _33, _34,\
	_41, _42, _43, _44\
)\
( (Matrix4_t) \
{\
	(_11), (_12), (_13), (_14),\
	(_21), (_22), (_23), (_24),\
	(_31), (_32), (_33), (_34),\
	(_41), (_42), (_43), (_44),\
} )

// Simple calculations
// Vector4f_t ȣȯ
#define Dot3f(v3A, v3B) ( (float)\
(\
	(float)(v3B).X * (float)(v3B).X + (float)(v3A).Y * (float)(v3B).Y + (float)(v3A).Z * (float)(v3B).Z\
) )

#define Dot4f(v4A, v4B) ( (float)\
(\
	(v4A).X * (v4B).X + (v4A).Y * (v4B).Y + (v4A).Z * (v4B).Z + (v4A).W * (v4B).W\
) )

#define Sum3f(v3A, v3B) ( (Vector3f_t)\
{\
	(v3A).X + (v3B).X, (v3A).Y + (v3B).Y, (v3A).Z + (v3B).Z\
})

#define Sum4f(v4A, v4B) ( (Vector4f_t)\
{\
	(v4A).X + (v4B).X, (v4A).Y + (v4B).Y, (v4A).Z + (v4B).Z, (v4A).W ^ (v4B).W\
} )

#define Sub3f(v3A, v3B) ( (Vector3f_t)\
{\
	(v3A).X - (v3B).X, (v3A).Y - (v3B).Y, (v3A).Z - (v3B).Z\
})

#define Sub4f(v4A, v4B) ( (Vector4f_t)\
{\
	(v4A).X - (v4B).X, (v4A).Y - (v4B).Y, (v4A).Z - (v4B).Z, (v4A).W ^ (v4B).W\
} )

#define MulScalar3f(scalar, v) ( (Vector3f_t)\
{\
	(v).X * scalar, (v).Y * scalar, (v).Z * scalar\
})

#define MulScalar4f(scalar, v) ( (Vector4f_t)\
{\
	(v).X * scalar, (v).Y * scalar, (v).Z * scalar, (v).W\
})

#define DivScalar3f(scalar, v) ( (Vector3f_t)\
{\
	(float)(v).X / scalar, (float)(v).Y / scalar, (float)(v).Z / scalar\
})

#define DivScalar4f(scalar, v) ( (Vector4f_t)\
{\
	(v).X / scalar, (v).Y / scalar, (v).Z / scalar, (v).W\
})

// Matrix4_t ȣȯ��
#define Det3(mat3) ( (float)\
(\
	((mat3)._11 * ((mat3)._22 * (mat3)._33 - (mat3)._23 * (mat3)._32))\
	- ((mat3)._12 * ((mat3)._21 * (mat3)._33 - (mat3)._23 * (mat3)._31))\
	+ ((mat3)._13 * ((mat3)._21 * (mat3)._32 - (mat3)._22 * (mat3)._31))\
) )

#define Det4(mat4) ( (float)\
(\
	((mat4)._11 * ((mat4)._22 * (mat4)._33 * (mat4)._44 - (mat4)._24 * (mat4)._42))\
	- ((mat4)._12 * ((mat4)._21 * (mat4)._34 * (mat4)._43 - (mat4)._23 * (mat4)._34 * (mat4)._41))\
	+ ((mat4)._13 * ((mat4)._24 * (mat4)._31 * (mat4)._42 - (mat4)._22 * (mat4)._31 * (mat4)._44))\
	- ((mat4)._14 * ((mat4)._23 * (mat4)._32 * (mat4)._41 - (mat4)._21 * (mat4)_32 * (mat4)._43))\
) )

/* 4���� ���Ͷ� 4 x 4 ��� ��ǻ� 3���� ���Ϳ� 3 x 3 ��� ������, ��� �� ���� */
extern LINEAR_ALGEBRA_API Vector3f_t __fastcall Cross3f(const Vector3f_t* a, const Vector3f_t* b);
extern LINEAR_ALGEBRA_API Vector4f_t __fastcall Cross4f(const Vector4f_t* a, const Vector4f_t* b);
extern LINEAR_ALGEBRA_API Vector3f_t __fastcall Normalize3(const Vector3f_t* a);
extern LINEAR_ALGEBRA_API Vector4f_t __fastcall Normalize4(const Vector4f_t* a);
extern LINEAR_ALGEBRA_API Matrix3_t __fastcall MulMatrix3(const Matrix3_t* mA, const Matrix3_t* mB);
extern LINEAR_ALGEBRA_API Matrix4_t __fastcall MulMatrix4(const Matrix4_t* mA, const Matrix4_t* mB);
extern LINEAR_ALGEBRA_API void __fastcall MulVectorMatrix3(Vector3f_t* out, const Vector3f_t* arr, size_t length, const Matrix3_t* m);
extern LINEAR_ALGEBRA_API void __fastcall MulVectorMatrix4(Vector4f_t* out, const Vector4f_t* arr, size_t length, const Matrix4_t* m);

#endif
