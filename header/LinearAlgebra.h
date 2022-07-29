#pragma once

#include "HDTJMType.h"

#if defined LINEAR_ALGEBRA_EXPORTS
#define LINEAR_ALGEBRA_API __declspec(dllexport)
#else
#define LINEAR_ALGEBRA_API __declspec(dllimport)
#endif

/* 4Â÷¿ø º¤ÅÍ¶û 4 x 4 Çà·Ä »ç½Ç»ó 3Â÷¿ø º¤ÅÍ¿Í 3 x 3 Çà·Ä ¿¬»êÀÓ, Çà·Ä °ö »©°í */
extern LINEAR_ALGEBRA_API Vector3f_t __cdecl Cross3f(const Vector3f_t* a, const Vector3f_t* b);
extern LINEAR_ALGEBRA_API Vector4f_t __cdecl Cross4f(const Vector4f_t* a, const Vector4f_t* b);
extern LINEAR_ALGEBRA_API Vector3f_t __cdecl Normalize3(const Vector3f_t* a);
extern LINEAR_ALGEBRA_API Vector4f_t __cdecl Normalize4(const Vector4f_t* a);
extern LINEAR_ALGEBRA_API Matrix3_t __cdecl MulMatrix3(const Matrix3_t* mA, const Matrix3_t* mB);
extern LINEAR_ALGEBRA_API Matrix4_t __cdecl MulMatrix4(const Matrix4_t* mA, const Matrix4_t* mB);
extern LINEAR_ALGEBRA_API void __cdecl MulVectorMatrix3(Vector3f_t* out, const Vector3f_t* arr, unsigned long long length, const Matrix3_t* m);
extern LINEAR_ALGEBRA_API void __cdecl MulVectorMatrix4(Vector4f_t* out, const Vector4f_t* arr, unsigned long long length, const Matrix4_t* m);
