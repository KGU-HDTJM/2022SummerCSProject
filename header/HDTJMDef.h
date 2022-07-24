#pragma once
#include "HDTJMType.h"

#ifdef _WIN64
// x86_64
#define GetAlignedSize(size) ((size + 7) & ~7)
#else
// x86
#define GetAlignedSize(size) ((size + 3) & ~3)
#endif


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
	(v3B).X * (v3B).X + (v3A).Y * (v3B).Y + (v3A).Z * (v3B).Z\
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
	(v).X / scalar, (v).Y / scalar, (v).Z / scalar\
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

