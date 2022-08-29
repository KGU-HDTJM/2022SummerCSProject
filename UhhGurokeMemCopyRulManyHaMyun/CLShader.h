#pragma once
#ifndef CLSHADER_H
#define CLSHADER_H

#if defined(GPGPU_EXPORTS)
#define CL_API extern __declspec(dllexport)
#else
#define CL_API extern __declspec(dllimport)
#endif

typedef struct
{
    byte_t X;
    byte_t Y;
    byte_t Z;
    byte_t TextureID;
} VoxelPos_t;

typedef struct
{
    Vector3f_t Position;
    byte_t Red;
    byte_t Green;
    byte_t Blue;
    byte_t Alpha;
} Light_t;

typedef enum
{
    DEVICE_TYPE_GPU, DEVICE_TYPE_CPU
} eClDeviceType;

typedef enum
{
    KERNEL_VERTEX_SHADER, KERNEL_PIXEL_SHADER
} eKernelID_t;


CL_API int __fastcall InitCL(void* textureBuffer, size_t textureWidth, size_t textureHeight, size_t texturePixelSize);
CL_API size_t __fastcall ClVertexShader(Vector3f_t* outPolygonArr,
    Vector2f_t* outTextureCoord, cl_mem* outNormalBuffer,
    VoxelPos_t* voxelPositionArr, size_t length, Matrix4_t* transMatrix, Vector3i_t* gridSize );
//CL_API void ClPixelShader(void);
CL_API void __fastcall  ReleaseCL(void);

#endif