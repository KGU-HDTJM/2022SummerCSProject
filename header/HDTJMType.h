#pragma once

// defualt
typedef unsigned char byte_t;
typedef unsigned short ushort_t;
typedef unsigned int uint_t;
typedef unsigned long long ull_t;
typedef enum { False, True } boolean_t;

// StackAlloc.h
#ifdef _WIN64
// x86_64
typedef long long Word_t;
#else
// x86
typedef int Word_t;
#endif

typedef struct
{
	char* SP, * BP;
	void* memBlock;
}Stack_t, * pStack_t;

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
			int X;
			int Y;
			int Z;
			int W;
		};
		struct
		{
			int V[4];
		};
	};
} Vector4i_t;

typedef struct
{
	union
	{
		struct
		{
			float _11;
			float _12;
      
			float _21;
			float _22;
		};
		struct
		{
			float M[2][2];
		};
	};
} Matrix2_t;

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

// Voxel Data
typedef struct
{
	Vector4f_t Vertex[8];
	int TextureId;
}Voxel_t;